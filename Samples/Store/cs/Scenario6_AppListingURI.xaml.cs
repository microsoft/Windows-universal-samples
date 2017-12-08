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
using Windows.Services.Store;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// A page for sixth scenario.
    /// </summary>
    public sealed partial class Scenario6_AppListingURI : Page
    {
        MainPage rootPage = MainPage.Current;
        private StoreContext storeContext = StoreContext.GetDefault();

        public Scenario6_AppListingURI()
        {
            this.InitializeComponent();
        }

        private async void DisplayLink()
        {
            StoreProductResult result = await storeContext.GetStoreProductForCurrentAppAsync();
            if (result.ExtendedError != null)
            {
                Utils.ReportExtendedError(result.ExtendedError);
                return;
            }

            await Windows.System.Launcher.LaunchUriAsync(result.Product.LinkUri);
        }
    }
}
