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

#include "pch.h"
#include "Scenario1_Files.h"
#include "Scenario1_Files.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Files::Scenario1_Files()
    {
        InitializeComponent();
    }

    // Guidance for Local, LocalCache, and Temporary files.
    //
    // Files are ideal for storing large data-sets, databases, or data that is
    // in a common file-format.
    //
    // Files can exist in either the Local, LocalCache, or Temporary folders.
    // (They can also be put in Roaming folders, but the data no longer roams.)
    //
    // Local files are not synchronized, but they are backed up, and can then be restored to a 
    // machine different than where they were originally written. These should be for 
    // important files that allow the feel that the user did not lose anything
    // when they restored to a new device.
    //
    // Temporary files are subject to deletion when not in use.  The system 
    // considers factors such as available disk capacity and the age of a file when
    // determining when or whether to delete a temporary file.
    //
    // LocalCache files are for larger files that can be recreated by the app, and for
    // machine specific or private files that should not be restored to a new device.

    fire_and_forget Scenario1_Files::Increment_Local_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto strong = get_strong();

        localCounter++;

        StorageFile file = co_await localFolder.CreateFileAsync(filename, CreationCollisionOption::ReplaceExisting);
        co_await FileIO::WriteTextAsync(file, std::to_wstring(localCounter));

        Read_Local_Counter();
    }

    fire_and_forget Scenario1_Files::Read_Local_Counter()
    {
        auto strong = get_strong();

        StorageFile file = (co_await localFolder.TryGetItemAsync(filename)).as<StorageFile>();
        if (file != nullptr)
        {
            hstring text = co_await FileIO::ReadTextAsync(file);

            LocalOutputTextBlock().Text(L"Local Counter: " + text);

            localCounter = wcstol(text.c_str(), nullptr, 10);
        }
        else
        {
            LocalOutputTextBlock().Text(L"Local Counter: <not found>");
        }
    }

    fire_and_forget Scenario1_Files::Increment_LocalCache_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto strong = get_strong();
        localCacheCounter++;

        StorageFile file = co_await localCacheFolder.CreateFileAsync(filename, CreationCollisionOption::ReplaceExisting);
        co_await FileIO::WriteTextAsync(file, std::to_wstring(localCacheCounter));

        Read_LocalCache_Counter();
    }

    fire_and_forget Scenario1_Files::Read_LocalCache_Counter()
    {
        auto strong = get_strong();

        StorageFile file = (co_await localCacheFolder.TryGetItemAsync(filename)).as<StorageFile>();
        if (file != nullptr)
        {
            hstring text = co_await FileIO::ReadTextAsync(file);

            LocalCacheOutputTextBlock().Text(L"LocalCache Counter: " + text);

            localCacheCounter = wcstol(text.c_str(), nullptr, 10);
        }
        else
        {
            LocalCacheOutputTextBlock().Text(L"LocalCache Counter: <not found>");
        }
    }

    fire_and_forget Scenario1_Files::Increment_Temporary_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto strong = get_strong();
        temporaryCounter++;

        StorageFile file = co_await temporaryFolder.CreateFileAsync(filename, CreationCollisionOption::ReplaceExisting);
        co_await FileIO::WriteTextAsync(file, std::to_wstring(temporaryCounter));

        Read_Temporary_Counter();
    }

    fire_and_forget Scenario1_Files::Read_Temporary_Counter()
    {
        StorageFile file = (co_await temporaryFolder.TryGetItemAsync(filename)).as<StorageFile>();
        if (file != nullptr)
        {
            hstring text = co_await FileIO::ReadTextAsync(file);

            TemporaryOutputTextBlock().Text(L"Temporary Counter: " + text);

            temporaryCounter = wcstol(text.c_str(), nullptr, 10);
        }
        else
        {
            TemporaryOutputTextBlock().Text(L"Temporary Counter: <not found>");
        }
    }

    void Scenario1_Files::DisplayOutput()
    {
        Read_Local_Counter();
        Read_LocalCache_Counter();
        Read_Temporary_Counter();
    }

    void Scenario1_Files::OnNavigatedTo(NavigationEventArgs const&)
    {
        DisplayOutput();
    }
}

