// Copyright (c) Microsoft. All rights reserved.

#pragma once 
#include "pch.h"

namespace SDKTemplate
{
    value struct Scenario;

    partial ref class App
    {
    protected:
        void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs^ args) override;
    };

    partial ref class MainPage
    {
    internal:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return "Share source C++ sample";
            }
        }

        static property Platform::String^ MissingTitleError
        {
            Platform::String^ get()
            {
                return "Enter a title for what you are sharing and try again.";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        void LoadScenarioForProtocolActivation(Windows::ApplicationModel::Activation::ProtocolActivatedEventArgs^ args);

    private:
        static Platform::Array<Scenario>^ scenariosInner;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
