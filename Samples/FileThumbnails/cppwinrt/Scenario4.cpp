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
#include "Scenario4.h"
#include "Scenario4.g.cpp"
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
    Scenario4::Scenario4()
    {
        InitializeComponent();
    }

    void Scenario4::OnNavigatedTo(NavigationEventArgs const&)
    {
        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());
    }

    fire_and_forget Scenario4::GetThumbnailButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());

        // Pick a folder
        FolderPicker folderPicker;
        folderPicker.FileTypeFilter().ReplaceAll(FileExtensions::Image());
        StorageFolder folder = co_await folderPicker.PickSingleFolderAsync();

        if (folder != nullptr)
        {
            const ThumbnailMode thumbnailMode = ThumbnailMode::PicturesView;
            const uint32_t size = 200;
            StorageItemThumbnail thumbnail = co_await folder.GetThumbnailAsync(thumbnailMode, size);
            if (thumbnail != nullptr)
            {
                Helpers::DisplayResult(ThumbnailImage(), OutputTextBlock(), thumbnailMode, size, folder, thumbnail, false);
                thumbnail.Close();
            }
            else
            {
                rootPage.NotifyUser(Errors::NoIcon, NotifyType::StatusMessage);
            }
        }
        else
        {
            rootPage.NotifyUser(Errors::Cancel, NotifyType::StatusMessage);
        }
    }
}
