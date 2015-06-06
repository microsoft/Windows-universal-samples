//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario2.xaml.h
// Declaration of the Scenario2 class
//

#pragma once

#include "pch.h"
#include "Scenario2.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();

    private:
        void GroupByMonthButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void GroupByRatingButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void GroupByTagButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void GroupByHelper(Windows::Storage::Search::QueryOptions^ queryOptions);
        Windows::UI::Xaml::Controls::TextBlock^ CreateHeaderTextBlock(Platform::String^ contents);
        Windows::UI::Xaml::Controls::TextBlock^ CreateLineItemTextBlock(Platform::String^ contents);
    };
}
