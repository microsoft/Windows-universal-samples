//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario3.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include <ppltasks.h>
#include "GetMatchingPropertiesWithRanges.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;

Scenario3::Scenario3()
{
    InitializeComponent();
    FindQueryText->Text = "a AND datemodified:>2/1/2013";
}

void Scenario3::Find_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ContentTextOutput->Text = "";
    if (FindQueryText->Text == "")
    {
        return;
    }

    auto propertyNames = ref new Platform::Collections::Vector<String^>();
    propertyNames->Append("System.FileName");
    auto queryOptions = ref new QueryOptions();
    queryOptions->IndexerOption = IndexerOption::OnlyUseIndexer;
    queryOptions->UserSearchFilter = FindQueryText->Text;
    queryOptions->SetPropertyPrefetch(Windows::Storage::FileProperties::PropertyPrefetchOptions::DocumentProperties, propertyNames);

    // Query the Pictures library.
    StorageFileQueryResult^ queryResult = KnownFolders::PicturesLibrary->CreateFileQueryWithOptions(queryOptions);
    create_task(queryResult->GetFilesAsync()).then([this, queryOptions, queryResult](IVectorView<StorageFile^>^ files)
    {
        std::for_each(begin(files), end(files), [this, queryResult](StorageFile^ file)
        {
            IMap<String^, IVectorView<Windows::Data::Text::TextSegment>^>^ fileRangeProperties = queryResult->GetMatchingPropertiesWithRanges(file);
            if (fileRangeProperties->HasKey("System.FileName"))
            {
                IVectorView<Windows::Data::Text::TextSegment>^ ranges = fileRangeProperties->Lookup("System.FileName");
                HighlightRanges(ContentTextOutput, file->DisplayName, ranges);
            }
            // Note: You can continue looking for other properties you would like to highlight on the file here.
        });
        if (files->Size == 0)
        {
            ContentTextOutput->Text = "There were no matching files in your Pictures Library";
        }
    });
}

