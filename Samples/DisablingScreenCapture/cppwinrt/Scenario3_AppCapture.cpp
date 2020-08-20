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
#include "Scenario3_AppCapture.h"
#include "Scenario3_AppCapture.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_AppCapture::Scenario3_AppCapture()
    {
        InitializeComponent();
    }

    void Scenario3_AppCapture::OnNavigatedTo(NavigationEventArgs const&)
    {
        appCapture = AppCapture::GetForCurrentView();
        capturingChangedToken = appCapture.CapturingChanged({ get_weak(), &Scenario3_AppCapture::AppCapture_CapturingChanged });
        UpdateCaptureStatus();
    }

    void Scenario3_AppCapture::OnNavigatedFrom(NavigationEventArgs const&)
    {
        appCapture.CapturingChanged(capturingChangedToken);

        // Re-enable app capture when navigating away.
        AppCapture::SetAllowedAsync(true);
    }

    fire_and_forget Scenario3_AppCapture::AllowAppCaptureCheckBox_Toggled(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        ToggleSwitch toggleSwitch = sender.as<ToggleSwitch>();
        co_await AppCapture::SetAllowedAsync(toggleSwitch.IsOn());
    }
    
    void Scenario3_AppCapture::AppCapture_CapturingChanged(AppCapture const&, IInspectable const&)
    {
        UpdateCaptureStatus();
    }

    void Scenario3_AppCapture::UpdateCaptureStatus()
    {
        hstring audioStatus = appCapture.IsCapturingAudio() ? L"Audio is being captured." : L"Audio is not being captured.";
        hstring videoStatus = appCapture.IsCapturingVideo() ? L"Video is being captured." : L"Video is not being captured.";
        rootPage.NotifyUser(audioStatus + L"\n" + videoStatus, NotifyType::StatusMessage);
    }
}
