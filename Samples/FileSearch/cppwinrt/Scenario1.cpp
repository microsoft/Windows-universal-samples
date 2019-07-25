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
#include "Scenario1.h"
#include "Scenario1.g.cpp"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1::Scenario1()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1::SearchButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFolder musicFolder = co_await KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::MusicLibrary);

        QueryOptions queryOptions(CommonFileQuery::OrderBySearchRank, { L"*" });
        //use the user's input to make a query
        queryOptions.UserSearchFilter(InputTextBox().Text());
        StorageFileQueryResult queryResult = musicFolder.CreateFileQueryWithOptions(queryOptions);

        std::wstringstream output;

        //find all files that match the query
        IVectorView<StorageFile> files = co_await queryResult.GetFilesAsync();
        //output how many files that match the query were found
        if (files.Size() == 0)
        {
            output << L"No files found for '" << std::wstring_view{ queryOptions.UserSearchFilter() } << L"'";
        }
        else if (files.Size() == 1)
        {
            output << files.Size() << L" file found:" << std::endl << std::endl;
        }
        else
        {
            output << files.Size() << L" files found:" << std::endl << std::endl;
        }

        //output the name of each file that matches the query
        for (StorageFile const& file : files)
        {
            output << std::wstring_view{ file.Name() } << std::endl;
        }

        OutputTextBlock().Text(output.str());
    }
}
