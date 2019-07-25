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
#include "Scenario6.h"
#include "Scenario6.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace
{
    // Utility function to convert a string to a uint32_t and detect bad input
    bool TryParseUInt(const wchar_t* str, uint32_t& result)
    {
        wchar_t* end;
        errno = 0;
        result = std::wcstoul(str, &end, 0);

        if (str == end)
        {
            // Not parseable.
            return false;
        }

        if (errno == ERANGE)
        {
            // Out of range.
            return false;
        }

        if (*end != L'\0')
        {
            // Extra unparseable characters at the end.
            return false;
        }

        return true;
    }

}

namespace winrt::SDKTemplate::implementation
{
    Scenario6::Scenario6()
    {
        InitializeComponent();
    }

    void Scenario6::OnNavigatedTo(NavigationEventArgs const&)
    {
        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());
    }

    fire_and_forget Scenario6::SelectImageButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock());

        uint32_t size;
        if (TryParseUInt(RequestSize().Text().c_str(), size))
        {
            // Pick a photo
            FileOpenPicker openPicker;
            openPicker.FileTypeFilter().ReplaceAll(FileExtensions::Image());
            StorageFile file = co_await openPicker.PickSingleFileAsync();
            if (file != nullptr)
            {
                const ThumbnailMode thumbnailMode = ThumbnailMode::SingleItem;

                bool fastThumbnail = FastThumbnailCheckBox().IsChecked().Value();
                ThumbnailOptions thumbnailOptions = ThumbnailOptions::UseCurrentScale;
                if (fastThumbnail)
                {
                    thumbnailOptions |= ThumbnailOptions::ReturnOnlyIfCached;
                }

                StorageItemThumbnail thumbnail = co_await file.GetScaledImageAsThumbnailAsync(thumbnailMode, size, thumbnailOptions);
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
        else
        {
            rootPage.NotifyUser(Errors::InvalidSize, NotifyType::ErrorMessage);
        }
    }
}
