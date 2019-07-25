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
#include "SampleConfiguration.h"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario4::Scenario4()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario4::GetFilesButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Reset output.
        OutputPanel().Children().Clear();

        // Ask the user to pick a folder.
        FolderPicker picker;
        picker.FileTypeFilter().Append(L"*");
        picker.ViewMode(PickerViewMode::List);
        picker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);
        StorageFolder folder = co_await picker.PickSingleFolderAsync();
        if (folder != nullptr)
        {
            // Query the folder.
            StorageFileQueryResult query = folder.CreateFileQuery();
            IVectorView<StorageFile> fileList = co_await query.GetFilesAsync();


            // Display file list with storage provider and availability.
            for (StorageFile const& file : fileList)
            {
                std::wstringstream output;

                // Create an entry in the list for the item.
                output << std::wstring_view{ file.Name() };

                // Show the item's provider (This PC, OneDrive, Network, or Application Content).
                output << L": On " << std::wstring_view{ file.Provider().DisplayName() };

                // Show if the item is available (OneDrive items are usually available when
                // online or when they are marked for "always available offline").
                output << L" (";
                if (file.IsAvailable())
                {
                    output << L"available";
                }
                else
                {
                    output << L"not available";
                }
                output << L")";
                OutputPanel().Children().Append(Helpers::CreateLineItemTextBlock(output.str()));
            }
        }

    }
}
