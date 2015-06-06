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
// Scenario4.xaml.cpp
// Implementation of the Scenario4 class
//

#include "pch.h"
#include "Scenario4.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario4::Scenario4()
{
    InitializeComponent();
    rootPage = MainPage::Current;
    GetThumbnailButton->Click += ref new RoutedEventHandler(this, &Scenario4::GetThumbnailButton_Click);
}

void Scenario4::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ResetOutput(ThumbnailImage, OutputTextBlock, nullptr);
}

void Scenario4::GetThumbnailButton_Click(Object^ sender, RoutedEventArgs^ e)
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
            const ThumbnailMode thumbnailMode = ThumbnailMode::PicturesView;
            const size_t size = 200;
            create_task(folder->GetThumbnailAsync(thumbnailMode, size)).then([this, folder, size](StorageItemThumbnail^ thumbnail)
            {
                if (thumbnail != nullptr)
                {
                    MainPage::DisplayResult(ThumbnailImage, OutputTextBlock, "PicturesView", size, folder, thumbnail, false);
                }
                else
                {
                    rootPage->NotifyUser(Errors::NoImages, NotifyType::StatusMessage);
                }
            });
        }
        else
        {
            rootPage->NotifyUser(Errors::Cancel, NotifyType::StatusMessage);
        }
    });
}
