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
#include "Scenario3_InkingFeedback.h"
#include "Scenario3_InkingFeedback.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Haptics;
using namespace Windows::Devices::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;


// This sample shows how to play inking feedback tactile signals, how to
// adjust the intensity of the tactile signal, and how to fall back to a default
// waveform if the desired waveform is not supported by the current PenDevice
namespace winrt::SDKTemplate::implementation
{
    Scenario3_InkingFeedback::Scenario3_InkingFeedback()
    {
        InitializeComponent();
    }

    // The PointerEnter event will get fired as soon as Pointer input is received.
    // This event handler implementation will query the device providing input to see if it's a pen and
    // then check to see the pen supports tactile feedback and, if so, configure the PenDevice
    // to send the appropriate tactile signal based on what is selected in the dropdown.
    void Scenario3_InkingFeedback::HapticCanvas_Entered(IInspectable const&, PointerRoutedEventArgs const& e)
    {
        // If the current Pointer device is not a pen, exit
        if (e.Pointer().PointerDeviceType() != PointerDeviceType::Pen)
        {
            return;
        }

        // Attempt to retrieve the PenDevice from the current PointerId
        penDevice = PenDevice::GetFromPointerId(e.Pointer().PointerId());

        // If a PenDevice cannot be retrieved based on the PointerId, it does not support
        // advanced pen features, such as tactile feedback
        if (penDevice == nullptr)
        {
            statusText().Text(L"Advanced pen features not supported");
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

        // Get feedback based on the user's selected waveform and what the haptic controller supports
        std::wstring message;
        currentFeedback = GetSelectedFeedbackOrFallback(message);

        // Send the current feedback to the PenDevice's SimpleHapticsController.
        // Once sent, inking tactile feedback will be triggered as soon as the pen tip touches
        // the screen and will be stopped once the pen tip is lifted from the screen.
        // Also, check to see if the current PenDevice's SimpleHapticsController supports
        // setting the intensity value of the tactile feedback.  If so, set it based
        // on the slider.  If not, send the waveform without custom intensity.
        if (hapticsController.IsIntensitySupported())
        {
            hapticsController.SendHapticFeedback(currentFeedback, intensitySlider().Value() / 100.0);
            message.append(L"\nIntensity set to " + to_hstring(intensitySlider().Value()) + L"%");
        }
        else
        {
            hapticsController.SendHapticFeedback(currentFeedback);
            message.append(L"\nSetting intensity is not supported by this pen");
        }
        statusText().Text(message);
    }

    // Stop sending the tactile feedback and clear the current penDevice and hapticsController on PointerExit.
    // Stopping the feedback is important as it clears the tactile signal from the PenDevice's
    // SimpleHapticsController, ensuring that it has a clean state once it next enters range.
    void Scenario3_InkingFeedback::HapticCanvas_Exited(IInspectable const&, PointerRoutedEventArgs const&)
    {
        penDevice = nullptr;
        statusText().Text(L"");
        if (hapticsController != nullptr)
        {
            hapticsController.StopFeedback();
            hapticsController = nullptr;
            currentFeedback = nullptr;
        }
    }

    // Get feedback which matches the currently selected waveform in the dropdown.
    // To get the feedback, the SimpleHapticsController's SupportedFeedback list must be traversed
    // to see if a matching feedback is present.
    // This is important because InkContinuous is the only Inking waveform required to be
    // supported by every pen with haptics.
    // If no matching feedback is found in the SupportedFeedback list, this method returns
    // InkContinuous as a guaranteed fallback.
    SimpleHapticsControllerFeedback Scenario3_InkingFeedback::GetSelectedFeedbackOrFallback(std::wstring& message)
    {
        // Look up the waveform the user selected.
        hstring name = waveformComboBox().SelectionBoxItem().as<hstring>();
        uint16_t waveform = WaveformNamesMap().at(name);

        // See if the haptics controller supports the selected waveform.
        SimpleHapticsControllerFeedback feedback = FindSupportedFeedback(hapticsController, waveform);
        if (feedback != nullptr)
        {
            message = L"Waveform set to " + name;
            return feedback;
        }
        
        // It does not. Use InkContinuous as a fallback.
        message = name + L" is not supported by this pen, so falling back to InkContinuous";
        return FindSupportedFeedback(hapticsController, KnownSimpleHapticsControllerWaveforms::InkContinuous());
    }
}
