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

#include "Scenario3.g.h"
#include "MainPage.xaml.h"
#include "SampleDataModel.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3 sealed
    {
    public:
        Scenario3();

        property Windows::Foundation::Collections::IVector<SampleDataModel^>^ AllItems;

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::UI::Xaml::Controls::MenuFlyout^ sharedFlyout;

        void ListView_ContextRequested(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::Input::ContextRequestedEventArgs^ e);
        void ListView_ContextCanceled(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        SampleDataModel^ GetDataModelForCurrentListViewFlyout();

        void OpenMenuItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PrintMenuItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DeleteMenuItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
