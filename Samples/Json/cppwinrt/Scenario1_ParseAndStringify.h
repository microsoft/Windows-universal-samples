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

#include <winrt/SDKTemplate.h>
#include "Scenario1_ParseAndStringify.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_ParseAndStringify : Scenario1_ParseAndStringifyT<Scenario1_ParseAndStringify>
    {
        Scenario1_ParseAndStringify();

        void Parse_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void Stringify_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);        
        void Add_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void DeleteSchool_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        SDKTemplate::User CurrentUser() { return DataContext().as<SDKTemplate::User>(); }
        void CurrentUser(SDKTemplate::User const& user) { DataContext(user); }

        bool IsExceptionHandled(hresult_error const& ex);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_ParseAndStringify : Scenario1_ParseAndStringifyT<Scenario1_ParseAndStringify, implementation::Scenario1_ParseAndStringify>
    {
    };
}
