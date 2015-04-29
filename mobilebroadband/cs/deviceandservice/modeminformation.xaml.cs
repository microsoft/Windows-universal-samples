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
    public sealed partial class ModemInformation : Page
    {
        MainPage rootPage = MainPage.Current;
        public ModemInformation()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            btnRefresh_Click(null, null);
            base.OnNavigatedTo(e);
        }
        private void btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var modem = MobileBroadbandModem.GetDefault();
                StringBuilder sb = new StringBuilder();
                sb.AppendLine("Cellular Class: " + modem.DeviceInformation.CellularClass.ToString());
                sb.AppendLine("Current Radio Powerstate: " + modem.DeviceInformation.CurrentRadioState.ToString());
                sb.AppendLine("Custom Data Class: " + modem.DeviceInformation.CustomDataClass.ToString());
                sb.AppendLine("Supported Data Classes: " + modem.DeviceInformation.DataClasses.ToString());
                sb.AppendLine("Device ID: " + modem.DeviceInformation.DeviceId.ToString());
                sb.AppendLine("Device Type: " + modem.DeviceInformation.DeviceType.ToString());
                sb.AppendLine("Firmware Information: " + modem.DeviceInformation.FirmwareInformation);
                sb.AppendLine("Manufacturer: " + modem.DeviceInformation.Manufacturer);
                sb.AppendLine("IMEI (Mobile Equipment ID): " + modem.DeviceInformation.MobileEquipmentId);
                sb.AppendLine("Device Model: " + modem.DeviceInformation.Model);
                sb.AppendLine("Device Status: " + modem.DeviceInformation.NetworkDeviceStatus.ToString());
                sb.AppendLine("Suppported PINs: " + CheckForNullOrEmptyField(modem.DeviceInformation.PinManager.SupportedPins.ToString()));
                sb.AppendLine("Revision: " + modem.DeviceInformation.Revision);
                sb.AppendLine("Serial Number: " + modem.DeviceInformation.SerialNumber);
                sb.AppendLine("SIM ICCID: " + modem.DeviceInformation.SimIccId);
                sb.AppendLine("Subscriber ID: " + modem.DeviceInformation.SubscriberId);
                sb.AppendLine("Telephone Numbers: " + CheckForNullOrEmptyField(modem.DeviceInformation.TelephoneNumbers.ToString()));
                txtModemInformation.Text = sb.ToString();
                rootPage.NotifyUser("Success: Modem information loaded", NotifyType.StatusMessage);

            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error:" + ex.Message, NotifyType.ErrorMessage);
            }
        }
        private string CheckForNullOrEmptyField(object input)
        {
            string result = "empty";
            switch ((input.GetType().ToString().ToLower()))
            {
                case "string":
                    string val = (string)input;
                    if ((string.IsNullOrEmpty(val))||(string.IsNullOrWhiteSpace(val)))
                    {
                        result =val;
                    }
                    break;
                case "{system._comobject}":
                    break;
                default:
                    break;
            }


            return result; 
        }

    }
}
