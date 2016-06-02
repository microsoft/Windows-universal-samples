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
        else
        {
            // Add the file to the FAL
            try
            {
                rootPage->FalToken = StorageApplicationPermissions::FutureAccessList->Add(file, file->Name);
                rootPage->NotifyUser("The file '" + file->Name + "' was added to the FAL list and a token was stored.", NotifyType::StatusMessage);
            }
            catch (COMException^ ex)
            {
                if (ex->HResult == FA_E_MAX_PERSISTED_ITEMS_REACHED)
                {
                    // A real program would call Remove() to create room in the FAL.
                    rootPage->NotifyUser("The file '" + file->Name + "' was not added to the FAL list because the FAL list is full.", NotifyType::ErrorMessage);
                }
                else
                {
                    throw;
                }
            }
        }
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}

void Scenario7::ShowListButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    AccessListEntryView^ entries = nullptr;
    String^ listName;

    if (MRURadioButton->IsChecked->Value)
    {
        listName = "MRU";
        entries = StorageApplicationPermissions::MostRecentlyUsedList->Entries;
    }
    else
    {
        listName = "FAL";
        entries = StorageApplicationPermissions::FutureAccessList->Entries;
    }

    if (entries->Size > 0)
    {
        String^ outputText = "The " + listName + " + list contains the following item(s):";
        for (const AccessListEntry& entry : entries)
        {
            outputText += "\n" + entry.Metadata; // Application previously chose to store sampleFile->Name in this field
        }
        rootPage->NotifyUser(outputText, NotifyType::StatusMessage);
    }
    else
    {
        rootPage->NotifyUser("The " + listName + " list is empty.", NotifyType::ErrorMessage);
    }
}

void Scenario7::OpenFromListButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    task<StorageFile^> fileTask = task_from_result<StorageFile^>(nullptr);

    if (MRURadioButton->IsChecked->Value)
    {
        if (rootPage->MruToken != nullptr)
        {
            // When the MRU becomes full, older entries are automatically deleted, so check if the
            // token is still valid before using it.
            if (StorageApplicationPermissions::MostRecentlyUsedList->ContainsItem(rootPage->MruToken))
            {
                // Open the file via the token that was stored when adding this file into the MRU list
                fileTask = create_task(StorageApplicationPermissions::MostRecentlyUsedList->GetFileAsync(rootPage->MruToken));
            }
            else
            {
                rootPage->NotifyUser("The token is no longer valid.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            rootPage->NotifyUser("This operation requires a token. Add file to the MRU list first.", NotifyType::ErrorMessage);
        }
    }
    else
    {
        if (rootPage->FalToken != nullptr)
        {
            // Open the file via the token that was stored when adding this file into the FAL list.
            // The token remains valid until we explicitly remove it.
            fileTask = create_task(StorageApplicationPermissions::FutureAccessList->GetFileAsync(rootPage->FalToken));
        }
        else
        {
            rootPage->NotifyUser("This operation requires a token. Add file to the FAL list first.", NotifyType::ErrorMessage);
        }
    }

    fileTask.then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            create_task(FileIO::ReadTextAsync(file)).then([this, file](task<String^> task)
            {
                try
                {
                    String^ fileContent = task.get();
                    rootPage->NotifyUser("The file '" + file->Name + "' was opened by a stored token. It contains the following text:\n" + fileContent, NotifyType::StatusMessage);
                }
                catch (COMException^ ex)
                {
                    // I/O errors are reported as exceptions.
                    rootPage->HandleIoException(ex, "Error reading file opened from list");
                }
            });
        }
    });
}
