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
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public static class Globals
    {
        // WARNING! This custom OUI is for demonstration purposes only.
        // OUI values are assigned by the IEEE Registration Authority.
        // Replace this custom OUI with the value assigned to your organization.
        public static readonly byte[] CustomOui = { 0xAA, 0xBB, 0xCC };
        public static readonly byte CustomOuiType = 0xDD;

        // OUI assigned to the Wi-Fi Alliance.
        public static readonly byte[] WfaOui = { 0x50, 0x6F, 0x9A };

        // OUI assigned to Microsoft Corporation.
        public static readonly byte[] MsftOui = { 0x00, 0x50, 0xF2 };

        public static readonly string strServerPort = "50001";
        public static readonly int iAdvertisementStartTimeout = 5000; // in ms
    }

    public static class Utils
    {
        private static async Task ShowPinToUserAsync(CoreDispatcher dispatcher, string strPin)
        {
            await dispatcher.RunTaskAsync(async () =>
            {
                var messageDialog = new MessageDialog($"Enter this PIN on the remote device: {strPin}");

                // Add commands
                messageDialog.Commands.Add(new UICommand("OK", null, 0));

                // Set the command that will be invoked by default 
                messageDialog.DefaultCommandIndex = 0;

                // Set the command that will be invoked if the user cancels
                messageDialog.CancelCommandIndex = 0;

                // Show the Pin 
                await messageDialog.ShowAsync();
            });
        }

        private static async Task<string> GetPinFromUserAsync(CoreDispatcher dispatcher)
        {
            return await dispatcher.RunTaskAsync(async () =>
            {
                var pinBox = new TextBox();
                var dialog = new ContentDialog()
                {
                    Title = "Enter Pin",
                    PrimaryButtonText = "OK",
                    Content = pinBox
                };
                await dialog.ShowAsync();
                return pinBox.Text;
            });
        }

        public static async void HandlePairing(CoreDispatcher dispatcher, DevicePairingRequestedEventArgs args)
        {
            using (Deferral deferral = args.GetDeferral())
            {
                switch (args.PairingKind)
                {
                    case DevicePairingKinds.DisplayPin:
                        await ShowPinToUserAsync(dispatcher, args.Pin);
                        args.Accept();
                        break;

                    case DevicePairingKinds.ConfirmOnly:
                        args.Accept();
                        break;

                    case DevicePairingKinds.ProvidePin:
                        args.Accept(await GetPinFromUserAsync(dispatcher));
                        break;
                }
            }
        }

        // Helper function to take the selected item in a combo box
        // and return its tag, which is assumed to be of type T.
        static public T GetSelectedItemTag<T>(ComboBox comboBox)
        {
            var element = (FrameworkElement)comboBox.SelectedItem;
            return (T)element.Tag;
        }

        // Function binding target for the "Send" button on a text box.
        static public bool CanSendMessage(string message, object connectedDevice)
        {
            return !String.IsNullOrEmpty(message) && connectedDevice != null;
        }

        // General-purpose function binding targets.
        static public bool IsNonNull(object o)
        {
            return o != null;
        }

        static public bool IsNonEmptyString(string value)
        {
            return !String.IsNullOrEmpty(value);
        }
    }
}
