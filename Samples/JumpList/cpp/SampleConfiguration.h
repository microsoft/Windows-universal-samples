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
                return "Resize app view";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        property Platform::String^ LaunchParam
        {
            Platform::String^ get()
            {
                return launchParam;
            }

            void set(Platform::String^ value);
        }

    private:
        static Platform::Array<Scenario>^ scenariosInner;
        Platform::String^ launchParam;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };

    partial ref class App
    {
        void Partial_LaunchCompleted(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e);
    public:
        void OnFileActivated(Windows::ApplicationModel::Activation::FileActivatedEventArgs^ args) override;
    };
}
