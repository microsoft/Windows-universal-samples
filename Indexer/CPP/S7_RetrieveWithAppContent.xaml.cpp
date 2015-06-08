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
// S7_RetrieveWithAppContent.xaml.cpp
// Implementation of the S7_RetrieveWithAppContent class
//

#include "pch.h"
#include "S7_RetrieveWithAppContent.xaml.h"
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

S7_RetrieveWithAppContent::S7_RetrieveWithAppContent()
{
    InitializeComponent();
    InitializeRevisionNumber();
}

void S7_RetrieveWithAppContent::AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    return AddAppContentFilesToIndexedFolder();
}

void RetrieveAppContentHelper(std::function<Windows::Storage::Search::StorageFileQueryResult^(Windows::Storage::StorageFolder^)> getQueryFunction)
{
    create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFolderAsync(L"Indexed", Windows::Storage::CreationCollisionOption::OpenIfExists)).then(
    [getQueryFunction](Windows::Storage::StorageFolder^ indexedFolder)
    {
        auto query = getQueryFunction(indexedFolder);
        return query->GetFilesAsync();
    }).then([](IVectorView<Windows::Storage::StorageFile^>^ files)
    {
        auto propertyKeys = ref new Vector<String^>();
        propertyKeys->Append(Windows::Storage::SystemProperties::ItemNameDisplay);
        propertyKeys->Append(Windows::Storage::SystemProperties::Keywords);
        propertyKeys->Append(Windows::Storage::SystemProperties::Comment);
        propertyKeys->Append(Windows::Storage::SystemProperties::Title);
        std::vector<task<IMap<String^, Object^>^>> retrieveTasks;
        for (const auto& file : files)
        {
            retrieveTasks.push_back(create_task(file->Properties->RetrievePropertiesAsync(propertyKeys)));
        }
        return when_all(retrieveTasks.begin(), retrieveTasks.end());
    }).then([](std::vector<IMap<String^, Object^>^> retrievedItems)
    {
        String^ outputString;
        for (const auto& item : retrievedItems)
        {
            outputString += L"Name: " + StringifyProperty(item->Lookup(Windows::Storage::SystemProperties::ItemNameDisplay)) + L"\n";
            outputString += L"Keywords: " + StringifyProperty(item->Lookup(Windows::Storage::SystemProperties::Keywords)) + L"\n";
            outputString += L"Comment: " + StringifyProperty(item->Lookup(Windows::Storage::SystemProperties::Comment)) + L"\n";
            outputString += L"Title: " + StringifyProperty(item->Lookup(Windows::Storage::SystemProperties::Title)) + L"\n\n";
        }
        MainPage::Current->NotifyUser(outputString, NotifyType::StatusMessage);
    });
}

// This function retrieves all items added to the index through the app's appcontent-ms files
void S7_RetrieveWithAppContent::RetrieveAllItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    return RetrieveAppContentHelper([](Windows::Storage::StorageFolder^ indexedFolder)
        {
            // Query for all files in the "LocalState\Indexed" folder and sort the results by name
            return indexedFolder->CreateFileQuery(Windows::Storage::Search::CommonFileQuery::OrderByName);
        });
}

// This function retrieves all items added to the index through the app's appcontent-ms files which
// have an ItemNameDisplay property containing "Sample 1"
void S7_RetrieveWithAppContent::RetrieveMatchedItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    return RetrieveAppContentHelper([](Windows::Storage::StorageFolder^ indexedFolder)
        {
            auto queryOptions = ref new Windows::Storage::Search::QueryOptions();
            // Include only items returned via the index in the search results
            queryOptions->IndexerOption = Windows::Storage::Search::IndexerOption::OnlyUseIndexer;
            // Create an AQS (Advanced Query Syntax) query which will look for ItemNameDisplay properties which contain "Sample 1"
            queryOptions->ApplicationSearchFilter = Windows::Storage::SystemProperties::ItemNameDisplay + L":\"Sample 1\"";
            return indexedFolder->CreateFileQueryWithOptions(queryOptions);
        });
}
