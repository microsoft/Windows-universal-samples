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
// S6_DeleteWithAppContent.xaml.cpp
// Implementation of the S6_DeleteWithAppContent class
//

#include "pch.h"
#include "S6_DeleteWithAppContent.xaml.h"
#include "MainPage.xaml.h"
#include "Helpers.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

S6_DeleteWithAppContent::S6_DeleteWithAppContent()
{
    InitializeComponent();
    InitializeRevisionNumber();
}

void S6_DeleteWithAppContent::AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    return AddAppContentFilesToIndexedFolder();
}

// This function deletes a single appcontent-ms file, "sample1.appcontent-ms". Upon deletion, the indexer is notified
// and the content indexed from this file is removed from the index.
void S6_DeleteWithAppContent::DeleteSingleItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
    auto outputStrings = ref new Vector<String^>();
    outputStrings->Append(L"Items deleted from the \"Indexed\" folder:");
    create_task(localFolder->CreateFolderAsync(L"Indexed", Windows::Storage::CreationCollisionOption::OpenIfExists)).then(
    [](Windows::Storage::StorageFolder^ indexedFolder)
    {
        return indexedFolder->GetFileAsync(L"sample1.appcontent-ms");
    }).then([outputStrings](Windows::Storage::StorageFile^ file)
    {
        outputStrings->Append(L"\n" + file->DisplayName);
        return file->DeleteAsync();
    }).then([outputStrings]()
    {
        auto outputString = ref new String(L"");
        for (const auto& str : outputStrings)
        {
            outputString += str;
        }
        return outputString;
    }).then([](task<String^> outputStringTask)
    {
        try
        {
            MainPage::Current->NotifyUser(outputStringTask.get(), NotifyType::StatusMessage);
        }
        catch (Platform::Exception^ e)
        {
            MainPage::Current->NotifyUser(L"Exception thrown! Did you already delete the \"sample1.appcontent-ms\" file?" +
                                          L"\nMessage: " + e->Message +
                                          L"\nHRESULT: " + HResultToString(e->HResult),
                                          NotifyType::ErrorMessage);
        }
    });
}

// This function deletes all appcontent-ms files in the "LocalState\Indexed" folder. Upon deletion, the indexer is notified and
// the content indexed from these files is removed from the index.
void S6_DeleteWithAppContent::DeleteAllItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
    auto outputStrings = ref new Vector<String^>();
    outputStrings->Append(L"Items deleted from the \"Indexed\" folder:");
    create_task(localFolder->CreateFolderAsync(L"Indexed", Windows::Storage::CreationCollisionOption::OpenIfExists)).then(
    [](Windows::Storage::StorageFolder^ indexedFolder)
    {
        return indexedFolder->GetFilesAsync();
    }).then([outputStrings](IVectorView<Windows::Storage::StorageFile^>^ files)
    {
        std::vector<task<void>> deleteTasks;
        for (const auto& file : files)
        {
            deleteTasks.push_back(create_task(file->DeleteAsync()));
            outputStrings->Append(L"\n" + file->DisplayName);
        }
        return when_all(deleteTasks.begin(), deleteTasks.end());
    }).then([outputStrings]()
    {
        auto outputString = ref new String(L"");
        for (const auto& str : outputStrings)
        {
            outputString += str;
        }
        MainPage::Current->NotifyUser(outputString, NotifyType::StatusMessage);
    });
}
