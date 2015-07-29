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

namespace SDKSample
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref class DeviceInformationViewTemplate sealed
    {
    public:
        property Platform::String^ Name;
        property Platform::String^ Id;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3 sealed
    {
    public:
        Scenario3();
    private:
        int thisViewId;
        void StartProjecting(Windows::Devices::Enumeration::DeviceInformation^ selectedDisplay);
        void LoadAndDisplayScreens_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void displayList_listview_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
    };
}
