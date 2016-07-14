//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario2.xaml.cpp
// Implementation of the Scenario2 class
//

#include "pch.h"
#include "Scenario2.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2::Scenario2()
{
    InitializeComponent();
}

void Scenario2::GroupByMonthButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    GroupByHelper(ref new QueryOptions(CommonFolderQuery::GroupByMonth));
}

void Scenario2::GroupByRatingButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    GroupByHelper(ref new QueryOptions(CommonFolderQuery::GroupByRating));
}

void Scenario2::GroupByTagButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    GroupByHelper(ref new QueryOptions(CommonFolderQuery::GroupByTag));
}

void Scenario2::GroupByHelper(QueryOptions^ queryOptions)
{
    OutputPanel->Children->Clear();

    create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary)).then([this, queryOptions](StorageFolder^ picturesFolder)
    {
        StorageFolderQueryResult^ queryResult = picturesFolder->CreateFolderQueryWithOptions(queryOptions);
        return queryResult->GetFoldersAsync();
    }).then([this](IVectorView<StorageFolder^>^ folders)
    {
        // Add all file retrieval tasks to an array of tasks
        std::vector<task<std::pair<StorageFolder^, IVectorView<StorageFile^>^>>> fileTasks;
        std::for_each(begin(folders), end(folders), [&fileTasks](StorageFolder^ folder)
        {
            auto getFilesTask = create_task(folder->GetFilesAsync());
            fileTasks.push_back(getFilesTask.then([folder](IVectorView<StorageFile^>^ files)
            {
                // put the results in std::pair to create the association of folder and its files.
                return std::pair<StorageFolder^, IVectorView<StorageFile^>^>(folder, files);
            }));
        });

        // Aggregate the results of multiple asynchronous operations
        // so that they are returned after all are completed. This
        // ensures that all groups are displayed in order.
        when_all(fileTasks.begin(), fileTasks.end()).then([this, folders](std::vector<std::pair<StorageFolder^, IVectorView<StorageFile^>^>> folderContents)
        {
            std::for_each(begin(folderContents), end(folderContents), [this](std::pair<StorageFolder^, IVectorView<StorageFile^>^> folderContent)
            {
                auto folder = folderContent.first;
                auto files = folderContent.second;

                OutputPanel->Children->Append(CreateHeaderTextBlock(folder->DisplayName + " (" + files->Size.ToString() + ")"));
                std::for_each(begin(files), end(files), [this](StorageFile^ file)
                {
                    OutputPanel->Children->Append(CreateLineItemTextBlock(file->Name));
                });
            });
        });
    });
}

TextBlock^ Scenario2::CreateHeaderTextBlock(String^ contents)
{
    TextBlock^ textBlock = ref new TextBlock();
    textBlock->Text = contents;
    textBlock->Style = static_cast<Xaml::Style^>(::Application::Current->Resources->Lookup("SampleHeaderTextStyle"));
    textBlock->TextWrapping = TextWrapping::Wrap;
    return textBlock;
}

TextBlock^ Scenario2::CreateLineItemTextBlock(String^ contents)
{
    TextBlock^ textBlock = ref new TextBlock();
    textBlock->Text = contents;
    textBlock->Style = static_cast<Xaml::Style^>(::Application::Current->Resources->Lookup("BasicTextStyle"));
    textBlock->TextWrapping = TextWrapping::Wrap;
    Thickness margin = textBlock->Margin;
    margin.Left = 20;
    textBlock->Margin = margin;
    return textBlock;
}
