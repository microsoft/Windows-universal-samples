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

#include "Scenario3_MultipleDrawers.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_MultipleDrawers sealed
    {
    public:
        Scenario3_MultipleDrawers();

    private:
        MainPage^ rootPage;

        Windows::Devices::PointOfService::CashDrawer^ cashDrawerInstance1 = nullptr;
        Windows::Devices::PointOfService::CashDrawer^ cashDrawerInstance2 = nullptr;
        Windows::Devices::PointOfService::ClaimedCashDrawer^ claimedCashDrawerInstance1 = nullptr;
        Windows::Devices::PointOfService::ClaimedCashDrawer^ claimedCashDrawerInstance2 = nullptr;

        void ClaimButton1_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ClaimButton2_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ReleaseButton1_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ReleaseButton2_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        enum CashDrawerInstance
        {
            Instance1,
            Instance2
        };

        Windows::Foundation::EventRegistrationToken releaseRequestedToken1;
        Windows::Foundation::EventRegistrationToken releaseRequestedToken2;

        void claimedCashDrawerInstance1_ReleaseDeviceRequested(Windows::Devices::PointOfService::ClaimedCashDrawer^ sender, Platform::Object^ e);
        void claimedCashDrawerInstance2_ReleaseDeviceRequested(Windows::Devices::PointOfService::ClaimedCashDrawer^ sender, Platform::Object^ e);

        concurrency::task<bool> CreateDefaultCashDrawerObject(CashDrawerInstance instance);
        concurrency::task<bool> ClaimCashDrawer(CashDrawerInstance instance);

        void ResetScenarioState();
        void SetClaimedUI(CashDrawerInstance instance);
        void SetReleasedUI(CashDrawerInstance instance);

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    };

}
