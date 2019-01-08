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
                return "Line Display C++ Sample";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        Platform::String^ LineDisplayId = nullptr;

        Concurrency::task<Windows::Devices::PointOfService::ClaimedLineDisplay^> ClaimScenarioLineDisplayAsync();

    private:
        static Platform::Array<Scenario>^ scenariosInner;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };

    namespace Helpers
    {
        template<typename T>
        T ParseUIntWithFallback(Windows::UI::Xaml::Controls::TextBox^ textBox, T fallback)
        {
            wchar_t* end;
            errno = 0;
            unsigned long value = wcstoul(textBox->Text->Data(), &end, 10);
            if (errno == ERANGE || *end != L'\0')
            {
                return fallback;
            }

            return static_cast<T>(value);
        }

        void AddItem(Windows::UI::Xaml::Controls::ComboBox^ comboBox, Platform::Object^ value, Platform::String^ name);

        template<typename T>
        void AddItem(Windows::UI::Xaml::Controls::ComboBox^ comboBox, T value)
        {
            AddItem(comboBox, value, value.ToString());
        }

        template<typename T>
        T GetSelectedItemTag(Windows::UI::Xaml::Controls::Primitives::Selector^ selector)
        {
            return safe_cast<T>(safe_cast<Windows::UI::Xaml::FrameworkElement^>(selector->SelectedItem)->Tag);
        }

        inline Windows::Foundation::TimeSpan TimeSpanFromMilliseconds(double value)
        {
            return Windows::Foundation::TimeSpan{ static_cast<long long>(value * 10000) };
        }

        inline double MillisecondsFromTimeSpan(Windows::Foundation::TimeSpan value)
        {
            return static_cast<double>(value.Duration) / 10000;
        }
    }

    template<typename TLambda>
    Concurrency::task<void> invoke_async_lambda(TLambda lambda)
    {
        co_await lambda();
    }
}
