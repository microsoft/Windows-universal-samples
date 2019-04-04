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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"File thumbnails C++/WinRT sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Display a thumbnail for a picture",    xaml_typename<SDKTemplate::Scenario1>() },
    Scenario{ L"Display album art for a song",         xaml_typename<SDKTemplate::Scenario2>() },
    Scenario{ L"Display an icon for a document",       xaml_typename<SDKTemplate::Scenario3>() },
    Scenario{ L"Display a thumbnail for a folder",     xaml_typename<SDKTemplate::Scenario4>() },
    Scenario{ L"Display a thumbnail for a file group", xaml_typename<SDKTemplate::Scenario5>() },
    Scenario{ L"Display a scaled image",               xaml_typename<SDKTemplate::Scenario6>() },
});

hstring winrt::to_hstring(ThumbnailMode thumbnailMode)
{
    switch (thumbnailMode)
    {
    case ThumbnailMode::PicturesView: return L"PicturesView";
    case ThumbnailMode::VideosView: return L"VideosView";
    case ThumbnailMode::MusicView: return L"MusicView";
    case ThumbnailMode::DocumentsView: return L"DocumentsView";
    case ThumbnailMode::ListView: return L"ListView";
    case ThumbnailMode::SingleItem: return L"SingleItem";
    default: return L"Unknown " + to_hstring((uint32_t)thumbnailMode);
    }
}

std::vector<hstring> const& FileExtensions::Document()
{
    static std::vector<hstring> document{ L".doc", L".xls", L".ppt", L".docx", L".xlsx", L".pptx", L".pdf", L".txt", L".rtf" };
    return document;
}

std::vector<hstring> const& FileExtensions::Image()
{
    static std::vector<hstring> image{ L".jpg", L".png", L".bmp", L".gif", L".tif" };
    return image;
}

std::vector<hstring> const& FileExtensions::Music()
{
    static std::vector<hstring> music{ L".mp3", L".wma", L".m4a", L".aac" };
    return music;
}

BitmapImage Helpers::PlaceholderImage()
{
    static BitmapImage placeholder{ Uri{ L"ms-appx:///Assets/placeholder-sdk.png" } };
    return placeholder;
}

void Helpers::DisplayResult(Image const& image, TextBlock const& textBlock,
    ThumbnailMode thumbnailMode, uint32_t size, IStorageItem const& item,
    StorageItemThumbnail const& thumbnail, bool isGroup)
{
    BitmapImage bitmapImage;
    bitmapImage.SetSource(thumbnail);
    image.Source(bitmapImage);

    std::wstringstream text;
    text << L"ThumbnailMode." << std::wstring_view{ to_hstring(thumbnailMode) } << std::endl;
    text << (isGroup ? L"Group" : item.IsOfType(StorageItemTypes::File) ? L"File" : L"Folder")
        << L" used: " << std::wstring_view{ item.Name() } << std::endl;
    text << L"Requested size: " << size << std::endl;
    text << L"Returned size: " << thumbnail.OriginalWidth() << L"x" << thumbnail.OriginalHeight() << std::endl;
    textBlock.Text(text.str());
}

void Helpers::ResetOutput(MainPage const& rootPage, Image const& image, TextBlock const& output, TextBlock const& outputDetails)
{
    image.Source(PlaceholderImage());
    rootPage.NotifyUser(L"", NotifyType::StatusMessage);
    output.Text(L"");
    if (outputDetails)
    {
        outputDetails.Text(L"");
    }
}

