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
using System.Collections.Generic;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Store;
using Windows.Storage;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        // This is used on the main page as the title of the sample.
        public const string FEATURE_NAME = "Store C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Trial-mode", ClassType = typeof(Scenario1_TrialMode) },
            new Scenario() { Title = "In-app purchase", ClassType = typeof(Scenario2_InAppPurchase) },
            new Scenario() { Title = "Expiring product", ClassType = typeof(Scenario3_ExpiringProduct) },
            new Scenario() { Title = "Consumable product", ClassType = typeof(Scenario4_ConsumableProduct) },
            new Scenario() { Title = "Advanced consumable product", ClassType = typeof(Scenario5_AdvancedConsumableProduct) },
            new Scenario() { Title = "Large catalog product", ClassType = typeof(Scenario6_LargeCatalogProduct) },
            new Scenario() { Title = "App listing URI", ClassType = typeof(Scenario7_AppListingURI) },
            new Scenario() { Title = "Receipt", ClassType = typeof(Scenario8_Receipt) },
            new Scenario() { Title = "Business to Business", ClassType = typeof(Scenario9_B2B) }
        };

        public static async Task ConfigureSimulatorAsync(string filename)
        {
            StorageFile proxyFile = await Package.Current.InstalledLocation.GetFileAsync("data\\" + filename);
            await CurrentAppSimulator.ReloadSimulatorAsync(proxyFile);
        }
    }

    public class Scenario
    {
        public string Title { get; set; }

        public Type ClassType { get; set; }

        public override string ToString()
        {
            return Title;
        }
    }
}
