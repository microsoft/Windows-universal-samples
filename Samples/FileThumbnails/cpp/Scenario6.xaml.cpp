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
#include "Scenario6.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario6::Scenario6()
{
    InitializeComponent();
    rootPage = MainPage::Current;
}

void Scenario6::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ResetOutput(ThumbnailImage, OutputTextBlock, nullptr);
}

void Scenario6::SelectImageButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->ResetOutput(ThumbnailImage, OutputTextBlock, nullptr);

    // Pick a photo
    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    for (String^ ext : FileExtensions::Image)
    {
        openPicker->FileTypeFilter->Append(ext);
    }

    ThumbnailMode thumbnailMode = ThumbnailMode::SingleItem;
    String^ thumbnailModeName = thumbnailMode.ToString();

    const unsigned int size = _wtoi(RequestSize->Text->Data());
    if (size > 0)
    {
        bool fastThumbnail = FastThumbnailCheckBox->IsChecked->Value;
        ThumbnailOptions thumbnailOptions = ThumbnailOptions::UseCurrentScale;
        if (fastThumbnail)
        {
            thumbnailOptions = thumbnailOptions | ThumbnailOptions::ReturnOnlyIfCached;
        }

        create_task(openPicker->PickSingleFileAsync()).then([this, thumbnailMode, thumbnailModeName, thumbnailOptions, fastThumbnail, size](StorageFile^ file)
        {
            if (file != nullptr)
            {
                create_task(file->GetScaledImageAsThumbnailAsync(thumbnailMode, size, thumbnailOptions)).then([this, file, thumbnailMode, thumbnailModeName, thumbnailOptions, fastThumbnail, size](StorageItemThumbnail^ thumbnail)
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
    else
    {
        rootPage->NotifyUser(Errors::InvalidSize, NotifyType::ErrorMessage);
    }
}
