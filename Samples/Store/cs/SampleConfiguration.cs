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
using System.Collections.ObjectModel;
using Windows.Services.Store;

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
            new Scenario() { Title = "Unmanaged consumable product", ClassType = typeof(Scenario3_UnmanagedConsumable) },
            new Scenario() { Title = "Managed consumable product", ClassType = typeof(Scenario4_ConsumableProduct) },
            new Scenario() { Title = "User collection", ClassType = typeof(Scenario5_UserCollection) },
            new Scenario() { Title = "App listing URI", ClassType = typeof(Scenario6_AppListingURI) },
            new Scenario() { Title = "Business to Business", ClassType = typeof(Scenario7_B2B) }
        };
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

    public static class Utils
    {
        public static ObservableCollection<ItemDetails>
            CreateProductListFromQueryResult(StoreProductQueryResult addOns, string description)
        {
            var productList = new ObservableCollection<ItemDetails>();

            if (addOns.ExtendedError != null)
            {
                ReportExtendedError(addOns.ExtendedError);
            }
            else if (addOns.Products.Count == 0)
            {
                MainPage.Current.NotifyUser($"No configured {description} found for this Store Product.", NotifyType.ErrorMessage);
            }
            else
            {
                foreach (StoreProduct product in addOns.Products.Values)
                {
                    productList.Add(new ItemDetails(product));
                }
            }
            return productList;
        }

        static int IAP_E_UNEXPECTED = unchecked((int)0x803f6107);

        public static void ReportExtendedError(Exception extendedError)
        {
            string message;
            if (extendedError.HResult == IAP_E_UNEXPECTED)
            {
                message = "This sample has not been properly configured. See the README for instructions.";
            }
            else
            {
                // The user may be offline or there might be some other server failure.
                message = $"ExtendedError: {extendedError.Message}";
            }
            MainPage.Current.NotifyUser(message, NotifyType.ErrorMessage);
        }
    }

    public static class BindingUtils
    {
        // Helper function for binding.
        public static bool IsNonNull(object o)
        {
            return o != null;
        }
    }
}
