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
#include "Scenario7_TrackAFileOrFolderSoThatYouCanAccessItLater.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::UI::Xaml;

Scenario7::Scenario7() : rootPage(MainPage::Current)
{
    InitializeComponent();
    rootPage->Initialize();
    rootPage->ValidateFile();
    AddToListButton->Click += ref new RoutedEventHandler(this, &Scenario7::AddToListButton_Click);
    ShowListButton->Click += ref new RoutedEventHandler(this, &Scenario7::ShowListButton_Click);
    OpenFromListButton->Click += ref new RoutedEventHandler(this, &Scenario7::OpenFromListButton_Click);
}

void Scenario7::AddToListButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        if (MRURadioButton->IsChecked->Value)
        {
            // Add the file to app MRU and possibly system MRU
            RecentStorageItemVisibility visibility = SystemMRUCheckBox->IsChecked->Value ? RecentStorageItemVisibility::AppAndSystem : RecentStorageItemVisibility::AppOnly;
            rootPage->MruToken = StorageApplicationPermissions::MostRecentlyUsedList->Add(file, file->Name, visibility);
            rootPage->NotifyUser("The file '" + file->Name + "' was added to the MRU list and a token was stored.", NotifyType::StatusMessage);
        }
        else if (FALRadioButton->IsChecked->Value)
        {
            // Add the file to the MRU
            rootPage->FalToken = StorageApplicationPermissions::FutureAccessList->Add(file, file->Name);
            rootPage->NotifyUser("The file '" + file->Name + "' was added to the FAL list and a token was stored.", NotifyType::StatusMessage);
        }
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}

void Scenario7::ShowListButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        String^ outputText;
        NotifyType statusOrError = NotifyType::StatusMessage;
        if (MRURadioButton->IsChecked->Value)
        {
            AccessListEntryView^ entries = StorageApplicationPermissions::MostRecentlyUsedList->Entries;
            if (entries->Size > 0)
            {
                outputText = "The MRU list contains the following item(s):";
                std::for_each(begin(entries), end(entries), [this, &outputText](const AccessListEntry& entry)
                {
                    outputText += "\n" + entry.Metadata; // Application previously chose to store sampleFile->Name in this field
                });
            }
            else
            {
                outputText = "The MRU list is empty, please select 'Most Recently Used' list and click 'Add to List' to add a file to the MRU list.";
                statusOrError = NotifyType::ErrorMessage;
            }
        }
        else if (FALRadioButton->IsChecked->Value)
        {
            AccessListEntryView^ entries = StorageApplicationPermissions::FutureAccessList->Entries;
            if (entries->Size > 0)
            {
                outputText = "The FAL list contains the following item(s):";
                std::for_each(begin(entries), end(entries), [this, &outputText](const AccessListEntry& entry)
                {
                    outputText += "\n" + entry.Metadata; // Application previously chose to store sampleFile->Name in this field
                });
            }
            else
            {
                outputText = "The FAL list is empty, please select 'Future Access List' list and click 'Add to List' to add a file to the FAL list.";
                statusOrError = NotifyType::ErrorMessage;
            }
        }
        rootPage->NotifyUser(outputText, statusOrError);
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}

void Scenario7::OpenFromListButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        if (MRURadioButton->IsChecked->Value)
        {
            if (rootPage->MruToken != nullptr)
            {
                // Open the file via the token that was stored when adding this file into the MRU list
                create_task(StorageApplicationPermissions::MostRecentlyUsedList->GetFileAsync(rootPage->MruToken)).then([this](task<StorageFile^> task)
                {
                    try
                    {
                        StorageFile^ file = task.get();
                        // Read the file
                        create_task(FileIO::ReadTextAsync(file)).then([this, file](String^ fileContent)
                        {
                            rootPage->NotifyUser("The file '" + file->Name + "' was opened by a stored token from the MRU list, it contains the following text:\n" + fileContent, NotifyType::StatusMessage);
                        });
                    }
                    catch (COMException^ ex)
                    {
                        rootPage->HandleFileNotFoundException(ex);
                    }
                });
            }
            else
            {
                rootPage->NotifyUser("The MRU list is empty, please select 'Most Recently Used' list and click 'Add to List' to add a file to the MRU list.", NotifyType::ErrorMessage);
            }
        }
        else if (FALRadioButton->IsChecked->Value)
        {
            if (rootPage->FalToken != nullptr)
            {
                // Open the file via the token that was stored when adding this file into the FAL list
                create_task(StorageApplicationPermissions::FutureAccessList->GetFileAsync(rootPage->FalToken)).then([this](task<StorageFile^> task)
                {
                    try
                    {
                        StorageFile^ file = task.get();
                        // Read the file
                        create_task(FileIO::ReadTextAsync(file)).then([this, file](String^ fileContent)
                        {
                            rootPage->NotifyUser("The file '" + file->Name + "' was opened by a stored token from the FAL list, it contains the following text:\n" + fileContent, NotifyType::StatusMessage);
                        });
                    }
                    catch (COMException^ ex)
                    {
                        rootPage->HandleFileNotFoundException(ex);
                    }
                });
            }
            else
            {
                rootPage->NotifyUser("The FAL list is empty, please select 'Future Access List' list and click 'Add to List' to add a file to the FAL list.", NotifyType::ErrorMessage);
            }
        }
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
