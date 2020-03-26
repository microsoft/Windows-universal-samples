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
    public sealed partial class Scenario3_InterstitialAd : Page
    {
        private MainPage rootPage = MainPage.Current;
        private InterstitialAd interstitialAd;

        public Scenario3_InterstitialAd()
        {
            this.InitializeComponent();

            // Instantiate the interstitial video ad
            interstitialAd = new InterstitialAd();

            // Attach event handlers
            interstitialAd.ErrorOccurred += OnAdError;
            interstitialAd.AdReady += OnAdReady;
            interstitialAd.Cancelled += OnAdCancelled;
            interstitialAd.Completed += OnAdCompleted;
        }

        private void RunInterstitialAd(object sender, RoutedEventArgs e)
        {
            // Request an ad. When the ad is ready to show, the AdReady event will fire.
            // The application id and ad unit id are passed in here.
            // The application id and ad unit id can be obtained from Dev Center.
            // See "Monetize with Ads" at https://msdn.microsoft.com/en-us/library/windows/apps/mt170658.aspx
            interstitialAd.RequestAd(AdType.Video, "d25517cb-12d4-4699-8bdc-52040c712cab", "11389925");
            rootPage.NotifyUser("Ad requested", NotifyType.StatusMessage);
        }

        // This is an event handler for the interstitial ad. It is triggered when the interstitial ad is ready to play.
        private void OnAdReady(object sender, object e)
        {
            // The ad is ready to show; show it.
            interstitialAd.Show();
        }

        // This is an event handler for the interstitial ad. It is triggered when the interstitial ad is cancelled.
        private void OnAdCancelled(object sender, object e)
        {
            rootPage.NotifyUser("Ad cancelled", NotifyType.StatusMessage);
        }

        // This is an event handler for the interstitial ad. It is triggered when the interstitial ad has completed playback.
        private void OnAdCompleted(object sender, object e)
        {
            rootPage.NotifyUser("Ad completed", NotifyType.StatusMessage);
        }

        // This is an error handler for the interstitial ad.
        private void OnAdError(object sender, AdErrorEventArgs e)
        {
            rootPage.NotifyUser($"An error occurred. {e.ErrorCode}: {e.ErrorMessage}", NotifyType.ErrorMessage);
        }
    }
}
