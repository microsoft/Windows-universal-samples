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
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario1::Scenario1()
{
    InitializeComponent();
    SearchButton->Click += ref new RoutedEventHandler(this, &Scenario1::SearchButton_Click);
}

void Scenario1::SearchButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto fileTypeFilter = ref new Platform::Collections::Vector<Platform::String^>();
    fileTypeFilter->Append("*");
    auto queryOptions = ref new QueryOptions(CommonFileQuery::OrderBySearchRank, fileTypeFilter);
    //use the user's input to make a query
    queryOptions->UserSearchFilter = InputTextBox->Text;

    create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::MusicLibrary)).then([this, queryOptions](StorageFolder^ musicFolder)
    {
        auto queryResult = musicFolder->CreateFileQueryWithOptions(queryOptions);
        //find all files that match the query
        return queryResult->GetFilesAsync();
    }).then([this, queryOptions](IVectorView<StorageFile^>^ files)
    {
        String^ outputText = "";

        //output how many files that match the query were found
        if (files->Size == 0)
        {
            outputText += "No files found for '" + queryOptions->UserSearchFilter + "'";
        }
        else if (files->Size == 1)
        {
            outputText += files->Size.ToString() + " file found:\n\n";
        }
        else
        {
            outputText += files->Size.ToString() + " files found:\n\n";
        }

        //output the name of each file that matches the query
        for (unsigned int i = 0; i < files->Size; i++)
        {
            outputText += files->GetAt(i)->Name + "\n";
        }

        OutputTextBlock->Text = outputText;
    });
}
