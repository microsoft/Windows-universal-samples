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
#include "SampleConfiguration.h"
#include "Scenario2_SupportedTactileFeedback.h"
#include "Scenario2_SupportedTactileFeedback.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Haptics;
using namespace Windows::Devices::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;

// This sample shows how to query the pen currently providing input to the system to see
// if it supports tactile feedback and, if so, what features it supports
namespace winrt::SDKTemplate::implementation
{
    Scenario2_SupportedTactileFeedback::Scenario2_SupportedTactileFeedback()
    {
        InitializeComponent();
    }

    // The PointerEnter event will get fired as soon as Pointer input is received.
    // This event handler implementation will query the device providing input to see if it's a pen and
    // then check to see the pen supports tactile feedback and, if so, what features it supports
    void Scenario2_SupportedTactileFeedback::Pointer_Entered(IInspectable const&, PointerRoutedEventArgs const& e)
    {
        // If the current Pointer device is not a pen, exit
        if (e.Pointer().PointerDeviceType() != PointerDeviceType::Pen)
        {
            supportedFeatures().Text(L"");
            supportedFeedback().Text(L"");
            return;
        }

        // Attempt to retrieve the PenDevice from the current PointerId
        penDevice = PenDevice::GetFromPointerId(e.Pointer().PointerId());

        // If a PenDevice cannot be retrieved based on the PointerId, it does not support
        // advanced pen features, such as tactile feedback
        if (penDevice == nullptr)
        {
            statusText().Text(L"Advanced pen features not supported");
            supportedFeatures().Text(L"");
            supportedFeedback().Text(L"");
            return;
        }

        // Check to see if the current PenDevice supports tactile feedback by seeing if it
        // has a SimpleHapticsController
        hapticsController = penDevice.SimpleHapticsController();
        if (hapticsController == nullptr)
        {
            statusText().Text(L"This pen does not provide tactile feedback");
            return;
        }

        // Check which tactile feedback features are supported
        std::wstring message = L"Supported Haptics Features:\n";
        if (hapticsController.IsIntensitySupported())
        {
            message.append(L"Intensity\n");
        }
        if (hapticsController.IsPlayCountSupported())
        {
            message.append(L"PlayCount\n");
        }
        if (hapticsController.IsPlayDurationSupported())
        {
            message.append(L"PlayDuration\n");
        }
        if (hapticsController.IsReplayPauseIntervalSupported())
        {
            message.append(L"ReplayPauseInterval\n");
        }
        supportedFeatures().Text(message);

        // Check which feedback waveforms are supported
        message = L"Supported Feedback:\n";
        for (SimpleHapticsControllerFeedback feedback : hapticsController.SupportedFeedback())
        {
            uint16_t waveform = feedback.Waveform();
            for (auto [name, value] : WaveformNamesMap())
            {
                if (waveform == value)
                {
                    message.append(name);
                    message.append(L"\n");
                    break;
                }
            }
        }
        supportedFeedback().Text(message);
        statusText().Text(L"");
    }

    // Clear the current penDevice and hapticsController on PointerExit
    void Scenario2_SupportedTactileFeedback::Pointer_Exited(IInspectable const&, PointerRoutedEventArgs const&)
    {
        penDevice = nullptr;
        hapticsController = nullptr;
    }
}
