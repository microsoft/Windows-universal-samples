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
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario3::Scenario3()
    {
        InitializeComponent();
    }

    hstring Scenario3::GetPropertyDisplayValue(IInspectable const& rawValue)
    {
        if (!rawValue)
        {
            return L"none";
        }
        else
        {
            return unbox_value<hstring>(rawValue);
        }
    }

    fire_and_forget Scenario3::GetFilesButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Define some properties we will be using.
        hstring CopyrightProperty = L"System.Copyright";
        hstring ColorSpaceProperty = L"System.Image.ColorSpace";

        // Reset output.
        OutputPanel().Children().Clear();

        // Set up file type filter.
        std::vector<hstring> fileTypeFilter{ L".jpg", L".png", L".bmp", L".gif" };

        // Create query options.
        QueryOptions queryOptions{ CommonFileQuery::OrderByName, fileTypeFilter };

        // Set up property prefetch - use the PropertyPrefetchOptions for top-level properties
        // and a list for additional properties.
        std::vector<hstring> propertyNames{ CopyrightProperty, ColorSpaceProperty };
        queryOptions.SetPropertyPrefetch(PropertyPrefetchOptions::ImageProperties, propertyNames);

        // Set up thumbnail prefetch if needed, e.g. when creating a picture gallery view.
        /*
        const uint32_t requestedSize = 190;
        const ThumbnailMode thumbnailMode = ThumbnailMode::PicturesView;
        const ThumbnailOptions thumbnailOptions = ThumbnailOptions::UseCurrentScale;
        queryOptions.SetThumbnailPrefetch(thumbnailMode, requestedSize, thumbnailOptions);
        */

        // Set up the query and retrieve files.
        StorageFolder picturesFolder = co_await KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary);
        auto query = picturesFolder.CreateFileQueryWithOptions(queryOptions);
        IVectorView<StorageFile> fileList = co_await query.GetFilesAsync();
        for (StorageFile const& file : fileList)
        {
            OutputPanel().Children().Append(Helpers::CreateHeaderTextBlock(file.Name()));

            // GetImagePropertiesAsync will return synchronously when prefetching has been able to
            // retrieve the properties in advance.
            ImageProperties properties = co_await file.Properties().GetImagePropertiesAsync();
            OutputPanel().Children().Append(Helpers::CreateLineItemTextBlock(L"Dimensions: " + to_hstring(properties.Width()) + L"x" + to_hstring(properties.Height())));

            // Similarly, extra properties are retrieved asynchronously but may
            // return immediately when prefetching has fulfilled its task.
            IMap<hstring, IInspectable> extraProperties = co_await file.Properties().RetrievePropertiesAsync(std::move(propertyNames));
            auto propValue = GetPropertyDisplayValue(extraProperties.Lookup(CopyrightProperty));
            OutputPanel().Children().Append(Helpers::CreateLineItemTextBlock(L"Copyright: " + propValue));
            propValue = GetPropertyDisplayValue(extraProperties.Lookup(ColorSpaceProperty));
            OutputPanel().Children().Append(Helpers::CreateLineItemTextBlock(L"Color space: " + propValue));

            // Thumbnails can also be retrieved and used.
            // auto thumbnail = co_await file.GetThumbnailAsync(thumbnailMode, requestedSize, thumbnailOptions);
        }
    }
}
