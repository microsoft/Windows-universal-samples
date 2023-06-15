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
using System.Threading.Tasks;
using Windows.Networking.Connectivity;
using Windows.UI.Xaml.Controls;
using Windows.Web.Http;

namespace SDKTemplate
{
    internal class Helpers
    {
        public static void AppendLine(TextBlock textBlock, string message)
        {
            textBlock.Text += message + "\n\n";
        }

        enum CostGuidance
        {
            Normal,
            OptIn,
            Conservative
        }

        public static bool ShouldAttemptToConnectToInternet(NetworkConnectivityLevel connectivityLevel)
        {
            bool shouldConnectToInternet = false;
            if (connectivityLevel == NetworkConnectivityLevel.LocalAccess || connectivityLevel == NetworkConnectivityLevel.InternetAccess)
            {
                shouldConnectToInternet = true;
            }

            return shouldConnectToInternet;
        }

        private static async Task<string> SendHttpGetRequestAsync()
        {
            string message;
            HttpClient httpClient = new HttpClient();
            HttpGetStringResult result = await httpClient.TryGetStringAsync(new Uri("http://www.msftconnecttest.com/connecttest.txt"));
            if (result.Succeeded)
            {
                message = $"Connection to server confirmed: \"{result.Value}\"";
            }
            else if (result.ResponseMessage != null)
            {
                message = $"Cannot connect to server: {result.ResponseMessage.StatusCode}";
            }
            else
            {
                message = $"Cannot connect to server: {result.ExtendedError.Message}";
            }
            return message;
        }

        private static CostGuidance GetNetworkCostGuidance(ConnectionCost connectionCost)
        {
            CostGuidance costGuidance = CostGuidance.Normal;
            if (connectionCost.Roaming || connectionCost.OverDataLimit)
            {
                costGuidance = CostGuidance.OptIn;
            }
            else if (connectionCost.NetworkCostType == NetworkCostType.Fixed || connectionCost.NetworkCostType == NetworkCostType.Variable)
            {
                costGuidance = CostGuidance.Conservative;
            }
            else
            {
                costGuidance = CostGuidance.Normal;
            }
            return costGuidance;
        }

        public static void EvaluateAndReportConnectionCost(ConnectionCost connectionCost, TextBlock textBlock)
        {
            CostGuidance costGuidance = GetNetworkCostGuidance(connectionCost);
            switch (costGuidance)
            {
                // In opt-in scenarios, apps handle cases where the network access cost is significantly higher than the plan cost.
                // For example, when a user is roaming, a mobile carrier may charge a higher rate data usage.
                case CostGuidance.OptIn:
                    AppendLine(textBlock, "Apps should implement opt-in behavior.");
                    break;
                // In conservative scenarios, apps implement restrictions for optimizing network usage to handle transfers over metered networks.
                case CostGuidance.Conservative:
                    AppendLine(textBlock, "Apps should implement conservative behavior.");
                    break;
                // In normal scenarios, apps do not implement restrictions. Apps treat the connection as unlimited in cost.
                case CostGuidance.Normal:
                default:
                    AppendLine(textBlock, "Apps should implement normal behavior.");
                    break;
            }
        }

        public static async Task EvaluateCostAndConnectAsync(ConnectionCost connectionCost, bool optedInToCharges, TextBlock textBlock)
        {
            CostGuidance costGuidance = GetNetworkCostGuidance(connectionCost);
            switch (costGuidance)
            {
                case CostGuidance.OptIn:
                    {
                        AppendLine(textBlock, "Connecting to the Internet may incur charges.");
                        if (optedInToCharges)
                        {
                            AppendLine(textBlock, "User has opted in to additional network usage charges.");
                            AppendLine(textBlock, await SendHttpGetRequestAsync());
                        }
                        else
                        {
                            AppendLine(textBlock, "User has opted out of additional network usage charges. Not connecting to the Internet.");
                        }
                    }
                    break;
                case CostGuidance.Conservative:
                    AppendLine(textBlock, "Attempting to connect to the Internet, optimizing traffic for conservative network usage.");
                    AppendLine(textBlock, await SendHttpGetRequestAsync());
                    break;
                case CostGuidance.Normal:
                default:
                    AppendLine(textBlock, "Attempting to connect to the Internet.");
                    AppendLine(textBlock, await SendHttpGetRequestAsync());
                    break;
            }
        }
    }
}
