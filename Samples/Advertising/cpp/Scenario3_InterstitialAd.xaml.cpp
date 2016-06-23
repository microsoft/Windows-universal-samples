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

#include "pch.h"
#include "Scenario3_InterstitialAd.xaml.h"

using namespace SDKTemplate;

using namespace Microsoft::Advertising::WinRT::UI;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario3_InterstitialAd::Scenario3_InterstitialAd() : rootPage(MainPage::Current)
{
    InitializeComponent();

    // Instantiate the interstitial ad
    interstitialAd = ref new InterstitialAd();

    // Attach event handlers
    interstitialAd->ErrorOccurred += ref new EventHandler<AdErrorEventArgs^>(this, &Scenario3_InterstitialAd::OnErrorOccurred);
    interstitialAd->AdReady += ref new EventHandler<Object^>(this, &Scenario3_InterstitialAd::OnAdReady);
    interstitialAd->Cancelled += ref new EventHandler<Object^>(this, &Scenario3_InterstitialAd::OnAdCancelled);
    interstitialAd->Completed += ref new EventHandler<Object^>(this, &Scenario3_InterstitialAd::OnAdCompleted);
}

void Scenario3_InterstitialAd::RunInterstitialAd(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Request an ad. When the ad is ready to show, the AdReady event will fire.
    // The application id and ad unit are passed in here.
    // The application id and ad unit id can be obtained from Dev Center.
    // See "Monetize with Ads" at https://msdn.microsoft.com/en-us/library/windows/apps/mt170658.aspx
    interstitialAd->RequestAd(AdType::Video, L"d25517cb-12d4-4699-8bdc-52040c712cab", L"11389925");
    rootPage->NotifyUser("Ad requested", NotifyType::StatusMessage);
}

// This is an event handler for the interstitial ad. It is triggered when the interstitial ad information has been downloaded and is ready to show.
void Scenario3_InterstitialAd::OnAdReady(Object^ sender, Object^ e)
{
    // The ad is ready to show; show it.
    interstitialAd->Show();
}

// This is an event handler for the interstitial ad. It is triggered when the interstitial ad is cancelled.
void Scenario3_InterstitialAd::OnAdCancelled(Object^ sender, Object^ e)
{
    rootPage->NotifyUser("Ad cancelled", NotifyType::StatusMessage);
}

// This is an event handler for the interstitial ad. It is triggered when the interstitial ad has completed playback.
void Scenario3_InterstitialAd::OnAdCompleted(Object^ sender, Object^ e)
{
    rootPage->NotifyUser("Ad completed", NotifyType::StatusMessage);
}

// This is an error handler for the interstitial ad.
void Scenario3_InterstitialAd::OnErrorOccurred(Object^ sender, AdErrorEventArgs^ e)
{
    rootPage->NotifyUser("An error occurred. " + e->ErrorCode.ToString() + ": " + e->ErrorMessage, NotifyType::ErrorMessage);
}