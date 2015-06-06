//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario4.xaml.cpp
// Implementation of the Scenario4 class
//

#include "pch.h"
#include "Scenario4.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Search;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario4::Scenario4()
{
    InitializeComponent();
}

void Scenario4::GetFilesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Reset output.
    OutputPanel->Children->Clear();

    // Ask the user to pick a folder.
    FolderPicker^ picker = ref new FolderPicker();
    picker->FileTypeFilter->Append("*");
    picker->ViewMode = PickerViewMode::List;
    picker->SuggestedStartLocation = PickerLocationId::DocumentsLibrary;

    create_task(picker->PickSingleFolderAsync()).then([=](StorageFolder^ folder)
    {
        if (folder != nullptr)
        {
            // Query the folder.
            auto query = folder->CreateFileQuery();
            create_task(query->GetFilesAsync()).then([=](IVectorView<StorageFile^>^ files)
            {
                // Display file list with storage provider and availability.
                std::for_each(begin(files), end(files), [=](StorageFile^ file)
                {
                    // Create an entry in the list for the item.
                    String^ line = file->Name;

                    // Show the item's provider (This PC, OneDrive, Network, or Application Content).
                    line += ": On " + file->Provider->DisplayName;

                    // Show if the item is available (OneDrive items are usually available when
                    // online or when they are marked for "always available offline").
                    line += " (";
                    if (file->IsAvailable)
                    {
                        line += "available";
                    }
                    else
                    {
                        line += "not available";
                    }
                    line += ")";

                    OutputPanel->Children->Append(CreateLineItemTextBlock(line));
                });
            });
        }
    });
}

TextBlock^ Scenario4::CreateLineItemTextBlock(String^ contents)
{
    TextBlock^ textBlock = ref new TextBlock();
    textBlock->Text = contents;
    textBlock->Style = static_cast<Xaml::Style^>(::Application::Current->Resources->Lookup("BasicTextStyle"));
    textBlock->TextWrapping = TextWrapping::Wrap;
    Thickness margin = textBlock->Margin;
    margin.Left = 20;
    textBlock->Margin = margin;
    return textBlock;
}
