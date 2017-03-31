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

#include "Scenario3_CustomItems.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_CustomItems sealed
    {
    public:
        Scenario3_CustomItems();

    private:
        void DisplayNameLocalizedResource_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DescriptionLocalizedResource_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void GroupNameLocalizedResource_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AddToJumpList_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
