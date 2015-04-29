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
    public sealed partial class SimCard : Page
    {
        MainPage rootPage = MainPage.Current;
        public SimCard()
        {
            this.InitializeComponent();
        }

        private async void btnGetUICC_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var modem = MobileBroadbandModem.GetDefault();
                MobileBroadbandModemConfiguration modemCfg = await modem.GetCurrentConfigurationAsync();
                MobileBroadbandUicc uicc = modemCfg.Uicc;
                if (uicc != null)
                {
                    StringBuilder sb = new StringBuilder();
                    sb.AppendLine("SIM Card ICCID:" + uicc.SimIccId);

                    sb.AppendLine("Application on SIM Card");
                    MobileBroadbandUiccAppsResult appsResult = await uicc.GetUiccAppsAsync();
                    foreach (var uiccApp in appsResult.UiccApps)
                    {
                        sb.AppendLine("Kind: " + uiccApp.Kind);
                    }
                    txtSIMInformation.Text = sb.ToString();
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error:" + ex.Message, NotifyType.ErrorMessage);
            }
        }
    }
}
