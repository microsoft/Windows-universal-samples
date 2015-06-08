// Copyright (c) Microsoft. All rights reserved.

#pragma once 
#include "pch.h"

#include <collection.h>

namespace SDKTemplate
{
    value struct Scenario;

    class Errors
    {
    public:
        static Platform::String^ NoExifThumbnail;
        static Platform::String^ NoThumbnail;
        static Platform::String^ NoAlbumArt;
        static Platform::String^ NoIcon;
        static Platform::String^ NoImages;
        static Platform::String^ FileGroupEmpty;
        static Platform::String^ FileGroupLocation;
        static Platform::String^ Cancel;
        static Platform::String^ InvalidSize;
    };

    class FileExtensions
    {
    public:
        static Platform::Array<Platform::String^>^ Document;
        static Platform::Array<Platform::String^>^ Image;
        static Platform::Array<Platform::String^>^ Music;
    };

    partial ref class MainPage
    {
    internal:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return "File thumbnails C++ sample";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

    internal:
        void ResetOutput(Windows::UI::Xaml::Controls::Image^ image, Windows::UI::Xaml::Controls::TextBlock^ output, Windows::UI::Xaml::Controls::TextBlock^ outputDetails);
        static void DisplayResult(Windows::UI::Xaml::Controls::Image^ image, Windows::UI::Xaml::Controls::TextBlock^ textBlock,
                                  Platform::String^ thumbnailModeName, size_t size,
                                  Windows::Storage::IStorageItem^ item, Windows::Storage::FileProperties::StorageItemThumbnail^ thumbnail,
                                  bool isGroup);

    private:
        static Platform::Array<Scenario>^ scenariosInner;

        Windows::UI::Xaml::Media::Imaging::BitmapImage^ _placeholder;
        Windows::UI::Xaml::Media::Imaging::BitmapImage^ GetPlaceHolderImage();
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
