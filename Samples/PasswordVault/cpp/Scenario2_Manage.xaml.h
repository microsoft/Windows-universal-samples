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

#include "Scenario2_Manage.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Manage sealed
    {
    public:
        Scenario2_Manage();

    internal:
        void RetrieveCredentials_Click(Platform::Object^, Platform::Object^);
        void RevealPasswords_Click(Platform::Object^, Platform::Object^);
        void RemoveCredentials_Click(Platform::Object^, Platform::Object^);

    private:
        MainPage^ rootPage = MainPage::Current;

        static const int ElementNotFound = (int)0x80070490;
    };
}
