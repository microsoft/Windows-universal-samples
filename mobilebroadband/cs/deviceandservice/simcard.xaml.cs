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
        MobileBroadbandUiccApp currentUiccApp = null;

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
                    
                    
                    MobileBroadbandUiccAppsResult appsResult = await uicc.GetUiccAppsAsync();
                    System.Collections.ObjectModel.ObservableCollection<object> AppList = new System.Collections.ObjectModel.ObservableCollection<object>();
                    foreach (var uiccApp in appsResult.UiccApps)
                    {
                        ListBoxItem item = new ListBoxItem();
                        item.Name = uiccApp.Kind.ToString();
                        item.Content = uiccApp;
                        AppList.Add(item);
                    }

                    listUiccApps.ItemsSource = AppList;
                    if (AppList.Count > 0)
                    {
                        listUiccApps.SelectedIndex = 0;
                    }
                    else
                    {
                        sb.AppendLine("No UICC app found.");
                    }

                    txtUICCInformation.Text = sb.ToString();
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error:" + ex.Message, NotifyType.ErrorMessage);
            }
        }

        private void listUiccApps_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (null != listUiccApps.SelectedItem)
            {
                ListBoxItem item = listUiccApps.SelectedItem as ListBoxItem;
                currentUiccApp = (MobileBroadbandUiccApp)item.Content;
                btnGetRecord.IsEnabled = true;
                byte[] appIdByteBuffer = currentUiccApp.Id.ToArray();
                StringBuilder sb = new StringBuilder();
                sb.AppendLine("AppId:");
                foreach(var v in appIdByteBuffer)
                {
                    sb.Append(v.ToString("X") + ", ");
                }
                txtUICCAppId.Text = sb.ToString();
                
            }
            else
            {
                btnGetRecord.IsEnabled = false;
            }

            labelRecordInformation.Text = "";
        }

        private async void btnGetRecord_Click(object sender, RoutedEventArgs e)
        {
            List<uint> iccidFilePath = new List<uint>{ 0x3F00, 0x2FE2}; // The wellknown file path for IccId
            MobileBroadbandUiccAppRecordDetailsResult recordDetails = await currentUiccApp.GetRecordDetailsAsync(iccidFilePath);
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("Record Details");
            sb.AppendLine("Record Type:" + recordDetails.Kind.ToString());
            sb.AppendLine("Record Size:" + recordDetails.RecordSize.ToString());
            sb.AppendLine("Read result:");

            MobileBroadbandUiccAppReadRecordResult readRecord = await currentUiccApp.ReadRecordAsync(iccidFilePath, 1);
            sb.AppendLine(readRecord.Status.ToString());

            if (readRecord.Status == MobileBroadbandUiccAppOperationStatus.Success)
            {
                sb.AppendLine("IccId record content:");
                byte[] iccidByteArray = readRecord.Data.ToArray();
                foreach (var v in iccidByteArray)
                {
                    sb.Append(v.ToString("X") + ", ");
                }
            }

            labelRecordInformation.Text = sb.ToString();
        }
    }
}
