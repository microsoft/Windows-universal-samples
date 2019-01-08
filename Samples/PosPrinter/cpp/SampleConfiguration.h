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
                return "POS Printer C++ Sample";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        Windows::Devices::PointOfService::PosPrinter^ Printer = nullptr;
        Windows::Devices::PointOfService::ClaimedPosPrinter^ ClaimedPrinter = nullptr;
        Windows::Devices::Enumeration::DeviceInformation^ deviceInfo = nullptr;

        bool IsAnImportantTransaction = true;
        std::function<void(void)> StateChanged;

        void SubscribeToReleaseDeviceRequested();
        void ReleaseClaimedPrinter();
        void ReleaseAllPrinters();

    private:
        static Platform::Array<Scenario>^ scenariosInner;

        Windows::Foundation::EventRegistrationToken releaseDeviceRequestedToken;
        void ClaimedPrinter_ReleaseDeviceRequested(Windows::Devices::PointOfService::ClaimedPosPrinter^ sender, Windows::Devices::PointOfService::PosPrinterReleaseDeviceRequestedEventArgs^ args);
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
