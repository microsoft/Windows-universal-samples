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
using Windows.UI.Xaml;

namespace SDKTemplate
{
    public sealed partial class DeviceIO
    {
        MainPage rootPage = MainPage.Current;

        public DeviceIO()
        {
            this.InitializeComponent();
        }

        private async void DeviceIOGet_Click(object sender, RoutedEventArgs e)
        {
            var fx2Device = DeviceList.Current.GetSelectedDevice();

            if (fx2Device == null)
            {
                rootPage.NotifyUser("Fx2 device not connected or accessible", NotifyType.ErrorMessage);
                return;
            }

            byte[] outputBuffer = new byte[1];

            try
            {
                await fx2Device.SendIOControlAsync(Fx2Driver.GetSevenSegmentDisplay,
                                                   null,
                                                   outputBuffer.AsBuffer());
                
                rootPage.NotifyUser($"The segment display value is {Fx2Driver.SevenSegmentToDigit(outputBuffer[0])}", NotifyType.StatusMessage);
            }
            catch (ArgumentException)
            {
                rootPage.NotifyUser("The segment display value is not yet initialized", NotifyType.StatusMessage);
            }
            catch (Exception exception)
            {
                rootPage.NotifyUser(exception.Message, NotifyType.ErrorMessage);
            }
        }


        private async void DeviceIOSet_Click(object sender, RoutedEventArgs e)
        {
            var fx2Device = DeviceList.Current.GetSelectedDevice();

            if (fx2Device == null)
            {
                rootPage.NotifyUser("Fx2 device not connected or accessible", NotifyType.ErrorMessage);
                return;
            }

            byte val = (byte)(DeviceIOSegmentInput.SelectedIndex + 1);

            byte[] input = new byte[] { Fx2Driver.DigitToSevenSegment(val) };

            try
            {
                await fx2Device.SendIOControlAsync(
                                    Fx2Driver.SetSevenSegmentDisplay,
                                    input.AsBuffer(),
                                    null
                                    );

                rootPage.NotifyUser($"The segment display is set to {val}", NotifyType.StatusMessage);
            }
            catch (Exception exception)
            {
                rootPage.NotifyUser(exception.Message, NotifyType.ErrorMessage);
            }
        }
    }
}
