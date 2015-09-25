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
// Scenario5.xaml.cpp
// Implementation of the Scenario5 class
//

#include "pch.h"
#include "Scenario5.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario5::Scenario5()
{
    InitializeComponent();
    rootPage = MainPage::Current;
    GetThumbnailButton->Click += ref new RoutedEventHandler(this, &Scenario5::GetThumbnailButton_Click);
}

void Scenario5::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ResetOutput(ThumbnailImage, OutputTextBlock, nullptr);
}

void Scenario5::GetThumbnailButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->ResetOutput(ThumbnailImage, OutputTextBlock, nullptr);

    // Pick a folder
    FolderPicker^ folderPicker = ref new FolderPicker();
    Array<String^>^ extensions = FileExtensions::Image;
    for (unsigned int i = 0; i < extensions->Length; i++)
    {
        folderPicker->FileTypeFilter->Append(extensions[i]);
    }

    create_task(folderPicker->PickSingleFolderAsync()).then([this](StorageFolder^ folder)
    {
        if (folder != nullptr)
        {
            const CommonFolderQuery monthShape = CommonFolderQuery::GroupByMonth;
            // Verify queries are supported because they are not supported in all picked locations.
            if (folder->IsCommonFolderQuerySupported(monthShape))
            {
                create_task(folder->CreateFolderQuery(monthShape)->GetFoldersAsync()).then([this, folder](IVectorView<StorageFolder^>^ months)
                {
                    if (months != nullptr && months->Size > 0)
                    {
                        StorageFolder^ firstMonth = months->GetAt(0);
                        const ThumbnailMode thumbnailMode = ThumbnailMode::PicturesView;
                        const size_t size = 200;
                        create_task(firstMonth->GetThumbnailAsync(thumbnailMode, size)).then([this, folder, firstMonth, size](StorageItemThumbnail^ thumbnail)
                        {
                            if (thumbnail != nullptr)
                            {
                                MainPage::DisplayResult(ThumbnailImage, OutputTextBlock, "PicturesView", size, folder, thumbnail, true);

                                // Also display the list of files in the file group
                                create_task(firstMonth->GetFilesAsync()).then([this](IVectorView<StorageFile^>^ files)
                                {
                                    String^ output = "\nList of files in the group:\n\n";
                                    for (StorageFile^ file : files)
                                    {
                                        output += file->Name + "\n";
                                    }
                                    OutputDetails->Text = output;
                                });
                            }
                            else
                            {
                                rootPage->NotifyUser(Errors::NoImages, NotifyType::StatusMessage);
                            }
                        });
                    }
                    else
                    {
                        rootPage->NotifyUser(Errors::FileGroupEmpty, NotifyType::StatusMessage);
                    }
                });
            }
            else
            {
                rootPage->NotifyUser(Errors::FileGroupLocation, NotifyType::StatusMessage);
            }
        }
        else
        {
            rootPage->NotifyUser(Errors::Cancel, NotifyType::StatusMessage);
        }
    });
}
