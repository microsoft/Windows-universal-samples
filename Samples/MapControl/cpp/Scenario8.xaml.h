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

#include "Scenario8.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario8 sealed
    {
    public:
        Scenario8();
    private:
        MainPage^ rootPage = MainPage::Current;
        void myMap_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void findAtCenterButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void addToResultGrid(Windows::Services::Maps::OfflineMaps::OfflineMapPackage^ package);
        static unsigned long long nearestMegabyte(unsigned long long bytes);
        static bool canDownload(Windows::Services::Maps::OfflineMaps::OfflineMapPackage^ package);
    };
}
