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
                return "My People notifications C++ Sample";
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

    /// <summary>
    /// Contains constant values that are used to create the sample contact. These values are later
    /// used in the notification payload to resolve the contact.
    /// </summary>
    namespace Constants
    {
        const Platform::StringReference c_ContactRemoteId = L"{D44056FA-6C0E-47BE-B984-0974B21FF59D}";
        const Platform::StringReference c_ContactListName = L"MyPeopleNotificationsSample";
        const Platform::StringReference c_ContactEmail = L"johndoe@example.com";
        const Platform::StringReference c_ContactPhone = L"4255550123";
    }
}
