//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#include "pch.h"
#include "Scenario4.g.h"
#include "WASAPICapture.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4 : Scenario4T<Scenario4>, MediaFoundationInitializer
    {
        Scenario4();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        void btnStartCapture_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnStopCapture_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        event_token m_deviceStateChangeToken;
        event_token m_plotDataReadyToken;

        int m_discontinuityCount;
        bool m_isLowLatency;
        com_ptr<WASAPICapture> m_capture;

        // UI Helpers
        void UpdateMediaControlUI(DeviceState deviceState);

        // Handlers
        fire_and_forget OnDeviceStateChange(IDeviceStateSource const& sender, SDKTemplate::DeviceStateChangedEventArgs e);
        fire_and_forget OnPlotDataReady(IPlotDataSource const& sender, SDKTemplate::PlotDataReadyEventArgs e);

        void InitializeCapture();
        void StopCapture();
        void ClearCapture();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4 : Scenario4T<Scenario4, implementation::Scenario4>
    {
    };
}
