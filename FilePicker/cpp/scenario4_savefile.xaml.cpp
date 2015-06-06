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
// Scenario4.xaml.cpp
// Implementation of the Scenario4 class
//

#include "pch.h"
#include "Scenario4_SaveFile.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Provider;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario4::Scenario4()
{
    InitializeComponent();
    rootPage = MainPage::Current;
    SaveFileButton->Click += ref new RoutedEventHandler(this, &Scenario4::SaveFileButton_Click);
}

void Scenario4::SaveFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Clear previous returned file name, if it exists, between iterations of this scenario
    OutputTextBlock->Text = "";

    FileSavePicker^ savePicker = ref new FileSavePicker();
    savePicker->SuggestedStartLocation = PickerLocationId::DocumentsLibrary;

    auto plainTextExtensions = ref new Platform::Collections::Vector<String^>();
    plainTextExtensions->Append(".txt");
    savePicker->FileTypeChoices->Insert("Plain Text", plainTextExtensions);
    savePicker->SuggestedFileName = "New Document";

    create_task(savePicker->PickSaveFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            // Prevent updates to the remote version of the file until we finish making changes and call CompleteUpdatesAsync.
            CachedFileManager::DeferUpdates(file);
            // write to file
            create_task(FileIO::WriteTextAsync(file, file->Name)).then([this, file]()
            {
                // Let Windows know that we're finished changing the file so the other app can update the remote version of the file.
                // Completing updates may require Windows to ask for user input.
                create_task(CachedFileManager::CompleteUpdatesAsync(file)).then([this, file](FileUpdateStatus status)
                {
                    if (status == FileUpdateStatus::Complete)
                    {
                        OutputTextBlock->Text = "File " + file->Name + " was saved.";
                    }
                    else
                    {
                        OutputTextBlock->Text = "File " + file->Name + " couldn't be saved.";
                    }
                });
            });
        }
        else
        {
            OutputTextBlock->Text = "Operation cancelled.";
        }
    });
}
