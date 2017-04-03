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

#include "Scenario2.g.h"
#include "MainPage.xaml.h"
#include "SampleDataModel.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();

    property Windows::Foundation::Collections::IVector<SampleDataModel^>^ AllItems;

    private:
        MainPage^ rootPage = MainPage::Current;

        SampleDataModel^ GetDataModelForCurrentListViewFlyout();
        void OpenMenuItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PrintMenuItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DeleteMenuItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
