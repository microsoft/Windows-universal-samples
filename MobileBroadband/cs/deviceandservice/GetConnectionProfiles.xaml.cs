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
using System.Collections.Generic;
using Windows.Networking.Connectivity;

namespace MobileBroadband
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class GetConnectionProfiles : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private IReadOnlyList<string> deviceAccountId = null;

        public GetConnectionProfiles()
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
            PrepareScenario();
        }

        /// <summary>
        /// This is the click handler for the 'Default' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void GetConnectionProfilesButton_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                try
                {
                    // For the sake of simplicity, we only get the ConnectionProfiles from the first NetworkAccountId
                    var mobileBroadbandAccount = MobileBroadbandAccount.CreateFromNetworkAccountId(deviceAccountId[0]);
                    PrintConnectionProfiles(mobileBroadbandAccount.GetConnectionProfiles());
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Error:" + ex.Message, NotifyType.ErrorMessage);
                }
            }
        }


        void PrepareScenario()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            try
            {
                deviceAccountId = MobileBroadbandAccount.AvailableNetworkAccountIds;

                if (deviceAccountId.Count != 0)
                {
                    GetConnectionProfilesButton.Content = "Get Connection Profiles";
                    GetConnectionProfilesButton.IsEnabled = true;
                }
                else
                {
                    GetConnectionProfilesButton.Content = "No available accounts detected";
                    GetConnectionProfilesButton.IsEnabled = false;
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error:" + ex.Message, NotifyType.ErrorMessage);
            }
        }

        private void PrintConnectionProfiles(IReadOnlyList<ConnectionProfile> connectionProfiles)
        {
            string connectionProfileList = string.Empty;
            try
            {
                foreach (var connectionProfile in connectionProfiles)
                {
                    //Display Profile information for each of the connection profiles
                    connectionProfileList += GetConnectionProfile(connectionProfile);
                    connectionProfileList += "--------------------------------------------------------------------\n";
                }

                if (connectionProfiles.Count == 0)
                {
                    connectionProfileList += "No Connection Profiles found.\n";
                }

                rootPage.NotifyUser(connectionProfileList, NotifyType.StatusMessage);
            }

            catch (Exception ex)
            {
                rootPage.NotifyUser("Unexpected exception occurred: " + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        //
        //Get Connection Profile name and cost information
        //
        string GetConnectionProfile(ConnectionProfile connectionProfile)
        {
            string connectionProfileInfo = string.Empty;
            if (connectionProfile != null)
            {
                connectionProfileInfo = "Profile Name : " + connectionProfile.ProfileName + "\n";

                switch (connectionProfile.GetNetworkConnectivityLevel())
                {
                    case NetworkConnectivityLevel.None:
                        connectionProfileInfo += "Connectivity Level : None\n";
                        break;
                    case NetworkConnectivityLevel.LocalAccess:
                        connectionProfileInfo += "Connectivity Level : Local Access\n";
                        break;
                    case NetworkConnectivityLevel.ConstrainedInternetAccess:
                        connectionProfileInfo += "Connectivity Level : Constrained Internet Access\n";
                        break;
                    case NetworkConnectivityLevel.InternetAccess:
                        connectionProfileInfo += "Connectivity Level : Internet Access\n";
                        break;
                }

                switch (connectionProfile.GetDomainConnectivityLevel())
                {
                    case DomainConnectivityLevel.None:
                        connectionProfileInfo += "Domain Connectivity Level : None\n";
                        break;
                    case DomainConnectivityLevel.Unauthenticated:
                        connectionProfileInfo += "Domain Connectivity Level : Unauthenticated\n";
                        break;
                    case DomainConnectivityLevel.Authenticated:
                        connectionProfileInfo += "Domain Connectivity Level : Authenticated\n";
                        break;
                }

                //Get Connection Cost information
                ConnectionCost connectionCost = connectionProfile.GetConnectionCost();
                connectionProfileInfo += GetConnectionCostInfo(connectionCost);

                //Get Dataplan Status information
                DataPlanStatus dataPlanStatus = connectionProfile.GetDataPlanStatus();
                connectionProfileInfo += GetDataPlanStatusInfo(dataPlanStatus);

                //Get Network Security Settings
                NetworkSecuritySettings netSecuritySettings = connectionProfile.NetworkSecuritySettings;
                connectionProfileInfo += GetNetworkSecuritySettingsInfo(netSecuritySettings);

                //Get Wlan Connection Profile Details if this is a Wlan ConnectionProfile
                if (connectionProfile.IsWlanConnectionProfile)
                {
                    WlanConnectionProfileDetails wlanConnectionProfileDetails = connectionProfile.WlanConnectionProfileDetails;
                    connectionProfileInfo += GetWlanConnectionProfileDetailsInfo(wlanConnectionProfileDetails);
                }

                //Get Wwan Connection Profile Details if this is a Wwan ConnectionProfile
                if (connectionProfile.IsWwanConnectionProfile)
                {
                    WwanConnectionProfileDetails wwanConnectionProfileDetails = connectionProfile.WwanConnectionProfileDetails;
                    connectionProfileInfo += GetWwanConnectionProfileDetailsInfo(wwanConnectionProfileDetails);
                }

                //Get the Service Provider GUID
                if (connectionProfile.ServiceProviderGuid.HasValue)
                {
                    connectionProfileInfo += "====================\n";
                    connectionProfileInfo += "Service Provider GUID: " + connectionProfile.ServiceProviderGuid + "\n";
                }

                //Get the number of signal bars
                if (connectionProfile.GetSignalBars().HasValue)
                {
                    connectionProfileInfo += "====================\n";
                    connectionProfileInfo += "Signal Bars: " + connectionProfile.GetSignalBars() + "\n";
                }
            }
            return connectionProfileInfo;
        }

        //
        //Get Profile Connection cost
        //
        string GetConnectionCostInfo(ConnectionCost connectionCost)
        {
            string cost = string.Empty;
            cost += "Connection Cost Information: \n";
            cost += "====================\n";

            if (connectionCost == null)
            {
                cost += "Connection Cost not available\n";
                return cost;
            }

            switch (connectionCost.NetworkCostType)
            {
                case NetworkCostType.Unrestricted:
                    cost += "Cost: Unrestricted";
                    break;
                case NetworkCostType.Fixed:
                    cost += "Cost: Fixed";
                    break;
                case NetworkCostType.Variable:
                    cost += "Cost: Variable";
                    break;
                case NetworkCostType.Unknown:
                    cost += "Cost: Unknown";
                    break;
                default:
                    cost += "Cost: Error";
                    break;
            }
            cost += "\n";
            cost += "Roaming: " + connectionCost.Roaming + "\n";
            cost += "Over Data Limit: " + connectionCost.OverDataLimit + "\n";
            cost += "Approaching Data Limit : " + connectionCost.ApproachingDataLimit + "\n";

            //Display cost based suggestions to the user
            cost += CostBasedSuggestions(connectionCost);
            return cost;
        }

        //
        //Display Cost based suggestions to the user
        //
        string CostBasedSuggestions(ConnectionCost connectionCost)
        {
            string costBasedSuggestions = string.Empty;
            costBasedSuggestions += "Cost Based Suggestions: \n";
            costBasedSuggestions += "====================\n";

            if (connectionCost.Roaming)
            {
                costBasedSuggestions += "Connection is out of MNO's network, using the connection may result in additional charge. Application can implement High Cost behavior in this scenario\n";
            }
            else if (connectionCost.NetworkCostType == NetworkCostType.Variable)
            {
                costBasedSuggestions += "Connection cost is variable, and the connection is charged based on usage, so application can implement the Conservative behavior\n";
            }
            else if (connectionCost.NetworkCostType == NetworkCostType.Fixed)
            {
                if (connectionCost.OverDataLimit || connectionCost.ApproachingDataLimit)
                {
                    costBasedSuggestions += "Connection has exceeded the usage cap limit or is approaching the datalimit, and the application can implement High Cost behavior in this scenario\n";
                }
                else
                {
                    costBasedSuggestions += "Application can implemement the Conservative behavior\n";
                }
            }
            else
            {
                costBasedSuggestions += "Application can implement the Standard behavior\n";
            }
            return costBasedSuggestions;
        }

        //
        //Display Profile Dataplan Status information
        //
        string GetDataPlanStatusInfo(DataPlanStatus dataPlan)
        {
            string dataplanStatusInfo = string.Empty;
            dataplanStatusInfo = "Dataplan Status Information:\n";
            dataplanStatusInfo += "====================\n";

            if (dataPlan == null)
            {
                dataplanStatusInfo += "Dataplan Status not available\n";
                return dataplanStatusInfo;
            }

            if (dataPlan.DataPlanUsage != null)
            {
                dataplanStatusInfo += "Usage In Megabytes : " + dataPlan.DataPlanUsage.MegabytesUsed + "\n";
                dataplanStatusInfo += "Last Sync Time : " + dataPlan.DataPlanUsage.LastSyncTime + "\n";
            }
            else
            {
                dataplanStatusInfo += "Usage In Megabytes : Not Defined\n";
            }

            ulong? inboundBandwidth = dataPlan.InboundBitsPerSecond;
            if (inboundBandwidth.HasValue)
            {
                dataplanStatusInfo += "InboundBitsPerSecond : " + inboundBandwidth + "\n";
            }
            else
            {
                dataplanStatusInfo += "InboundBitsPerSecond : Not Defined\n";
            }

            ulong? outboundBandwidth = dataPlan.OutboundBitsPerSecond;
            if (outboundBandwidth.HasValue)
            {
                dataplanStatusInfo += "OutboundBitsPerSecond : " + outboundBandwidth + "\n";
            }
            else
            {
                dataplanStatusInfo += "OutboundBitsPerSecond : Not Defined\n";
            }

            uint? dataLimit = dataPlan.DataLimitInMegabytes;
            if (dataLimit.HasValue)
            {
                dataplanStatusInfo += "DataLimitInMegabytes : " + dataLimit + "\n";
            }
            else
            {
                dataplanStatusInfo += "DataLimitInMegabytes : Not Defined\n";
            }

            System.DateTimeOffset? nextBillingCycle = dataPlan.NextBillingCycle;
            if (nextBillingCycle.HasValue)
            {
                dataplanStatusInfo += "NextBillingCycle : " + nextBillingCycle + "\n";
            }
            else
            {
                dataplanStatusInfo += "NextBillingCycle : Not Defined\n";
            }

            uint? maxTransferSize = dataPlan.MaxTransferSizeInMegabytes;
            if (maxTransferSize.HasValue)
            {
                dataplanStatusInfo += "MaxTransferSizeInMegabytes : " + maxTransferSize + "\n";
            }
            else
            {
                dataplanStatusInfo += "MaxTransferSizeInMegabytes : Not Defined\n";
            }
            return dataplanStatusInfo;
        }

        //
        //Get Network Security Settings information
        //
        string GetNetworkSecuritySettingsInfo(NetworkSecuritySettings netSecuritySettings)
        {
            string networkSecurity = string.Empty;
            networkSecurity += "Network Security Settings: \n";
            networkSecurity += "====================\n";

            if (netSecuritySettings == null)
            {
                networkSecurity += "Network Security Settings not available\n";
                return networkSecurity;
            }

            //NetworkAuthenticationType
            switch (netSecuritySettings.NetworkAuthenticationType)
            {
                case NetworkAuthenticationType.None:
                    networkSecurity += "NetworkAuthenticationType: None";
                    break;
                case NetworkAuthenticationType.Unknown:
                    networkSecurity += "NetworkAuthenticationType: Unknown";
                    break;
                case NetworkAuthenticationType.Open80211:
                    networkSecurity += "NetworkAuthenticationType: Open80211";
                    break;
                case NetworkAuthenticationType.SharedKey80211:
                    networkSecurity += "NetworkAuthenticationType: SharedKey80211";
                    break;
                case NetworkAuthenticationType.Wpa:
                    networkSecurity += "NetworkAuthenticationType: Wpa";
                    break;
                case NetworkAuthenticationType.WpaPsk:
                    networkSecurity += "NetworkAuthenticationType: WpaPsk";
                    break;
                case NetworkAuthenticationType.WpaNone:
                    networkSecurity += "NetworkAuthenticationType: WpaNone";
                    break;
                case NetworkAuthenticationType.Rsna:
                    networkSecurity += "NetworkAuthenticationType: Rsna";
                    break;
                case NetworkAuthenticationType.RsnaPsk:
                    networkSecurity += "NetworkAuthenticationType: RsnaPsk";
                    break;
                default:
                    networkSecurity += "NetworkAuthenticationType: Error";
                    break;
            }
            networkSecurity += "\n";

            //NetworkEncryptionType
            switch (netSecuritySettings.NetworkEncryptionType)
            {
                case NetworkEncryptionType.None:
                    networkSecurity += "NetworkEncryptionType: None";
                    break;
                case NetworkEncryptionType.Unknown:
                    networkSecurity += "NetworkEncryptionType: Unknown";
                    break;
                case NetworkEncryptionType.Wep:
                    networkSecurity += "NetworkEncryptionType: Wep";
                    break;
                case NetworkEncryptionType.Wep40:
                    networkSecurity += "NetworkEncryptionType: Wep40";
                    break;
                case NetworkEncryptionType.Wep104:
                    networkSecurity += "NetworkEncryptionType: Wep104";
                    break;
                case NetworkEncryptionType.Tkip:
                    networkSecurity += "NetworkEncryptionType: Tkip";
                    break;
                case NetworkEncryptionType.Ccmp:
                    networkSecurity += "NetworkEncryptionType: Ccmp";
                    break;
                case NetworkEncryptionType.WpaUseGroup:
                    networkSecurity += "NetworkEncryptionType: WpaUseGroup";
                    break;
                case NetworkEncryptionType.RsnUseGroup:
                    networkSecurity += "NetworkEncryptionType: RsnUseGroup";
                    break;
                default:
                    networkSecurity += "NetworkEncryptionType: Error";
                    break;
            }
            networkSecurity += "\n";
            return networkSecurity;
        }

        string GetWlanConnectionProfileDetailsInfo(WlanConnectionProfileDetails wlanConnectionProfileDetails)
        {
            string wlanDetails = string.Empty;
            wlanDetails += "Wireless LAN Connection Profile Details:\n";
            wlanDetails += "====================\n";

            if (wlanConnectionProfileDetails == null)
            {
                wlanDetails += "Wireless LAN connection profile details unavailable.\n";
                return wlanDetails;
            }

            wlanDetails += "Connected SSID: " + wlanConnectionProfileDetails.GetConnectedSsid() + "\n";
            return wlanDetails;
        }

        string GetWwanConnectionProfileDetailsInfo(WwanConnectionProfileDetails wwanConnectionProfileDetails)
        {
            string wwanDetails = string.Empty;
            wwanDetails += "Wireless WAN Connection Profile Details:\n";
            wwanDetails += "====================\n";

            if (wwanConnectionProfileDetails == null)
            {
                wwanDetails += "Wireless WAN connection profile details unavailable.\n";
                return wwanDetails;
            }

            wwanDetails += "Home Provider ID: " + wwanConnectionProfileDetails.HomeProviderId + "\n";
            wwanDetails += "Access Point Name: " + wwanConnectionProfileDetails.AccessPointName + "\n";
            wwanDetails += "Network Registration State: " + wwanConnectionProfileDetails.GetNetworkRegistrationState() + "\n";
            wwanDetails += "Current Data Class: " + wwanConnectionProfileDetails.GetCurrentDataClass() + "\n";

            return wwanDetails;
        }
    }
}
