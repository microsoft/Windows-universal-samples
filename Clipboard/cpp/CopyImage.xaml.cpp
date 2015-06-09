//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// CopyImage.xaml.cpp
// Implementation of the CopyImage class
//

#include "pch.h"
#include "CopyImage.xaml.h"
#include "SampleConfiguration.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Imaging;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Imaging;

// App specific namespace references
using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Imaging;

CopyImage::CopyImage()
{
    InitializeComponent();
}

void CopyImage::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}


void CopyImage::CopyWithDelayedRenderingButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    CopyBitmap(true);
}

void CopyImage::CopyButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    CopyBitmap(false);
}

void CopyImage::CopyBitmap(bool useDelayedRendering)
{
    OutputText->Text = "";
    OutputImage->Source = nullptr;

    // Select an image using the filepicker.
    auto imageFilePicker = ref new FileOpenPicker();
    imageFilePicker->ViewMode = PickerViewMode::Thumbnail;
    imageFilePicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;

    // Supported image formats for clipboard.
    imageFilePicker->FileTypeFilter->Append(".jpg");
    imageFilePicker->FileTypeFilter->Append(".png");
    imageFilePicker->FileTypeFilter->Append(".bmp");
    imageFilePicker->FileTypeFilter->Append(".gif");
    imageFilePicker->FileTypeFilter->Append(".tif");

    create_task(imageFilePicker->PickSingleFileAsync()).then(
        [this, useDelayedRendering](task<StorageFile^> imageTask)
    {
        StorageFile^ imageFile = imageTask.get();
        if (imageFile)
        {
            imgStreamRef = RandomAccessStreamReference::CreateFromFile(imageFile);
            auto dataPackage = ref new DataPackage();
            if (useDelayedRendering)
            {
                dataPackage->SetDataProvider(StandardDataFormats::Bitmap, ref new DataProviderHandler(this, &CopyImage::OnDeferredImageRequestedHandler));
            }
            else
            {
                dataPackage->SetBitmap(imgStreamRef);
            }

            try
            {
                // Set the contents in clipboard
                DataTransfer::Clipboard::SetContent(dataPackage);
                if (useDelayedRendering)
                {
                    rootPage->NotifyUserBackgroundThread("Image has been copied to clipboard using delayed rendering.", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUserBackgroundThread("Image has been copied to clipboard.", NotifyType::StatusMessage);
                }
            }
            catch (Exception^ ex)
            {
                // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                rootPage->NotifyUserBackgroundThread("Error copying image to Clipboard: " + ex->Message + ".", NotifyType::ErrorMessage);
            }
        }
        else
        {
            rootPage->NotifyUserBackgroundThread("No Image was selected", NotifyType::ErrorMessage);
        }
    });
}

void CopyImage::PasteButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    OutputText->Text = "";
    OutputImage->Source = nullptr;

    auto dataPackageView = DataTransfer::Clipboard::GetContent();
    if (dataPackageView->Contains(StandardDataFormats::Bitmap))
    {
        create_task(dataPackageView->GetBitmapAsync()).then(
            [this](task<RandomAccessStreamReference^> bitmapTask)
        {
            RandomAccessStreamReference^ bitmapStreamRef;

            try
            {
                bitmapStreamRef = bitmapTask.get();
            }
            catch (Exception^ ex)
            {
                rootPage->NotifyUserBackgroundThread("Failed GetBitmapAsync - " + ex->Message, NotifyType::ErrorMessage);
            }

            if (bitmapStreamRef != nullptr)
            {
                create_task(bitmapStreamRef->OpenReadAsync()).then(
                    [this](task<IRandomAccessStreamWithContentType^> imageTask)
                {
                    IRandomAccessStreamWithContentType^ imageStream;

                    try
                    {
                        imageStream = imageTask.get();
                    }
                    catch (Exception^ ex)
                    {
                        rootPage->NotifyUserBackgroundThread("Failed OpenReadAsync - " + ex->Message, NotifyType::ErrorMessage);
                    }

                    if (imageStream != nullptr)
                    {
                        auto bitmapImage = ref new BitmapImage();
                        bitmapImage->SetSource(imageStream);

                        // Display the image.
                        OutputImage->Source = bitmapImage;
                        OutputImage->Visibility = Windows::UI::Xaml::Visibility::Visible;
                        rootPage->NotifyUserBackgroundThread("Image in clipboard rendered below:", NotifyType::StatusMessage);
                    }
                });
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Bitmap format is not available in clipboard", NotifyType::StatusMessage);
    }
}

void CopyImage::OnDeferredImageRequestedHandler(DataProviderRequest^ request)
{
    if (imgStreamRef != nullptr)
    {
        // Since this method is calling async methods prior to setting the data in DataPackage,
        // deferral object must be used
        auto deferral = request->GetDeferral();
        auto inMemoryStream = ref new InMemoryRandomAccessStream();

        // Use a reference-counted pointer to store the requested dimensions across function calls.
        std::shared_ptr<Size> newSize(new Size);

        create_task(imgStreamRef->OpenReadAsync()).then(
            [this](task<IRandomAccessStreamWithContentType^> streamTask) -> IAsyncOperation<BitmapDecoder^> ^
        {
            return BitmapDecoder::CreateAsync(streamTask.get());
        }).then([this, inMemoryStream, newSize](BitmapDecoder^ imageDecoder) -> IAsyncOperation<BitmapEncoder^> ^
        {
            newSize.get()->Height = imageDecoder->OrientedPixelHeight * 0.5f;
            newSize.get()->Width = imageDecoder->OrientedPixelWidth * 0.5f;
            return BitmapEncoder::CreateForTranscodingAsync(inMemoryStream, imageDecoder);
        }).then([this, newSize](BitmapEncoder^ imageEncoder) -> IAsyncAction ^
        {
            imageEncoder->BitmapTransform->ScaledWidth = safe_cast<unsigned int>(newSize.get()->Width);
            imageEncoder->BitmapTransform->ScaledHeight = safe_cast<unsigned int>(newSize.get()->Height);
            return imageEncoder->FlushAsync();
        }).then([this, request, deferral, inMemoryStream](task<void> newTask)
        {
            // Forces any exceptions from previous operations to get thrown here.
            // That way, we can ensure that the deferral completed method always gets called.
            try
            {
                newTask.get();
                request->SetData(RandomAccessStreamReference::CreateFromStream(inMemoryStream));
            }
            catch (Exception^ ex)
            {
                rootPage->NotifyUserBackgroundThread("Failed delayed rendering - " + ex->Message, NotifyType::ErrorMessage);
            }

            // End the deferral.
            deferral->Complete();
        });
    }
}

#pragma endregion
