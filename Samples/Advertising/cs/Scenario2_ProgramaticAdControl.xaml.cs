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

using Microsoft.Advertising.WinRT.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_ProgramaticAdControl : Page
    {
        MainPage rootPage = MainPage.Current;
        int adCount = 0;

        public Scenario2_ProgramaticAdControl()
        {
            this.InitializeComponent();
        }

        private void CreateAdControl_Click(object sender, RoutedEventArgs e)
        {
            var button = (Button)sender;
            button.IsEnabled = false;

            // Programatically create an ad control. This must be done from the UI thread.
            var adControl = new AdControl();

            // Set the application id and ad unit id
            // The application id and ad unit id can be obtained from Dev Center.
            // See "Monetize with Ads" at https://msdn.microsoft.com/en-us/library/windows/apps/mt170658.aspx
            adControl.ApplicationId = "d25517cb-12d4-4699-8bdc-52040c712cab";
            adControl.AdUnitId = "10043134";

            // Set the dimensions
            adControl.Width = 160;
            adControl.Height = 600;

            // Add event handlers if you want
            adControl.ErrorOccurred += OnErrorOccurred;
            adControl.AdRefreshed += OnAdRefreshed;

            // Add the ad control to the page.
            var parent = (Panel)button.Parent;
            parent.Children.Add(adControl);
        }

        // This is an error handler for the interstitial ad.
        private void OnErrorOccurred(object sender, AdErrorEventArgs e)
        {
            rootPage.NotifyUser($"An error occurred. {e.ErrorCode}: {e.ErrorMessage}", NotifyType.ErrorMessage);
        }

        // This is an event handler for the ad control. It's invoked when the ad is refreshed.
        private void OnAdRefreshed(object sender, RoutedEventArgs e)
        {
            // We increment the ad count so that the message changes at every refresh.
            adCount++;
            rootPage.NotifyUser($"Advertisement #{adCount}", NotifyType.StatusMessage);
        }
    }
}
