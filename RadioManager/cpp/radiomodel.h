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

namespace SDKTemplate
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref class RadioModel sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    private:
        Windows::Devices::Radios::Radio^ radio;
        bool isEnabled = true;
        Windows::UI::Xaml::UIElement^ parent;

    public:

        RadioModel(Windows::Devices::Radios::Radio^ radio, Windows::UI::Xaml::UIElement^ parent);

        property Platform::String^ Name
        {
            Platform::String^ get() { return this->radio->Name; }
        }

        property bool IsRadioOn
        {
            bool get() { return this->radio->State == Windows::Devices::Radios::RadioState::On; }
            void set(bool value)
            {
                SetRadioState(value);
            }
        }

        property bool IsEnabled
        {
            bool get() { return this->isEnabled; }
            void set(bool value)
            {
                this->isEnabled = value;
                NotifyPropertyChanged("IsEnabled");
            }
        }

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

    private:
        void NotifyPropertyChanged(Platform::String^ propertyName);
        void SetRadioState(bool isRadioOn);
        void Radio_StateChanged(Windows::Devices::Radios::Radio^ sender, Object^ args);
        void Enable();
        void Disable();

        template<typename Func>
        void Dispatch(Func function);
    };
};