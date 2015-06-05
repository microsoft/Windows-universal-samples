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
// Scenario6.xaml.cpp
// Implementation of the Scenario6 class
//

#include "pch.h"
#include "Scenario6_SaveCachedFile.xaml.h"

using namespace SDKTemplate;
using namespace FilePicker;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Provider;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Globalization::DateTimeFormatting;

Scenario6::Scenario6()
{
    InitializeComponent();
    rootPage = MainPage::Current;
    SaveFileButton->Click += ref new RoutedEventHandler(this, &Scenario6::SaveFileButton_Click);
    WriteFileButton->Click += ref new RoutedEventHandler(this, &Scenario6::WriteFileButton_Click);
}

void Scenario6::SaveFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Clear previous returned file name, if it exists, between iterations of this scenario
    OutputFileName->Text = "";
    OutputFileContent->Text = "";

    FileSavePicker^ savePicker = ref new FileSavePicker();

    auto plainTextExtensions = ref new Platform::Collections::Vector<String^>();
    plainTextExtensions->Append(".txt");
    savePicker->FileTypeChoices->Insert("Plain Text", plainTextExtensions);
    savePicker->SuggestedFileName = "New Document";

    create_task(savePicker->PickSaveFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            fileToken = Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->Add(file);
            MainPage::Current->NotifyUser(L"Received file: " + file->Name, NotifyType::StatusMessage);
            WriteFileButton->IsEnabled = true;
        }
        else
        {
            MainPage::Current->NotifyUser("Operation cancelled.", NotifyType::StatusMessage);
        }
    });
}

void Scenario6::WriteFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (fileToken != nullptr)
    {
        create_task(Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->GetFileAsync(fileToken)).then([this](StorageFile^ file)
        {
            // Prevent updates to the remote version of the file until we finish making changes and call CompleteUpdatesAsync.
            CachedFileManager::DeferUpdates(file);
            // write to file
            Windows::Globalization::Calendar^ Calendar = ref new Windows::Globalization::Calendar;
            create_task(FileIO::AppendTextAsync(file, L"\nText Added @ " + DateTimeFormatter::LongTime->Format(Calendar->GetDateTime()))).then([this, file]()
            {
                // Let Windows know that we're finished changing the file so the other app can update the remote version of the file.
                // Completing updates may require Windows to ask for user input.
                create_task(CachedFileManager::CompleteUpdatesAsync(file)).then([this, file](FileUpdateStatus status)
                {
                    switch (status)
                    {
                    case FileUpdateStatus::Complete:
                        MainPage::Current->NotifyUser("File " + file->Name + " was saved.", NotifyType::StatusMessage);
                        OutputFileAsync(file);
                        break;

                    case FileUpdateStatus::CompleteAndRenamed:
                        MainPage::Current->NotifyUser("File " + file->Name + " was renamed and saved.", NotifyType::StatusMessage);
                        Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->AddOrReplace(fileToken, file);
                        OutputFileAsync(file);
                        break;

                    default:
                        MainPage::Current->NotifyUser("File " + file->Name + " couldn't be saved.", NotifyType::StatusMessage);
                        break;
                    }
                });
            });
        });
    }
}

void Scenario6::OutputFileAsync(StorageFile^ file)
{
    create_task(FileIO::ReadTextAsync(file)).then([this, file](String^ fileContent)
    {
        OutputFileName->Text = L"Received file: " + file->Name;
        OutputFileContent->Text = L"File content:\n" + fileContent;
    });
}
