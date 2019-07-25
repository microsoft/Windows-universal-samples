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
#include "Scenario4.h"
#include "Scenario4.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Provider;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario4::Scenario4()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario4::SaveFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Clear previous returned file name, if it exists, between iterations of this scenario
        OutputTextBlock().Text(L"");

        FileSavePicker savePicker;
        savePicker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);
        // Dropdown of file types the user can save the file as
        savePicker.FileTypeChoices().Insert(L"Plain Text", single_threaded_vector<hstring>({ L".txt" }));
        // Default file name if the user does not type one in or select a file to replace
        savePicker.SuggestedFileName(L"New Document");
        StorageFile file = co_await savePicker.PickSaveFileAsync();
        if (file != nullptr)
        {
            // Prevent updates to the remote version of the file until we finish making changes and call CompleteUpdatesAsync.
            CachedFileManager::DeferUpdates(file);
            // write to file
            await FileIO::WriteTextAsync(file, L"Example file contents.");
            // Let Windows know that we're finished changing the file so the other app can update the remote version of the file.
            // Completing updates may require Windows to ask for user input.
            FileUpdateStatus status = co_await CachedFileManager::CompleteUpdatesAsync(file);
            if (status == FileUpdateStatus::Complete)
            {
                OutputTextBlock().Text(L"File " + file.Name() + L" was saved.");
            }
            else if (status == FileUpdateStatus::CompleteAndRenamed)
            {
                OutputTextBlock().Text(L"File " + file.Name() + L" was renamed and saved.");
            }
            else
            {
                OutputTextBlock().Text(L"File " + file.Name() + L" couldn't be saved.");
            }
        }
        else
        {
            OutputTextBlock().Text(L"Operation cancelled.");
        }
    }
}
