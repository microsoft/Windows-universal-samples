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
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2::Scenario2()
    {
        InitializeComponent();
    }

    void Scenario2::OnNavigatedTo(NavigationEventArgs const&)
    {
        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());
    }

    fire_and_forget Scenario2::GetThumbnailButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());

        // Pick a music file
        FileOpenPicker openPicker;
        openPicker.FileTypeFilter().ReplaceAll(FileExtensions::Music());
        StorageFile file = co_await openPicker.PickSingleFileAsync();

        if (file != nullptr)
        {
            const ThumbnailMode thumbnailMode = ThumbnailMode::MusicView;
            const uint32_t size = 100;
            StorageItemThumbnail thumbnail = co_await file.GetThumbnailAsync(thumbnailMode, size);

            // Also verify the type is ThumbnailType::Image (album art) instead of ThumbnailType::Icon
            // (which may be returned as a fallback if the file does not provide album art)
            if (thumbnail != nullptr && thumbnail.Type() == ThumbnailType::Image)
            {
                // Display the thumbnail
                Helpers::DisplayResult(ThumbnailImage(), OutputTextBlock(), thumbnailMode, size, file, thumbnail, false);
            }
            else
            {
                rootPage.NotifyUser(Errors::NoAlbumArt, NotifyType::StatusMessage);
            }
            if (thumbnail != nullptr)
            {
                thumbnail.Close();
            }
        }
        else
        {
            rootPage.NotifyUser(Errors::Cancel, NotifyType::StatusMessage);
        }
    }
}
