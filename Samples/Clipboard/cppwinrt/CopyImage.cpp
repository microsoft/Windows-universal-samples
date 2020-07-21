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
#include "SampleConfiguration.h"
#include "CopyImage.h"
#include "CopyImage.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;

namespace winrt::SDKTemplate::implementation
{
    CopyImage::CopyImage()
    {
        InitializeComponent();
    }

    void CopyImage::CopyButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        CopyBitmap(false);
    }

    void CopyImage::CopyWithDelayedRenderingButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        CopyBitmap(true);
    }

    fire_and_forget CopyImage::CopyBitmap(bool isDelayRendered)
    {
        auto lifetime = get_strong();

        auto imagePicker = FileOpenPicker();
        imagePicker.ViewMode(PickerViewMode::Thumbnail);
        imagePicker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
        imagePicker.FileTypeFilter().ReplaceAll({ L".jpg", L".png", L".bmp", L".gif", L".tif" });

        auto imageFile = co_await imagePicker.PickSingleFileAsync();
        if (imageFile)
        {
            auto dataPackage = DataPackage();
            hstring message;

            // Use one click handler for two operations: regular copy and copy using delayed rendering
            // Differentiate the case by the button name
            if (isDelayRendered)
            {
                dataPackage.SetDataProvider(StandardDataFormats::Bitmap(),
                    [imageFile](auto&& request)
                    {
                        OnDeferredImageRequestedHandler(request, imageFile);
                    });
                message = L"Image has been copied using delayed rendering";
            }
            else
            {
                dataPackage.SetBitmap(RandomAccessStreamReference::CreateFromFile(imageFile));
                message = L"Image has been copied";
            }

            if (Clipboard::SetContentWithOptions(dataPackage, nullptr))
            {
                rootPage.NotifyUser(message, NotifyType::StatusMessage);
            }
            else
            {
                // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                rootPage.NotifyUser(L"Error copying content to clipboard.", NotifyType::ErrorMessage);
            }
        }
    }

    fire_and_forget CopyImage::PasteButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        // Get the bitmap and display it.
        auto dataPackageView = Clipboard::GetContent();
        if (dataPackageView.Contains(StandardDataFormats::Bitmap()))
        {
            IRandomAccessStreamReference imageReceived = nullptr;
            try
            {
                imageReceived = co_await dataPackageView.GetBitmapAsync();
            }
            catch (hresult_error const& ex)
            {
                rootPage.NotifyUser(L"Error retrieving image from Clipboard: " + ex.message(), NotifyType::ErrorMessage);
            }

            if (imageReceived)
            {
                auto imageStream = co_await imageReceived.OpenReadAsync();
                auto bitmapImage = BitmapImage();
                bitmapImage.SetSource(imageStream);
                OutputImage().Source(bitmapImage);
                OutputImage().Visibility(Visibility::Visible);
                OutputText().Text(L"Image is retrieved from the clipboard and pasted successfully.");
                imageStream.Close();
            }
        }
        else
        {
            OutputText().Text(L"Bitmap format is not available in clipboard");
            OutputImage().Visibility(Visibility::Collapsed);
        }
    }

    fire_and_forget CopyImage::OnDeferredImageRequestedHandler(DataProviderRequest request, StorageFile imageFile)
    {
        // Since this method is calling async methods prior to setting the data in DataPackage,
        // deferral object must be used
        auto deferral = request.GetDeferral();

        try
        {
            auto imageStream = co_await imageFile.OpenAsync(FileAccessMode::Read);

            // Decode the image
            auto imageDecoder = co_await BitmapDecoder::CreateAsync(imageStream);

            // Re-encode the image at 50% width and height
            auto inMemoryStream = InMemoryRandomAccessStream();
            auto imageEncoder = co_await BitmapEncoder::CreateForTranscodingAsync(inMemoryStream, imageDecoder);
            imageEncoder.BitmapTransform().ScaledWidth(static_cast<uint32_t>(imageDecoder.OrientedPixelWidth() * 0.5));
            imageEncoder.BitmapTransform().ScaledHeight(static_cast<uint32_t>(imageDecoder.OrientedPixelHeight() * 0.5));
            co_await imageEncoder.FlushAsync();

            request.SetData(RandomAccessStreamReference::CreateFromStream(inMemoryStream));

            SampleState::DisplayToast(L"Image has been set via deferral");
        }
        catch (hresult_error const& ex)
        {
            SampleState::DisplayToast(L"Failed delayed rendering - " + ex.message());
        }
        deferral.Complete();
    }
}
