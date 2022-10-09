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
#include "RadioModel.h"
#include "RadioModel.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Radios;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml::Data;
using namespace winrt::SDKTemplate;

namespace winrt::SDKTemplate::implementation
{
    RadioModel::RadioModel(Radio const& radio, CoreDispatcher const& dispatcher) :
        radio(radio), dispatcher(dispatcher),
        // Controlling the mobile broadband radio requires the cellularDeviceControl restricted capability, which we do not have.
        isEnabled(radio.Kind() != RadioKind::MobileBroadband)
    {
        radio.StateChanged({ get_weak(), &RadioModel::Radio_StateChanged });
    }

    fire_and_forget RadioModel::Radio_StateChanged(Radio const&, IInspectable const&)
    {
        // The Radio StateChanged event doesn't run from the UI thread, so we must use the dispatcher
        // to run NotifyPropertyChanged
        auto lifetime = get_strong();
        co_await resume_foreground(dispatcher);
        NotifyPropertyChanged(L"IsRadioOn");
    }

    void RadioModel::NotifyPropertyChanged(hstring const& propertyName)
    {
        propertyChanged(*this, PropertyChangedEventArgs(propertyName));
    }

    void RadioModel::SetEnabled(bool value)
    {
        isEnabled = value;
        NotifyPropertyChanged(L"IsEnabled");
    }

    fire_and_forget RadioModel::SetRadioState(bool value)
    {
        if (value != IsRadioOn())
        {
            auto lifetime = get_strong();
            RadioState newState = value ? RadioState::On : RadioState::Off;
            SetEnabled(false);
            co_await radio.SetStateAsync(newState);
            SetEnabled(true);
            NotifyPropertyChanged(L"IsRadioOn");
        }
    }

}
