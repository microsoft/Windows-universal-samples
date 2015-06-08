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
// S1_AddWithAPI.xaml.cpp
// Implementation of the S1_AddWithAPI class
//

#include "pch.h"
#include "S1_AddWithAPI.xaml.h"
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

S1_AddWithAPI::S1_AddWithAPI()
{
    InitializeComponent();
    InitializeRevisionNumber();
}

void S1_AddWithAPI::AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (ItemKeyInput->Text == L"")
    {
        MainPage::Current->NotifyUser(L"You must add an item key to insert an item into the index.", NotifyType::ErrorMessage);
    }
    else
    {
        // Retrieve an instance of the ContentIndexer
        auto indexer = Windows::Storage::Search::ContentIndexer::GetIndexer();

        // Create the property map
        struct PropertySetting
        {
            String^ strProperty;
            TextBox^ inputTextBox;
        } propertySettings [] =
        {
            { Windows::Storage::SystemProperties::ItemNameDisplay, NameInput },
            { Windows::Storage::SystemProperties::Keywords, KeywordsInput },
            { Windows::Storage::SystemProperties::Comment, CommentInput }
        };
        auto propertyKeys = ref new Vector<String^>();
        auto content = ref new Windows::Storage::Search::IndexableContent();
        content->Id = ItemKeyInput->Text;
        for (const auto& prop : propertySettings)
        {
            propertyKeys->Append(prop.strProperty);
            Object^ propertyValue = prop.inputTextBox->Text;
            if (LanguageInput->Text != L"")
            {
                auto propertyValueAndLanguage = ref new Windows::Storage::Search::ValueAndLanguage();
                propertyValueAndLanguage->Language = LanguageInput->Text;
                propertyValueAndLanguage->Value = propertyValue;
                propertyValue = propertyValueAndLanguage;
            }
            content->Properties->Insert(prop.strProperty, propertyValue);
        }

        // Create the content stream
        auto contentStream = ref new Windows::Storage::Streams::InMemoryRandomAccessStream();
        auto contentWriter = ref new Windows::Storage::Streams::DataWriter(contentStream);
        contentWriter->WriteString(ContentInput->Text);

        create_task(contentWriter->StoreAsync()).then([indexer, contentStream, contentWriter, content] (unsigned int bytesStored)
        {
            contentWriter->DetachStream();
            // Write the content stream and properties to the indexer
            contentStream->Seek(0);
            content->Stream = contentStream;
            content->StreamContentType = L"text/plain";
            OnIndexerOperationBegin();
            return indexer->AddAsync(content);
        }).then([indexer, this, propertyKeys]()
        {
            OnIndexerOperationComplete(indexer);
            // Retrieve the properties from the indexer
            return indexer->RetrievePropertiesAsync(ItemKeyInput->Text, propertyKeys);
        }).then([propertyKeys, this](IMapView<String^, Object^>^ retrievedProperties)
        {
            MainPage::Current->NotifyUser(CreateItemString(ItemKeyInput->Text, propertyKeys, retrievedProperties), NotifyType::StatusMessage);
        });
    }
}
