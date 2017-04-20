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

#include "Scenario3_LocationTrackingReason.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Page containing the Extended Execution LocationTracking Reason scenario functions.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class LocationTrackingReason sealed
    {
    public:
        LocationTrackingReason();

    protected:
        void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
        void BeginExtendedExecution();
        void EndExtendedExecution();

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::ApplicationModel::ExtendedExecution::ExtendedExecutionSession^ session;
        Windows::System::Threading::ThreadPoolTimer^ periodicTimer;
        Windows::Foundation::EventRegistrationToken sessionRevokedToken;
        Windows::Devices::Geolocation::Geolocator^ geolocator;

        void ClearExtendedExecution();
        void UpdateUI();
        Concurrency::task<void> StartLocationTrackingAsync();
        void OnTimer(Windows::System::Threading::ThreadPoolTimer^ timer);
        void SessionRevoked(Platform::Object^ sender, Windows::ApplicationModel::ExtendedExecution::ExtendedExecutionRevokedEventArgs^ args);
    };
}
