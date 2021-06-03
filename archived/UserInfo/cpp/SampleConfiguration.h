// Copyright (c) Microsoft. All rights reserved.

#pragma once 
#include "pch.h"

namespace SDKTemplate
{
    value struct Scenario;

    partial ref class MainPage
    {
    internal:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return "User info C++ sample";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

    private:
        static Platform::Array<Scenario>^ scenariosInner;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };

    // This is an observable object that we data-bind the combo box / list box to.
    [Windows::UI::Xaml::Data::Bindable]
    public ref class UserViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        UserViewModel(Platform::String^ userId, Platform::String^ displayName)
        {
            this->userId = userId;
            this->DisplayName = displayName;
        }

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        property Platform::String^ UserId
        {
            Platform::String^ get()
            {
                return userId;
            }
        }

        property Platform::String^ DisplayName
        {
            Platform::String^ get()
            {
                return displayName;
            }

            void set(Platform::String^ value)
            {
                displayName = value;
                PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("DisplayName"));
            }
        }

    private:
        Platform::String^ userId;
        Platform::String^ displayName;
    };
}
