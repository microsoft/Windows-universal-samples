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
#include "Scenario2.h"
#include <sstream>
#include "Scenario2.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2::Scenario2()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario2::PickFilesButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Clear any previously returned files between iterations of this scenario
        OutputTextBlock().Text(L"");

        FileOpenPicker openPicker;
        openPicker.ViewMode(PickerViewMode::List);
        openPicker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);
        openPicker.FileTypeFilter().Append(L"*");
        IVectorView<StorageFile> files = co_await openPicker.PickMultipleFilesAsync();
        if (files.Size() > 0)
        {
            std::wstringstream output;
            output << L"Picked files:" << std::endl;
            // Application now has read/write access to the picked file(s)
            for (StorageFile const& file : files)
            {
                output << std::wstring_view{ file.Name() } << std::endl;                
            }
            OutputTextBlock().Text(output.str());
        }
        else
        {
            OutputTextBlock().Text(L"Operation cancelled.");
        }
    }
}
