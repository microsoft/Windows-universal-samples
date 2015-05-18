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
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Networking.NetworkOperators;
using Windows.Networking.Connectivity;
using SDKTemplate;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace MobileBroadband
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ModemDeviceServices : Page
    {
        MainPage rootPage = MainPage.Current;
        public ModemDeviceServices()
        {
            this.InitializeComponent();
        }


        private void btnDeviceService_Click(object sender, RoutedEventArgs e)
        {
            var modem = MobileBroadbandModem.GetDefault();
            var deviceService = modem.DeviceServices;
            if (deviceService != null)
            {
                StringBuilder sb = new StringBuilder();
                foreach (var d in deviceService)
                {
                    sb.AppendLine(String.Format("DeviceService ID:{0}, Read Allowed:{1}, Write Allowed:{2}", d.DeviceServiceId.ToString(), d.IsDataReadSupported.ToString(), d.IsDataWriteSupported.ToString()));
                }
                txtModemInformation.Text = sb.ToString();
            }
            else
            {
                rootPage.NotifyUser("No Device Services found.", NotifyType.ErrorMessage);

            }
        }
    }
}
