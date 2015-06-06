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
// S3_RetrieveWithAPI.xaml.cpp
// Implementation of the S3_RetrieveWithAPI class
//

#include "pch.h"
#include "S3_RetrieveWithAPI.xaml.h"
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

S3_RetrieveWithAPI::S3_RetrieveWithAPI()
{
    InitializeComponent();
    InitializeRevisionNumber();
}

void S3_RetrieveWithAPI::AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto indexer = Windows::Storage::Search::ContentIndexer::GetIndexer();
    AddItemsToIndex(indexer);
}

void ExecuteQueryHelper(String^ queryString)
{
    auto indexer = Windows::Storage::Search::ContentIndexer::GetIndexer();
    auto propertyKeys = ref new Vector<String^>();
    propertyKeys->Append(Windows::Storage::SystemProperties::ItemNameDisplay);
    propertyKeys->Append(Windows::Storage::SystemProperties::Keywords);
    propertyKeys->Append(Windows::Storage::SystemProperties::Comment);
    auto query = indexer->CreateQuery(queryString, propertyKeys);
    create_task(query->GetAsync()).then([propertyKeys](IVectorView<Windows::Storage::Search::IIndexableContent^>^ documents)
    {
        String^ itemsString;
        for (const auto& document : documents)
        {
            itemsString += CreateItemString(document->Id, propertyKeys, document->Properties->GetView()) + L"\n";
        }
        MainPage::Current->NotifyUser(itemsString, NotifyType::StatusMessage);
    });
}

// This function retrieves all items added to the index by this app via the ContentIndexer.
void S3_RetrieveWithAPI::RetrieveAllItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ExecuteQueryHelper(L"*");
}

// This function retrieves all items added to the index by this app via the ContentIndexer which contain the work "0" in
// either the ItemNameDisplay, Keywords, or Comment properties.
void S3_RetrieveWithAPI::RetrieveMatchingItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ExecuteQueryHelper(L"0");
}
