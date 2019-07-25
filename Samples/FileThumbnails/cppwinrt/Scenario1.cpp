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
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1::Scenario1()
    {
        InitializeComponent();
    }

    void Scenario1::OnNavigatedTo(NavigationEventArgs const&)
    {
        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());
    }

    fire_and_forget Scenario1::GetThumbnailButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());

        // Pick a photo
        FileOpenPicker openPicker;
        openPicker.FileTypeFilter().ReplaceAll(FileExtensions::Image());
        StorageFile file = co_await openPicker.PickSingleFileAsync();
        if (file != nullptr)
        {
            hstring thumbnailModeName = ModeComboBox().SelectedItem().as<ComboBoxItem>().Name();
            ThumbnailMode thumbnailMode = ThumbnailMode::ListView;
            if (thumbnailModeName == L"ListView")
            {
                thumbnailMode = ThumbnailMode::ListView;
            }
            if (thumbnailModeName == L"PicturesView")
            {
                thumbnailMode = ThumbnailMode::PicturesView;
            }
            else if (thumbnailModeName == L"SingleItem")
            {
                thumbnailMode = ThumbnailMode::SingleItem;
            }

            bool fastThumbnail = FastThumbnailCheckBox().IsChecked().Value();
            ThumbnailOptions thumbnailOptions = ThumbnailOptions::UseCurrentScale;
            if (fastThumbnail)
            {
                thumbnailOptions |= ThumbnailOptions::ReturnOnlyIfCached;
            }

            const uint32_t size = 200;
            StorageItemThumbnail thumbnail = co_await file.GetThumbnailAsync(thumbnailMode, size, thumbnailOptions);
            if (thumbnail != nullptr)
            {
                Helpers::DisplayResult(ThumbnailImage(), OutputTextBlock(), thumbnailMode, size, file, thumbnail, false);
                thumbnail.Close();
            }
            else if (fastThumbnail)
            {
                rootPage.NotifyUser(Errors::NoExifThumbnail, NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(Errors::NoThumbnail, NotifyType::StatusMessage);
            }
        }
        else
        {
            rootPage.NotifyUser(Errors::Cancel, NotifyType::StatusMessage);
        }
    }

}
