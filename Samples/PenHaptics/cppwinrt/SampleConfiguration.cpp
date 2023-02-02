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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Devices::Haptics;
using namespace winrt::SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"PenHaptics C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Ink Canvas Tactile Feedback", xaml_typename<SDKTemplate::Scenario1_InkCanvasTactileFeedback>() },
    Scenario{ L"Query Tactile Feedback Support", xaml_typename<SDKTemplate::Scenario2_SupportedTactileFeedback>() },
    Scenario{ L"Inking Feedback", xaml_typename<SDKTemplate::Scenario3_InkingFeedback>() },
    Scenario{ L"Interaction Feedback", xaml_typename<SDKTemplate::Scenario4_InteractionFeedback>() },
    Scenario{ L"Inking and Interaction Feedback", xaml_typename<SDKTemplate::Scenario5_InkingAndInteractionFeedback>() },
});

std::map<hstring, uint16_t> const& SDKTemplate::WaveformNamesMap()
{
    static const std::map<winrt::hstring, uint16_t> map
    {
        { L"BrushContinuous", KnownSimpleHapticsControllerWaveforms::BrushContinuous() },
        { L"BuzzContinuous", KnownSimpleHapticsControllerWaveforms::BuzzContinuous() },
        { L"ChiselMarkerContinuous", KnownSimpleHapticsControllerWaveforms::ChiselMarkerContinuous() },
        { L"Click", KnownSimpleHapticsControllerWaveforms::Click() },
        { L"EraserContinuous", KnownSimpleHapticsControllerWaveforms::EraserContinuous() },
        { L"Error", KnownSimpleHapticsControllerWaveforms::Error() },
        { L"GalaxyPenContinuous", KnownSimpleHapticsControllerWaveforms::GalaxyPenContinuous() },
        { L"Hover", KnownSimpleHapticsControllerWaveforms::Hover() },
        { L"InkContinuous", KnownSimpleHapticsControllerWaveforms::InkContinuous() },
        { L"MarkerContinuous", KnownSimpleHapticsControllerWaveforms::MarkerContinuous() },
        { L"PencilContinuous", KnownSimpleHapticsControllerWaveforms::PencilContinuous() },
        { L"Press", KnownSimpleHapticsControllerWaveforms::Press() },
        { L"Release", KnownSimpleHapticsControllerWaveforms::Release() },
        { L"RumbleContinuous", KnownSimpleHapticsControllerWaveforms::RumbleContinuous() },
        { L"Success", KnownSimpleHapticsControllerWaveforms::Success() },
    };
    return map;
}

SimpleHapticsControllerFeedback SDKTemplate::FindSupportedFeedback(SimpleHapticsController const& hapticsController, uint16_t waveform)
{
    for (auto&& feedback : hapticsController.SupportedFeedback())
    {
        if (feedback.Waveform() == waveform)
        {
            return feedback;
        }
    }
    return nullptr;
}
