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
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.System.RemoteSystems;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario5_Capabilities : Page
    {
        private MainPage m_rootPage;
        public Scenario5_Capabilities()
        {
            this.InitializeComponent();
            this.DataContext = this;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            m_rootPage = MainPage.Current;
            m_rootPage.ValidateSystemDiscovery();
        }

        public ObservableCollection<RemoteSystem> SystemList => m_rootPage.systemList;

        private async Task ReportCapabilityAsync(RemoteSystem selectedSystem, string capability, TextBlock resultBlock)
        {
            bool isSupported = await selectedSystem.GetCapabilitySupportedAsync(capability);
            resultBlock.Text = isSupported ? "Yes" : "No";
        }

        private async void Capabilities_Click(object sender, RoutedEventArgs e)
        {
            UpdateStatus("Checking for capabilities...", NotifyType.StatusMessage);
            appServiceSupport.Text = "";
            launchUriSupport.Text = "";
            spatialEntitySupport.Text = "";

            RemoteSystem selectedSystem = SystemListComboBox.SelectedItem as RemoteSystem;
            if (selectedSystem != null && selectedSystem.Status == RemoteSystemStatus.Available)
            {
                // Check and report each capability.
                await ReportCapabilityAsync(selectedSystem, KnownRemoteSystemCapabilities.AppService, appServiceSupport);
                await ReportCapabilityAsync(selectedSystem, KnownRemoteSystemCapabilities.LaunchUri, launchUriSupport);
                await ReportCapabilityAsync(selectedSystem, KnownRemoteSystemCapabilities.SpatialEntity, spatialEntitySupport);

                UpdateStatus("Successfully checked capabilities.", NotifyType.StatusMessage);
            }
            else if (selectedSystem != null && selectedSystem.Status != RemoteSystemStatus.Available)
            {
                UpdateStatus("The selected system is not available.", NotifyType.ErrorMessage);
            }
            else
            {
                UpdateStatus("Please select a system.", NotifyType.ErrorMessage);
            }
        }

        private void UpdateStatus(string status, NotifyType statusType)
        {
            m_rootPage.NotifyUser(status, statusType);
        }
    }
}
