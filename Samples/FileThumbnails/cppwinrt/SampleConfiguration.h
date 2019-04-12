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

#pragma once 
#include "pch.h"
#include "MainPage.h"

namespace winrt
{
    hstring to_hstring(Windows::Storage::FileProperties::ThumbnailMode thumbnailMode);
}

namespace winrt::SDKTemplate
{
    struct FileExtensions
    {
        static std::vector<hstring> const& Document();
        static std::vector<hstring> const& Image();
        static std::vector<hstring> const& Music();
    };

    struct Errors
    {
        static constexpr wchar_t NoExifThumbnail[] = L"No result (no EXIF thumbnail or cached thumbnail available for fast retrieval)";
        static constexpr wchar_t NoThumbnail[] = L"No result (no thumbnail could be obtained from the selected file)";
        static constexpr wchar_t NoAlbumArt[] = L"No result (no album art available for this song)";
        static constexpr wchar_t NoIcon[] = L"No result (no icon available for this document type)";
        static constexpr wchar_t NoImages[] = L"No result (no thumbnail could be obtained from the selected folder - make sure that the folder contains images)";
        static constexpr wchar_t FileGroupEmpty[] = L"No result (unexpected error: retrieved file group was null)";
        static constexpr wchar_t FileGroupLocation[] = L"File groups are only available for library locations, please select a folder from one of your libraries";
        static constexpr wchar_t Cancel[] = L"No result (operation cancelled, no item selected)";
        static constexpr wchar_t InvalidSize[] = L"Invalid size (specified size must be numerical and greater than zero)";
    };

    struct Helpers
    {
        static Windows::UI::Xaml::Media::Imaging::BitmapImage PlaceholderImage();
        static void DisplayResult(
            Windows::UI::Xaml::Controls::Image const& image,
            Windows::UI::Xaml::Controls::TextBlock const& textBlock,
            Windows::Storage::FileProperties::ThumbnailMode thumbnailMode,
            uint32_t size,
            Windows::Storage::IStorageItem const& item,
            Windows::Storage::FileProperties::StorageItemThumbnail const& thumbnail,
            bool isGroup);
        static void ResetOutput(
            MainPage const& rootPage,
            Windows::UI::Xaml::Controls::Image const& image,
            Windows::UI::Xaml::Controls::TextBlock const& output,
            Windows::UI::Xaml::Controls::TextBlock const& outputDetails = nullptr);
    };
}
