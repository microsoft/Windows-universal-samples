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
#include "Scenario3.g.h"
#include "MainPage.h"
#include "WASAPIRenderer.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3 : Scenario3T<Scenario3>, MediaFoundationInitializer
    {
        Scenario3();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void btnPlay_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnPause_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnPlayPause_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnStop_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnFilePicker_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void radioTone_Checked(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void radioFile_Checked(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void sliderFrequency_ValueChanged(IInspectable const& sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        void sliderVolume_ValueChanged(IInspectable const& sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);

        // UI Helpers
        void DisableContentUI();
        void UpdateContentProps(hstring const& text);
        void UpdateContentUI();
        void UpdateMediaControlUI(DeviceState deviceState);

        // Handlers
        fire_and_forget OnDeviceStateChange(IDeviceStateSource const& sender, SDKTemplate::DeviceStateChangedEventArgs e);
        fire_and_forget OnPickFileAsync();
        void OnSetVolume(double volume);

        void InitializeDevice();
        void StartDevice();
        void StopDevice();
        void PauseDevice();
        void PlayPauseToggleDevice();

        fire_and_forget MediaButtonPressed(Windows::Media::SystemMediaTransportControls const& sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Media::SystemMediaTransportControls m_systemMediaControls{ nullptr };
        event_token m_deviceStateChangeToken;
        event_token m_systemMediaControlsButtonToken;

        bool m_deviceSupportsRawMode = false;
        bool m_isMinimumLatency = false;

        Windows::Storage::Streams::IRandomAccessStream m_contentStream{ nullptr };
        ContentType m_contentType = ContentType::Tone;
        com_ptr<WASAPIRenderer> m_renderer;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3 : Scenario3T<Scenario3, implementation::Scenario3>
    {
    };
}
