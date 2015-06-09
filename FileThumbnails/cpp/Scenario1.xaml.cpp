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
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario1::Scenario1()
{
    InitializeComponent();
    rootPage = MainPage::Current;
    GetThumbnailButton->Click += ref new RoutedEventHandler(this, &Scenario1::GetThumbnailButton_Click);
}

void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ResetOutput(ThumbnailImage, OutputTextBlock, nullptr);
}

void Scenario1::GetThumbnailButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->ResetOutput(ThumbnailImage, OutputTextBlock, nullptr);

    // Pick a photo
    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    Array<String^>^ extensions = FileExtensions::Image;
    for (unsigned int i = 0; i < extensions->Length; i++)
    {
        openPicker->FileTypeFilter->Append(extensions[i]);
    }

    String^ thumbnailModeName = ((ComboBoxItem^)ModeComboBox->SelectedItem)->Name;
    ThumbnailMode thumbnailMode = ThumbnailMode::ListView;
    if (thumbnailModeName == "ListView")
    {
        thumbnailMode = ThumbnailMode::ListView;
    }
    if (thumbnailModeName == "PicturesView")
    {
        thumbnailMode = ThumbnailMode::PicturesView;
    }
    else if (thumbnailModeName == "SingleItem")
    {
        thumbnailMode = ThumbnailMode::SingleItem;
    }

    bool fastThumbnail = FastThumbnailCheckBox->IsChecked->Value;
    ThumbnailOptions thumbnailOptions = ThumbnailOptions::UseCurrentScale;
    if (fastThumbnail)
    {
        thumbnailOptions = thumbnailOptions | ThumbnailOptions::ReturnOnlyIfCached;
    }

    create_task(openPicker->PickSingleFileAsync()).then([this, thumbnailMode, thumbnailModeName, thumbnailOptions, fastThumbnail](StorageFile^ file)
    {
        if (file != nullptr)
        {
            const size_t size = 200;
            create_task(file->GetThumbnailAsync(thumbnailMode, size, thumbnailOptions)).then([this, file, thumbnailMode, thumbnailModeName, thumbnailOptions, fastThumbnail, size](StorageItemThumbnail^ thumbnail)
            {
                if (thumbnail != nullptr)
                {
                    MainPage::DisplayResult(ThumbnailImage, OutputTextBlock, thumbnailModeName, size, file, thumbnail, false);
                }
                else if (fastThumbnail)
                {
                    rootPage->NotifyUser(Errors::NoExifThumbnail, NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser(Errors::NoThumbnail, NotifyType::StatusMessage);
                }
            });
        }
        else
        {
            rootPage->NotifyUser(Errors::Cancel, NotifyType::StatusMessage);
        }
    });
}
