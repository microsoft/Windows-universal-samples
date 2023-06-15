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

using Windows.Networking.Connectivity;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_NetworkConnectivity : Page
    {
        public Scenario1_NetworkConnectivity()
        {
            this.InitializeComponent();
        }

        private async void QueryCurrentNetworkConnectivityButton_Click(object sender, RoutedEventArgs e)
        {
            ResultsText.Text = string.Empty;

            ConnectionProfile profile = NetworkInformation.GetInternetConnectionProfile();
            NetworkConnectivityLevel connectivityLevel = NetworkConnectivityLevel.None;
            if (profile != null)
            {
                // It can take the system a few moments to detect a change in network connectivity,
                // so it's possible that the value returned is slightly out of date.
                // Scenario3_NetworkConnectivityChanges shows how to be notified when the system
                // detects connectivity changes.
                connectivityLevel = profile.GetNetworkConnectivityLevel();
            }

            Helpers.AppendLine(ResultsText, $"Current connectivity is {connectivityLevel}.");

            if (Helpers.ShouldAttemptToConnectToInternet(connectivityLevel))
            {
                await Helpers.EvaluateCostAndConnectAsync(profile.GetConnectionCost(), OptedInToNetworkUsageToggle.IsOn, ResultsText);
            }
            else
            {
                Helpers.AppendLine(ResultsText, "Not attempting to connect to the Internet.");
            }
        }
    }
}


