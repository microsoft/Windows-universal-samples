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

#include "Scenario1_ShortName.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_ShortName sealed
    {
    public:
        Scenario1_ShortName();
    private:
        MainPage^ rootPage = MainPage::Current;

        void StatusMessage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ErrorMessage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ClearMessage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
