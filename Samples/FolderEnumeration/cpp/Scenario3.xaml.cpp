//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario3.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include "Scenario3.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::BulkAccess;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Search;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

String^ Scenario3::CopyrightProperty = "System.Copyright";
String^ Scenario3::ColorSpaceProperty = "System.Image.ColorSpace";

Scenario3::Scenario3()
{
    InitializeComponent();
}

String^ Scenario3::GetPropertyDisplayValue(Object^ property)
{
    String^ value;
    if (property == nullptr || property->ToString() == "")
    {
        value = "none";
    }
    else
    {
        value = property->ToString();
    }

    return value;
}

void Scenario3::GetFilesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Reset output.
    OutputPanel->Children->Clear();

    // Set up file type filter.
    auto fileTypeFilter = ref new Vector<String^>();
    fileTypeFilter->Append(".jpg");
    fileTypeFilter->Append(".png");
    fileTypeFilter->Append(".bmp");
    fileTypeFilter->Append(".gif");

    // Create query options.
    auto queryOptions = ref new QueryOptions(CommonFileQuery::OrderByName, fileTypeFilter);

    // Set up property prefetch - use the PropertyPrefetchOptions for top-level properties
    // and a vector for additional properties.
    auto propertyNames = ref new Vector<String^>();
    propertyNames->Append(CopyrightProperty);
    propertyNames->Append(ColorSpaceProperty);
    queryOptions->SetPropertyPrefetch(PropertyPrefetchOptions::ImageProperties, propertyNames);

    // Set up thumbnail prefetch if needed, e.g. when creating a picture gallery view.
    /*
    const UINT requestedSize = 190;
    const ThumbnailMode thumbnailMode = ThumbnailMode::PicturesView;
    const ThumbnailOptions thumbnailOptions = ThumbnailOptions::UseCurrentScale;
    queryOptions->SetThumbnailPrefetch(thumbnailMode, requestedSize, thumbnailOptions);
    */

    // Set up the query and retrieve files.
    create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary)).then([this, queryOptions](StorageFolder^ picturesFolder)
    {
        auto query = picturesFolder->CreateFileQueryWithOptions(queryOptions);
        return query->GetFilesAsync();
    }).then([=](IVectorView<StorageFile^>^ files)
    {
        std::for_each(begin(files), end(files), [=](StorageFile^ file)
        {
            // Create UI elements for the output and fill in the contents when they are available.
            OutputPanel->Children->Append(CreateHeaderTextBlock(file->Name));
            TextBlock^ imagePropTextBlock = CreateLineItemTextBlock();
            OutputPanel->Children->Append(imagePropTextBlock);
            TextBlock^ copyrightTextBlock = CreateLineItemTextBlock();
            OutputPanel->Children->Append(copyrightTextBlock);
            TextBlock^ colorspaceTextBlock = CreateLineItemTextBlock();
            OutputPanel->Children->Append(colorspaceTextBlock);

            // GetImagePropertiesAsync will return synchronously when prefetching has been able to
            // retrieve the properties in advance.
            create_task(file->Properties->GetImagePropertiesAsync()).then([=](ImageProperties^ properties)
            {
                imagePropTextBlock->Text = "Dimensions: " + properties->Width + "x" + properties->Height;
            });

            // Similarly, extra properties are retrieved asynchronously but may
            // return immediately when prefetching has fulfilled its task.
            create_task(file->Properties->RetrievePropertiesAsync(propertyNames)).then([=](IMap<String^, Object^>^ extraProperties)
            {
                copyrightTextBlock->Text = "Copyright: " + GetPropertyDisplayValue(extraProperties->Lookup(CopyrightProperty));
                colorspaceTextBlock->Text = "Color space: " + GetPropertyDisplayValue(extraProperties->Lookup(ColorSpaceProperty));
            });

            // Thumbnails can also be retrieved and used.
            /*
            create_task(file->GetThumbnailAsync(thumbnailMode, requestedSize, thumbnailOptions)).then([=](StorageItemThumbnail^ thumbnail)
            {
                // Use the thumbnail.
            });
            */
        });
    });
}

TextBlock^ Scenario3::CreateHeaderTextBlock(String^ contents)
{
    TextBlock^ textBlock = ref new TextBlock();
    textBlock->Text = contents;
    textBlock->Style = static_cast<Xaml::Style^>(Application::Current->Resources->Lookup("SampleHeaderTextStyle"));
    textBlock->TextWrapping = TextWrapping::Wrap;
    return textBlock;
}

TextBlock^ Scenario3::CreateLineItemTextBlock()
{
    TextBlock^ textBlock = ref new TextBlock();
    textBlock->Style = static_cast<Xaml::Style^>(Application::Current->Resources->Lookup("BasicTextStyle"));
    textBlock->TextWrapping = TextWrapping::Wrap;
    Thickness margin = textBlock->Margin;
    margin.Left = 20;
    textBlock->Margin = margin;
    return textBlock;
}
