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
                return "Password vault C++ sample";
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

        // Value is not important. This is just a trick to run code at construction.
        bool initialize = InitializePasswordVaultInTheBackground();

        static bool InitializePasswordVaultInTheBackground();
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
