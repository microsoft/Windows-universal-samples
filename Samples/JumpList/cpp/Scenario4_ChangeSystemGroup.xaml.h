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

#include "Scenario4_ChangeSystemGroup.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_ChangeSystemGroup sealed
    {
    public:
        Scenario4_ChangeSystemGroup();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void SystemGroup_Recent_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SystemGroup_Frequent_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SystemGroup_None_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PrepareSampleFiles_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
