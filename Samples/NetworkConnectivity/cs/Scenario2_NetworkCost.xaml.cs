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
    public sealed partial class Scenario2_NetworkCost : Page
    {
        public Scenario2_NetworkCost()
        {
            this.InitializeComponent();
        }

        private void GetNetworkCost_Click(object sender, RoutedEventArgs e)
        {
            ResultsText.Text = string.Empty;

            ConnectionProfile profile = NetworkInformation.GetInternetConnectionProfile();
            if (profile != null)
            {
                ConnectionCost connectionCost = profile.GetConnectionCost();
                Helpers.AppendLine(ResultsText, $"Network cost is {connectionCost.NetworkCostType}.");

                Helpers.EvaluateAndReportConnectionCost(connectionCost, ResultsText);
            }
            else
            {
                Helpers.AppendLine(ResultsText, "No internet connection profile.");
            }
        }
    }
}
