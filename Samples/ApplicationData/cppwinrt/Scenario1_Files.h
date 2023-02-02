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

#include "Scenario1_Files.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Files : Scenario1_FilesT<Scenario1_Files>
    {
        Scenario1_Files();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget Increment_Local_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget Increment_LocalCache_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget Increment_Temporary_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

        void DisplayOutput();

    private:

        fire_and_forget Read_Local_Counter();
        fire_and_forget Read_LocalCache_Counter();
        fire_and_forget Read_Temporary_Counter();

        Windows::Storage::StorageFolder localFolder = Windows::Storage::ApplicationData::Current().LocalFolder();
        int localCounter = 0;
        Windows::Storage::StorageFolder localCacheFolder = Windows::Storage::ApplicationData::Current().LocalCacheFolder();
        int localCacheCounter = 0;
        Windows::Storage::StorageFolder temporaryFolder = Windows::Storage::ApplicationData::Current().TemporaryFolder();
        int temporaryCounter = 0;

        inline static const auto filename = L"sampleFile.txt";
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Files : Scenario1_FilesT<Scenario1_Files, implementation::Scenario1_Files>
    {
    };
}

