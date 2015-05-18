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
// Scenario3.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include "Scenario3_SingleFolder.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario3::Scenario3()
{
    InitializeComponent();
    rootPage = MainPage::Current;
    PickFolderButton->Click += ref new RoutedEventHandler(this, &Scenario3::PickFolderButton_Click);
}

void Scenario3::PickFolderButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Clear previous returned folder name, if it exists, between iterations of this scenario
    OutputTextBlock->Text = "";

    FolderPicker^ folderPicker = ref new FolderPicker();
    folderPicker->SuggestedStartLocation = PickerLocationId::Desktop;

    // Users expect to have a filtered view of their folders depending on the scenario.
    // For example, when choosing a documents folder, restrict the filetypes to documents for your application.
    folderPicker->FileTypeFilter->Append(".docx");
    folderPicker->FileTypeFilter->Append(".xlsx");
    folderPicker->FileTypeFilter->Append(".pptx");

    create_task(folderPicker->PickSingleFolderAsync()).then([this](StorageFolder^ folder)
    {
        if (folder)
        {
            OutputTextBlock->Text = "Picked folder: " + folder->Name;
        }
        else
        {
            OutputTextBlock->Text = "Operation cancelled.";
        }
    });
}
