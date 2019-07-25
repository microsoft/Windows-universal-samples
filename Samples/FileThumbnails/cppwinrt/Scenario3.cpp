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
#include "Scenario3.h"
#include "Scenario3.g.cpp"
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
    Scenario3::Scenario3()
    {
        InitializeComponent();
    }

    void Scenario3::OnNavigatedTo(NavigationEventArgs const&)
    {
        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());
    }

    fire_and_forget Scenario3::GetThumbnailButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());

        // Pick a document
        FileOpenPicker openPicker;
        openPicker.FileTypeFilter().ReplaceAll(FileExtensions::Document());
        StorageFile file = co_await openPicker.PickSingleFileAsync();

        if (file != nullptr)
        {
            const ThumbnailMode thumbnailMode = ThumbnailMode::DocumentsView;
            const uint32_t size = 100;
            StorageItemThumbnail thumbnail = co_await file.GetThumbnailAsync(thumbnailMode, size);
            if (thumbnail != nullptr)
            {
                Helpers::DisplayResult(ThumbnailImage(), OutputTextBlock(), thumbnailMode, size, file, thumbnail, false);
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
