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
#include "Scenario2.h"
#include "Scenario2.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::SDKTemplate::implementation
{
    Scenario2::Scenario2()
    {
        InitializeComponent();
    }

    /// <summary>
    /// list all the files and folders in Pictures library by month
    /// </summary>
    fire_and_forget Scenario2::GroupByMonthButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        co_await GroupByHelperAsync(QueryOptions(CommonFolderQuery::GroupByMonth));
    }

    /// <summary>
    /// list all the files and folders in Pictures library by rating
    /// </summary>
    fire_and_forget Scenario2::GroupByRatingButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        co_await GroupByHelperAsync(QueryOptions(CommonFolderQuery::GroupByRating));
    }

    /// <summary>
    /// list all the files and folders in Pictures library by tag
    /// </summary>
    fire_and_forget Scenario2::GroupByTagButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        co_await GroupByHelperAsync(QueryOptions(CommonFolderQuery::GroupByTag));
    }

    /// <summary>
    /// helper for all list by functions
    /// </summary>
    IAsyncAction Scenario2::GroupByHelperAsync(QueryOptions queryOptions)
    {
        auto lifetime = get_strong();

        OutputPanel().Children().Clear();

        StorageFolder picturesFolder = co_await KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary);
        StorageFolderQueryResult queryResult = picturesFolder.CreateFolderQueryWithOptions(queryOptions);

        IVectorView<StorageFolder> folderList = co_await queryResult.GetFoldersAsync();
        for (StorageFolder const& folder : folderList)
        {
            IVectorView<StorageFile> fileList = co_await folder.GetFilesAsync();
            OutputPanel().Children().Append(Helpers::CreateHeaderTextBlock(folder.Name() + L" (" + to_hstring(fileList.Size()) + L")"));
        }
    }
}
