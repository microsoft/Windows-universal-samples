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
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using Windows.Devices.Custom;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class DeviceEvents
    {
        MainPage rootPage = MainPage.Current;

        // saved copy of the switch state, used to highlight which entries have changed
        bool[] previousSwitchValues = null;

        public DeviceEvents()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ClearSwitchStateTable();
            UpdateRegisterButton();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (cancelSource != null)
            {
                cancelSource.Cancel();
                cancelSource = null;
            }
        }

        private async void deviceEventsGet_Click(object sender, RoutedEventArgs e)
        {
            CustomDevice fx2Device = DeviceList.Current.GetSelectedDevice();

            bool[] switchStateArray = new bool[8];

            if (fx2Device == null)
            {
                rootPage.NotifyUser("Fx2 device not connected or accessible", NotifyType.ErrorMessage);
                return;
            }

            var button = (Button)sender;
            button.IsEnabled = false;

            var output = new byte[1];

            try
            {
                uint bytesRead = await fx2Device.SendIOControlAsync(Fx2Driver.ReadSwitches, null, output.AsBuffer());
                if (bytesRead == 0)
                {
                    rootPage.NotifyUser("Fx2 device returned 0 byte interrupt message.  Stopping", NotifyType.ErrorMessage);
                }
                else
                {
                    switchStateArray = CreateSwitchStateArray(output);
                    UpdateSwitchStateTable(switchStateArray);
                }
            }
            catch (Exception exception)
            {
                rootPage.NotifyUser(exception.ToString(), NotifyType.ErrorMessage);
            }
            button.IsEnabled = true;
        }

        private static bool[] CreateSwitchStateArray(byte[] output)
        {
            var switchStateArray = new bool[8];

            for (int i = 0; i < 8; i += 1)
            {
                switchStateArray[i] = (output[0] & (1 << i)) != 0;
            }

            return switchStateArray;
        }

        CancellationTokenSource cancelSource;
        byte[] switchMessageBuffer = new byte[1];

        private void deviceEventsBegin_Click(object sender, RoutedEventArgs e)
        {
            var fx2Device = DeviceList.Current.GetSelectedDevice();

            if (fx2Device == null)
            {
                rootPage.NotifyUser("Fx2 device not connected or accessible", NotifyType.ErrorMessage);
                return;
            }

            if (cancelSource == null)
            {
                ReadInterruptMessagesAsync();
            }

            UpdateRegisterButton();
        }

        private void deviceEventsCancel_Click(object sender, RoutedEventArgs e)
        {
            if (cancelSource != null)
            {
                cancelSource.Cancel();
            }
            UpdateRegisterButton();
        }

        private async void ReadInterruptMessagesAsync()
        {
            var fx2Device = DeviceList.Current.GetSelectedDevice();
            var switchMessageBuffer = new byte[1] { 0 };
            
            cancelSource = new CancellationTokenSource();

            bool failure = false;

            while (!cancelSource.IsCancellationRequested)
            {
                uint bytesRead;

                try
                {
                    bytesRead = await fx2Device.SendIOControlAsync(
                                                    Fx2Driver.GetInterruptMessage,
                                                    null,
                                                    switchMessageBuffer.AsBuffer()
                                                    ).AsTask(cancelSource.Token);
                }
                catch (TaskCanceledException)
                {
                    rootPage.NotifyUser("Pending GetInterruptMessage IO Control cancelled", NotifyType.StatusMessage);
                    failure = false;
                    break;
                }
                catch (Exception e)
                {
                    rootPage.NotifyUser("Error accessing Fx2 device:\n" + e.Message, NotifyType.ErrorMessage);
                    failure = true;
                    break;
                }

                if (bytesRead == 0)
                {
                    rootPage.NotifyUser("Fx2 device returned 0 byte interrupt message.  Stopping\n", NotifyType.ErrorMessage);
                    failure = true;
                    break;
                }

                var switchState = CreateSwitchStateArray(switchMessageBuffer);
                UpdateSwitchStateTable(switchState);

            }

            if (failure)
            {
                ClearSwitchStateTable();
            }

            cancelSource = null;
            UpdateRegisterButton();
        }

        private void UpdateRegisterButton()
        {
            deviceEventsBegin.IsEnabled = (cancelSource == null);
            deviceEventsCancel.IsEnabled = (cancelSource != null);
        }

        private void ClearSwitchStateTable()
        {
            deviceEventsSwitches.Inlines.Clear();
            previousSwitchValues = null;
        }

        private void UpdateSwitchStateTable(bool[] switchStateArray)
        {
            var output = deviceEventsSwitches;

            DeviceList.CreateBooleanTable(
                output.Inlines,
                switchStateArray,
                previousSwitchValues,
                "Switch Number",
                "Switch State",
                "off",
                "on"
                );
            previousSwitchValues = switchStateArray;
        }
    }
}
