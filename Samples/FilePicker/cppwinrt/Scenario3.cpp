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
#include "Scenario3.h"
#include "Scenario3.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario3::Scenario3()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario3::PickFolderButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Clear previous returned folder name, if it exists, between iterations of this scenario
        OutputTextBlock().Text(L"");

        FolderPicker folderPicker;
        folderPicker.SuggestedStartLocation(PickerLocationId::Desktop);
        folderPicker.FileTypeFilter().ReplaceAll({ L".docx",  L".xlsx", L".pptx" });
        StorageFolder folder = co_await folderPicker.PickSingleFolderAsync();
        if (folder != nullptr)
        {
            // Application now has read/write access to all contents in the picked folder (including other sub-folder contents)
            StorageApplicationPermissions::FutureAccessList().AddOrReplace(L"PickedFolderToken", folder);
            OutputTextBlock().Text(L"Picked folder: " + folder.Name());
        }
        else
        {
            OutputTextBlock().Text(L"Operation cancelled.");
        }
    }

}
