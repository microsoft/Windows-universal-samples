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
#include "Scenario1_SingleFile.h"
#include "Scenario1_SingleFile.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_SingleFile::Scenario1_SingleFile()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_SingleFile::PickAFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Clear previous returned file name, if it exists, between iterations of this scenario
        OutputTextBlock().Text(L"");

        FileOpenPicker openPicker;
        openPicker.ViewMode(PickerViewMode::Thumbnail);
        openPicker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
        openPicker.FileTypeFilter().ReplaceAll({ L".jpg", L".jpeg", L".png" });
        StorageFile file = co_await openPicker.PickSingleFileAsync();
        if (file != nullptr)
        {
            // The StorageFile has read/write access to the picked file.
            // See the FileAccess sample for code that uses a StorageFile to read and write.
            OutputTextBlock().Text(L"Picked photo: " + file.Name());
        }
        else
        {
            OutputTextBlock().Text(L"Operation cancelled.");
        }
    }
}
