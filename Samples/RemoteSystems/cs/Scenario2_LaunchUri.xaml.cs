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
using System.Collections.ObjectModel;
using Windows.System;
using Windows.System.RemoteSystems;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_LaunchUri : Page
    {
        private MainPage m_rootPage;

        public Scenario2_LaunchUri()
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

        private async void Launch_Clicked(object sender, RoutedEventArgs e)
        {
            RemoteSystem selectedSystem = SystemListComboBox.SelectedItem as RemoteSystem;
            if (selectedSystem != null)
            {
                Uri uri;
                if (Uri.TryCreate(UriTextBox.Text, UriKind.Absolute, out uri))
                {
                    UpdateStatus("LaunchUriAsync called. Waiting for response...", NotifyType.StatusMessage);

                    // Launch URI on the remote system. 
                    // Note: LaunchUriAsync needs to called from the UI thread.
                    RemoteLaunchUriStatus launchUriStatus = await RemoteLauncher.LaunchUriAsync(new RemoteSystemConnectionRequest(selectedSystem), uri);

                    UpdateStatus("LaunchUriStatus = " + launchUriStatus.ToString(), launchUriStatus == RemoteLaunchUriStatus.Success ? NotifyType.StatusMessage : NotifyType.ErrorMessage);
                }
                else
                {
                    UpdateStatus("Please enter a valid URI.", NotifyType.ErrorMessage);
                }
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
