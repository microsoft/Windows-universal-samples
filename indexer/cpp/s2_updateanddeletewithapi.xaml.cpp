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
// S2_UpdateAndDeleteWithAPI.xaml.cpp
// Implementation of the S2_UpdateAndDeleteWithAPI class
//

#include "pch.h"
#include "S2_UpdateAndDeleteWithAPI.xaml.h"
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

S2_UpdateAndDeleteWithAPI::S2_UpdateAndDeleteWithAPI()
{
    InitializeComponent();
    InitializeRevisionNumber();
}

void Indexer::S2_UpdateAndDeleteWithAPI::AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto indexer = Windows::Storage::Search::ContentIndexer::GetIndexer();
    AddItemsToIndex(indexer);
}

void Indexer::S2_UpdateAndDeleteWithAPI::UpdatePropertyHelper(String^ itemKey, String^ propertyKey, String^ propertyName, Object^ newValue)
{
    auto indexer = Windows::Storage::Search::ContentIndexer::GetIndexer();
    auto propertyKeys = ref new Vector<String^>();
    propertyKeys->Append(propertyKey);
    auto outputStrings = ref new Vector<String^>();
    create_task(indexer->RetrievePropertiesAsync(itemKey, propertyKeys)).then(
    [outputStrings, indexer, this, itemKey, propertyKey, newValue, propertyName](IMapView<String^, Object^>^ retrievedProperties) mutable
    {
        // Log the original value of the property
        outputStrings->Append(L"Original " + propertyName + L": " + StringifyProperty(retrievedProperties->Lookup(propertyKey)));

        // Create an indexable content item to update the property
        auto content = ref new Windows::Storage::Search::IndexableContent();
        content->Id = itemKey;
        content->Properties->Insert(propertyKey, newValue);

        // Update the indexer
        OnIndexerOperationBegin();
        return indexer->UpdateAsync(content);
    }).then([indexer, itemKey, propertyKeys]()
    {
        OnIndexerOperationComplete(indexer);
        return indexer->RetrievePropertiesAsync(itemKey, propertyKeys);
    }).then([outputStrings, propertyKey, propertyName](IMapView<String^, Object^>^ retrievedProperties) mutable
    {
        // Log the new value of the property
        outputStrings->Append(L"\nNew " + propertyName + L": " + StringifyProperty(retrievedProperties->Lookup(propertyKey)));
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
        catch (Platform::InvalidArgumentException^ e)
        {
            MainPage::Current->NotifyUser(L"Exception thrown! Did you try to retrieve or update a property on a non-existent item?" +
                                          L"\nMessage: " + e->Message +
                                          L"\nHRESULT: " + HResultToString(e->HResult),
                                          NotifyType::ErrorMessage);
        }
    });
}

// This function updates the ItemNameDisplay property for the indexed item with the key "Sample 0." The
// updated ItemNameDisplay property comes from the itemNameInput text box.
void Indexer::S2_UpdateAndDeleteWithAPI::UpdateName_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    UpdatePropertyHelper(L"SampleKey0", Windows::Storage::SystemProperties::ItemNameDisplay, L"item name", ItemNameInput->Text);
}

// This function deletes the Keywords property for the indexed item with the key "Sample0."
void Indexer::S2_UpdateAndDeleteWithAPI::DeleteKeywords_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    UpdatePropertyHelper(L"SampleKey0", Windows::Storage::SystemProperties::Keywords, L"keywords", nullptr);
}

void Indexer::S2_UpdateAndDeleteWithAPI::DeleteItemsHelper(std::function<IAsyncAction^(Windows::Storage::Search::ContentIndexer^)> deleteFunction)
{
    auto indexer = Windows::Storage::Search::ContentIndexer::GetIndexer();
    auto propertiesToRetrieve = ref new Vector<String^>();
    propertiesToRetrieve->Append(Windows::Storage::SystemProperties::ItemNameDisplay);
    auto query = indexer->CreateQuery(L"*", propertiesToRetrieve);
    auto outputStrings = ref new Vector<String^>();
    create_task(query->GetCountAsync()).then([outputStrings, indexer, deleteFunction](unsigned int count)
    {
        outputStrings->Append(L"Original number of items: " + count);
        OnIndexerOperationBegin();
        return deleteFunction(indexer);
    }).then([propertiesToRetrieve, indexer]()
    {
        OnIndexerOperationComplete(indexer);
        auto query = indexer->CreateQuery(L"*", propertiesToRetrieve);
        return query->GetCountAsync();
    }).then([outputStrings](unsigned int count)
    {
        outputStrings->Append(L"\nNew number of items: " + count);
        auto outputString = ref new String(L"");
        for (const auto& str : outputStrings)
        {
            outputString += str;
        }
        MainPage::Current->NotifyUser(outputString, NotifyType::StatusMessage);
    });
}

// This function deletes the indexed item with the key "SampleKey0" added to the index by this app via the ContentIndexer
void Indexer::S2_UpdateAndDeleteWithAPI::DeleteSingleItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    return DeleteItemsHelper(std::function<IAsyncAction^(Windows::Storage::Search::ContentIndexer^)>(
        [](Windows::Storage::Search::ContentIndexer^ indexer) -> IAsyncAction^
        {
            return indexer->DeleteAsync(L"SampleKey0");
        }));
}

// This function deletes the indexed items with the keys "SampleKey1" and "SampleKey2" added to the index by this app via the ContentIndexer
void Indexer::S2_UpdateAndDeleteWithAPI::DeleteMultipleItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    return DeleteItemsHelper(std::function<IAsyncAction^(Windows::Storage::Search::ContentIndexer^)>(
        [](Windows::Storage::Search::ContentIndexer^ indexer) -> IAsyncAction^
        {
            auto documentKeys = ref new Vector<String^>();
            documentKeys->Append(L"SampleKey1");
            documentKeys->Append(L"SampleKey2");
            return indexer->DeleteMultipleAsync(documentKeys);
        }));
}

// This function deletes all items added to the index by this app via the ContentIndexer
void Indexer::S2_UpdateAndDeleteWithAPI::DeleteAllItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    return DeleteItemsHelper(std::function<IAsyncAction^(Windows::Storage::Search::ContentIndexer^)>(
        [](Windows::Storage::Search::ContentIndexer^ indexer) -> IAsyncAction^
        {
            return indexer->DeleteAllAsync();
        }));
}
