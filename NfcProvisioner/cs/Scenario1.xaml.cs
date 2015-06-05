//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using Windows.Networking.Proximity;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace NfcProvisioner
{
    /// <summary>
    /// The NfcProvHeader is a prefix to each NFC provisioning message which contains information about the protocol version, message index, total message count, and etc.
    /// </summary>
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct NfcProvHeader
    {
        public byte version;
        public byte leading;
        public byte index;
        public byte total;
    }

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;
        private IStorageFile provisioningPackageFile = null;
        private CancellationTokenSource currentTransferCancelToken = null;

        const byte NFC_PROV_MESSAGE_CURRENT_VERSION = 0;
        const byte NFC_PROV_MESSAGE_LEADING_BYTE = 0xFF;
        const string NFC_PROV_MESSAGE_PROTOCOL_NAME = "Windows.ProvPlugins.Chunk";
        const long INVALID_PUBLISHED_MESSAGE_ID = -1;

        public Scenario1()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void Browse_Click(object sender, RoutedEventArgs e)
        {
            FileOpenPicker openPicker = new FileOpenPicker();

            // Filter *.ppkg files and only show provisioning package files when opening file picker.
            openPicker.FileTypeFilter.Add(".ppkg");
            provisioningPackageFile = await openPicker.PickSingleFileAsync();

            if (null == provisioningPackageFile)
            {
                rootPage.NotifyUser("No package was selected.", NotifyType.StatusMessage);
                ProvisioningPackagePathText.Text = "";
                TransferButton.Visibility = Visibility.Collapsed;
                CancelTransferButton.Visibility = Visibility.Collapsed;
            }
            else
            {
                rootPage.NotifyUser("One package was selected.", NotifyType.StatusMessage);
                ProvisioningPackagePathText.Text = provisioningPackageFile.Name;
                TransferButton.Visibility = Visibility.Visible;
            }
        }

        private async void Transfer_Click(object sender, RoutedEventArgs e)
        {
            TransferButton.IsEnabled = false;
            TransferButton.Visibility = Visibility.Collapsed;
            CancelTransferButton.Visibility = Visibility.Visible;

            try
            {
                currentTransferCancelToken = new CancellationTokenSource();
                await InternalTransferHandlerAsync(currentTransferCancelToken.Token);
                currentTransferCancelToken = null;
            }
            catch (System.OperationCanceledException /*exception*/)
            {
                rootPage.NotifyUser("Transfer has been cancelled.", NotifyType.StatusMessage);
            }
            catch (System.ArgumentException /*exception*/)
            {
                rootPage.NotifyUser("We couldn't finish the transfer. Tap the devices again and keep them close together.", NotifyType.ErrorMessage);
            }

            CancelTransferButton.Visibility = Visibility.Collapsed;
            TransferButton.Visibility = Visibility.Visible;
            TransferButton.IsEnabled = true;
        }

        private void CancelTransfer_Click(object sender, RoutedEventArgs e)
        {
            if (null != currentTransferCancelToken)
            {
                currentTransferCancelToken.Cancel();
            }
        }

        /// <summary>
        /// Function to convert the selected provisioning package into chunk and send it over NFC.
        /// </summary>
        /// <returns></returns>
        private async Task InternalTransferHandlerAsync(CancellationToken ct)
        {
            rootPage.NotifyUser("Tap device to transfer package.", NotifyType.StatusMessage);

            if (null == provisioningPackageFile)
            {
                rootPage.NotifyUser("Select a package before transferring.", NotifyType.ErrorMessage);
                return;
            }

            IBuffer buffer = await FileIO.ReadBufferAsync(provisioningPackageFile);
            if (null == buffer)
            {
                rootPage.NotifyUser("Something was wrong. Choose another package and retry again.", NotifyType.ErrorMessage);
                return;
            }

            ProximityDevice proximityDevice = ProximityDevice.GetDefault();
            if (null == proximityDevice)
            {
                rootPage.NotifyUser("You'll need to turn on NFC to transfer the package. Go to NFC settings.", NotifyType.ErrorMessage);
                return;
            }

            // Start to format the provisioning package into chunks, and transfer it to another device to be provisioned.
            await ConvertToNfcMessageAsync(buffer, proximityDevice, NfcMessageHandlerAsync, ct);

            rootPage.NotifyUser("Success!", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Function to handle the chunk message and push the NFC message to proximity device.
        /// </summary>
        /// <param name="proximityDevice">ProximityDevice instance to be used.</param>
        /// <param name="chuckData">IBuffer to chunk message.</param>
        private static async Task NfcMessageHandlerAsync(ProximityDevice proximityDevice, IBuffer chuckData, CancellationToken ct)
        {
            using (var publishedEvent = new AutoResetEvent(false))
            {
                long publishedMessageId = INVALID_PUBLISHED_MESSAGE_ID;

                publishedMessageId = proximityDevice.PublishBinaryMessage(
                    NFC_PROV_MESSAGE_PROTOCOL_NAME,
                    chuckData, (
                        Windows.Networking.Proximity.ProximityDevice device,
                        long messageId
                    ) =>
                    {
                        publishedEvent.Set();
                    });

                if (INVALID_PUBLISHED_MESSAGE_ID == publishedMessageId)
                {
                    throw new System.ArgumentException("Published message ID returned was invalid.");
                }

                // Wait for the event to be signaled on a threadpool thread so we don't block the UI thread.
                await Task.Run(() =>
                {
                    // Wait for the message transmitted handler to ensure the NFC message has been processed internally.
                    WaitHandle.WaitAny(new WaitHandle[] { publishedEvent, ct.WaitHandle });
                    ct.ThrowIfCancellationRequested();
                });
            }
        }

        /// <summary>
        /// Function to format provisioning package into chunk data with a NfcProvHeader prefix.
        /// </summary>
        /// <param name="content">IBuffer to the provisioning package content.</param>
        /// <param name="device">ProximityDevice instance passed in the messageHandler.</param>
        /// <param name="messageHandler">Delegate handler to consume the chunk data.</param>
        private static async Task ConvertToNfcMessageAsync(IBuffer content, ProximityDevice device, Func<ProximityDevice, IBuffer, CancellationToken, Task> messageHandlerAsync, CancellationToken ct)
        {
            var maxMessageSize = device.MaxMessageBytes;
            var headerSize = GetNfcProvHeaderSize();
            uint chunkSize = maxMessageSize - (uint)headerSize;

            // Calculate the chunk count needed for this transfer.
            uint chunks = content.Length / chunkSize;
            if ((content.Length % chunkSize) > 0)
            {
                ++chunks;
            }

            // Loop and compose the payload per chunk data.
            for (uint posToRead = 0; posToRead < content.Length; posToRead += chunkSize)
            {
                var dataWriter = new DataWriter();
                var header = new NfcProvHeader();

                header.version = NFC_PROV_MESSAGE_CURRENT_VERSION;  // Currently the supported version is 0x00.
                header.leading = NFC_PROV_MESSAGE_LEADING_BYTE;     // The leading byte should be always 0xFF.
                header.index = (byte)(posToRead / chunkSize);       // To specify which chunk to this message.
                header.total = (byte)chunks;                        // To specify the total message count for this transfer.

                int bufferSizeToAlloc = (int)maxMessageSize;
                if (header.total == (header.index + 1))
                {
                    bufferSizeToAlloc = headerSize + (int)(content.Length - posToRead);
                }

                // Write the prefix first and the chunk data of the provisioning package.
                dataWriter.WriteBytes(GetBytes(header));
                dataWriter.WriteBuffer(content, posToRead, (uint)(bufferSizeToAlloc - headerSize));

                // Invoke the delegate handler to handle the NFC message.
                await messageHandlerAsync(device, dataWriter.DetachBuffer(), ct);
            }
        }

        private static byte[] GetBytes(NfcProvHeader header)
        {
            int size = Marshal.SizeOf(header);
            byte[] arr = new byte[size];
            IntPtr ptr = Marshal.AllocHGlobal(size);

            Marshal.StructureToPtr(header, ptr, true);
            Marshal.Copy(ptr, arr, 0, size);
            Marshal.FreeHGlobal(ptr);

            return arr;
        }

        private static int GetNfcProvHeaderSize()
        {
            var header = new NfcProvHeader();
            return Marshal.SizeOf(header);
        }
    }
}