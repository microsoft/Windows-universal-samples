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
using System.Threading.Tasks;
using Windows.ApplicationModel.AppService;
using Windows.Foundation.Collections;
using Windows.System.RemoteSystems;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_LaunchAppServices : Page
    {
        private MainPage m_rootPage;
        private int m_minimumValue;
        private int m_maximumValue;

        public Scenario3_LaunchAppServices()
        {
            this.InitializeComponent();
            this.DataContext = this;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            m_rootPage = MainPage.Current;
            m_rootPage.ValidateDeviceDiscovery();
        }

        public ObservableCollection<RemoteSystem> DeviceList => m_rootPage.deviceList;

        private async void GenerateRandomNumber_Clicked(object sender, RoutedEventArgs e)
        {
            // Validate minimum and maximum input values.
            if (ValidateInput())
            {
                // Connect to app service running on the selected device.
                await ConnectToRemoteAppServiceAsync();
            }
        }

        private bool ValidateInput()
        {
            if (!int.TryParse(MinValue.Text, out m_minimumValue))
            {
                UpdateStatus("The Minimum Value should be a valid integer.", NotifyType.ErrorMessage);
                return false;
            }

            if (!int.TryParse(MaxValue.Text, out m_maximumValue))
            {
                UpdateStatus("The Maximum Value should be a valid integer.", NotifyType.ErrorMessage);
                return false;
            }

            if (m_maximumValue <= m_minimumValue)
            {
                UpdateStatus("Maximum Value must be larger than Minimum Value", NotifyType.ErrorMessage);
                return false;
            }

            return true;
        }

        private async Task ConnectToRemoteAppServiceAsync()
        {
            RemoteSystem selectedDevice = DeviceListComboBox.SelectedItem as RemoteSystem;
            if (selectedDevice != null)
            {
                // Create a remote system connection request.
                RemoteSystemConnectionRequest connectionRequest = new RemoteSystemConnectionRequest(selectedDevice);

                // Set up a new app service connection. The following app service name and package family name
                // are used in this sample to work with AppServices provider SDK sample on a remote system.
                using (AppServiceConnection connection = new AppServiceConnection
                {
                    AppServiceName = "com.microsoft.randomnumbergenerator",
                    PackageFamilyName = "Microsoft.SDKSamples.AppServicesProvider.CS_8wekyb3d8bbwe"
                })
                {
                    UpdateStatus("Opening connection to remote app service...", NotifyType.StatusMessage);
                    AppServiceConnectionStatus status = await connection.OpenRemoteAsync(connectionRequest);

                    if (status == AppServiceConnectionStatus.Success)
                    {
                        UpdateStatus("Successfully connected to remote app service.", NotifyType.StatusMessage);
                        await SendMessageToRemoteAppServiceAsync(connection);
                    }
                    else
                    {
                        UpdateStatus("Attempt to open a remote app service connection failed with error - " + status.ToString(), NotifyType.ErrorMessage);
                    }
                }
            }
            else
            {
                UpdateStatus("Select a device for remote connection.", NotifyType.ErrorMessage);
            }
        }

        private async Task SendMessageToRemoteAppServiceAsync(AppServiceConnection connection)
        {
            // Send message if connection to the remote app service is open.
            if (connection != null)
            {
                //Set up the inputs and send a message to the service.
                ValueSet inputs = new ValueSet();
                inputs.Add("minvalue", m_minimumValue);
                inputs.Add("maxvalue", m_maximumValue);
                UpdateStatus("Sent message to remote app service. Waiting for a response...", NotifyType.StatusMessage);
                AppServiceResponse response = await connection.SendMessageAsync(inputs);

                if (response.Status == AppServiceResponseStatus.Success)
                {
                    if (response.Message.ContainsKey("result"))
                    {
                        string resultText = response.Message["result"].ToString();
                        if (string.IsNullOrEmpty(resultText))
                        {
                            UpdateStatus("Remote app service did not respond with a result.", NotifyType.ErrorMessage);
                        }
                        else
                        {
                            UpdateStatus("Result = " + resultText, NotifyType.StatusMessage);
                        }
                    }
                    else
                    {
                        UpdateStatus("Response from remote app service does not contain a result.", NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    UpdateStatus("Sending message to remote app service failed with error - " + response.Status.ToString(), NotifyType.ErrorMessage);
                }
            }
            else
            {
                UpdateStatus("Not connected to any app service. Select a device to open a connection.", NotifyType.ErrorMessage);
            }
        }

        private void UpdateStatus(string status, NotifyType statusType)
        {
            m_rootPage.NotifyUser(status, statusType);
        }
    }
}
