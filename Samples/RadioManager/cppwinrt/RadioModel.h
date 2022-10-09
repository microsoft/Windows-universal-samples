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
#include "pch.h"
#include "RadioModel.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct RadioModel : RadioModelT<RadioModel>
    {
        RadioModel(Windows::Devices::Radios::Radio const& radio, Windows::UI::Core::CoreDispatcher const& dispatcher);

        hstring Name()
        {
            return radio.Name();
        }

        bool IsEnabled()
        {
            return isEnabled;
        }

        bool IsRadioOn()
        {
            return radio.State() == Windows::Devices::Radios::RadioState::On;
        }

        void IsRadioOn(bool value)
        {
            SetRadioState(value);
        }

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return propertyChanged.add(handler);
        }

        void PropertyChanged(winrt::event_token const& token)
        {
            propertyChanged.remove(token);
        }

    private:
        Windows::Devices::Radios::Radio radio;
        Windows::UI::Core::CoreDispatcher dispatcher;
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged;
        bool isEnabled;

        fire_and_forget Radio_StateChanged(Windows::Devices::Radios::Radio const&, Windows::Foundation::IInspectable const&);
        void NotifyPropertyChanged(hstring const& propertyName);
        void SetEnabled(bool value);
        fire_and_forget SetRadioState(bool value);        
    };
}
