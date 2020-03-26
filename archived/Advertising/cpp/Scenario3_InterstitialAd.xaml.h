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

#pragma once

#include "Scenario3_InterstitialAd.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_InterstitialAd sealed
    {
    public:
        Scenario3_InterstitialAd();

    private:
        MainPage^ rootPage = MainPage::Current;
        Microsoft::Advertising::WinRT::UI::InterstitialAd^ interstitialAd;

        void RunInterstitialAd(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnAdReady(Platform::Object^ sender, Platform::Object^ args);
        void OnAdCancelled(Platform::Object^ sender, Platform::Object^ args);
        void OnAdCompleted(Platform::Object^ sender, Platform::Object^ args);
        void OnErrorOccurred(Platform::Object^ sender, Microsoft::Advertising::WinRT::UI::AdErrorEventArgs^ args);
    };
}
