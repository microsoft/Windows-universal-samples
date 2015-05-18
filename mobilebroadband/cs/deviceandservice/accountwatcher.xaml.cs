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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Networking.NetworkOperators;
using System.Text;

namespace MobileBroadband
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AccountWatcher : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private MobileBroadbandAccountWatcher networkAccountWatcher = new MobileBroadbandAccountWatcher();

        public AccountWatcher()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            PrepScenario2Sample();
        }

        /// <summary>
        /// This is the click handler for the 'Start' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void StartMonitoring_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                if ((networkAccountWatcher.Status == MobileBroadbandAccountWatcherStatus.Started) ||
                (networkAccountWatcher.Status == MobileBroadbandAccountWatcherStatus.EnumerationCompleted))
                {
                    rootPage.NotifyUser("Watcher is already started.", NotifyType.StatusMessage);
                }
                else
                {
                    networkAccountWatcher.Start();

                    rootPage.NotifyUser("Watcher is started successfully.", NotifyType.StatusMessage);
                }
            }
        }

        /// <summary>
        /// This is the click handler for the 'Stop' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void StopMonitoring_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                if ((networkAccountWatcher.Status == MobileBroadbandAccountWatcherStatus.Started) ||
                (networkAccountWatcher.Status == MobileBroadbandAccountWatcherStatus.EnumerationCompleted))
                {
                    networkAccountWatcher.Stop();

                    rootPage.NotifyUser("Watcher is stopped successfully.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Watcher is already stopped.", NotifyType.StatusMessage);
                }
            }
        }

        void PrepScenario2Sample()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            networkAccountWatcher.AccountAdded +=
                (sender, args) =>
                {
                    string message = "[accountadded] ";

                    try
                    {
                        message += args.NetworkAccountId;

                        var account = MobileBroadbandAccount.CreateFromNetworkAccountId(args.NetworkAccountId);
                        //rootPage.mba = (MobileBroadbandAccount)account;
                        message += ", service provider name: " + account.ServiceProviderName;
                    }
                    catch (Exception ex)
                    {
                        message += ex.Message;
                    }

                    DisplayWatcherOutputFromCallback(message);
                };

            networkAccountWatcher.AccountUpdated +=
                (sender, args) =>
                {
                    string message = "[accountupdated] ";

                    try
                    {
                        message += args.NetworkAccountId + ", (network = " + args.HasNetworkChanged + "; deviceinformation = " + args.HasDeviceInformationChanged + ")" + Environment.NewLine;
                        message += DumpPropertyData(args.NetworkAccountId, args.HasNetworkChanged, args.HasDeviceInformationChanged);
                    }
                    catch (Exception ex)
                    {
                        message += ex.Message;
                    }

                    DisplayWatcherOutputFromCallback(message);
                };

            networkAccountWatcher.AccountRemoved +=
                (sender, args) =>
                {
                    string message = "[accountremoved] ";

                    try
                    {
                        message += args.NetworkAccountId;
                    }
                    catch (Exception ex)
                    {
                        message += ex.Message;
                    }

                    DisplayWatcherOutputFromCallback(message);
                };

            networkAccountWatcher.EnumerationCompleted +=
                (sender, args) =>
                {
                    string message = "[enumerationcompleted]";

                    DisplayWatcherOutputFromCallback(message);
                };

            networkAccountWatcher.Stopped +=
                (sender, args) =>
                {
                    string message = "[stopped] Watcher is stopped successfully";

                    DisplayWatcherOutputFromCallback(message);
                };
        }

        async void DisplayWatcherOutputFromCallback(string value)
        {
            await Dispatcher.RunAsync(
                Windows.UI.Core.CoreDispatcherPriority.Normal,
                () =>
                {
                    WatcherOutput.Text += value + Environment.NewLine;
                });
        }

        void DumpAccountDeviceInformation(StringBuilder message, MobileBroadbandDeviceInformation deviceInformation)
        {
            message.AppendLine("NetworkDeviceStatus: " + deviceInformation.NetworkDeviceStatus.ToString());
            message.AppendLine("MobileEquipmentId: " + deviceInformation.MobileEquipmentId);
            message.AppendLine("SubscriberId: " + deviceInformation.SubscriberId);
            message.AppendLine("SimIccId: " + deviceInformation.SimIccId);
            message.AppendLine("DeviceId: " + deviceInformation.DeviceId);
            message.AppendLine("RadioState: " + deviceInformation.CurrentRadioState);
        }

        void DumpAccountNetwork(StringBuilder message, MobileBroadbandNetwork network)
        {
            string accessPointName = network.AccessPointName;
            if (String.IsNullOrEmpty(accessPointName))
            {
                accessPointName = "(not connected)";
            }

            message.AppendLine("NetworkRegistrationState: " + network.NetworkRegistrationState.ToString());
            message.AppendLine("RegistrationNetworkError: " + NetErrorToString(network.RegistrationNetworkError));
            message.AppendLine("PacketAttachNetworkError: " + NetErrorToString(network.PacketAttachNetworkError));
            message.AppendLine("ActivationNetworkError: " + NetErrorToString(network.ActivationNetworkError));
            message.AppendLine("AccessPointName: " + accessPointName);
        }

        string DumpPropertyData(string networkAccountId, bool hasDeviceInformationChanged, bool hasNetworkChanged)
        {
            StringBuilder message = new StringBuilder();

            var account = MobileBroadbandAccount.CreateFromNetworkAccountId(networkAccountId);

            if (hasDeviceInformationChanged)
            {
                DumpAccountDeviceInformation(message, account.CurrentDeviceInformation);
            }

            if (hasNetworkChanged)
            {
                DumpAccountNetwork(message, account.CurrentNetwork);
            }

            return message.ToString();
        }

        string NetErrorToString(uint netError)
        {
            return netError == 0 ? "none" : netError.ToString();
        }
    }
}
