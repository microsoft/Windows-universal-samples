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

//
// Scenario2.xaml.cpp
// Implementation of the Scenario2 class
//

#include "pch.h"
#include "Scenario2_MultiFile.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2::Scenario2()
{
    InitializeComponent();
    rootPage = MainPage::Current;
    PickFilesButton->Click += ref new RoutedEventHandler(this, &Scenario2::PickFilesButton_Click);
}

void Scenario2::PickFilesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Clear any previously returned files between iterations of this scenario
    OutputTextBlock->Text = "";

    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    openPicker->ViewMode = PickerViewMode::List;
    openPicker->SuggestedStartLocation = PickerLocationId::DocumentsLibrary;
    openPicker->FileTypeFilter->Append("*");

    create_task(openPicker->PickMultipleFilesAsync()).then([this](IVectorView<StorageFile^>^ files)
    {
        if (files->Size > 0)
        {
            String^ output = "Picked files:\n";
            std::for_each(begin(files), end(files), [this, &output](StorageFile ^file)
            {
                output += file->Name + "\n";
            });
            OutputTextBlock->Text = output;
        }
        else
        {
            OutputTextBlock->Text = "Operation cancelled.";
        }
    });
}
