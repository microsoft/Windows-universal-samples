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
                return "Simple Communication";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        static void EnsureExtensionRegistration()
        {
            if (mediaExtensionManager == nullptr)
            {
                mediaExtensionManager = ref new Windows::Media::MediaExtensionManager();
                mediaExtensionManager->RegisterSchemeHandler("Microsoft.Samples.SimpleCommunication.StspSchemeHandler", "stsp:");
            }
        }

    private:
        static Platform::Array<Scenario>^ scenariosInner;
        static Windows::Media::MediaExtensionManager^ mediaExtensionManager;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
