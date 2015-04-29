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

//
// ShareDelayRenderedFiles.xaml.cpp
// Implementation of the ShareDelayRenderedFiles class
//

#include "pch.h"
#include "ShareDelayRenderedFiles.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::ApplicationModel::DataTransfer;

ShareDelayRenderedFiles::ShareDelayRenderedFiles()
{
    InitializeComponent();
}

bool ShareDelayRenderedFiles::GetShareContent(DataRequest^ request)
{
    bool succeeded = false;

    if (imageFile != nullptr)
    {
        auto requestData = request->Data;
        requestData->Properties->Title = "Delay rendered image";
        requestData->Properties->Description = "Resized image from the Share Source sample";
        requestData->Properties->Thumbnail = RandomAccessStreamReference::CreateFromFile(imageFile);
        requestData->SetDataProvider(StandardDataFormats::Bitmap, ref new DataProviderHandler(this, &ShareDelayRenderedFiles::OnDeferredImageRequestedHandler));
        succeeded = true;
    }
    else
    {
        request->FailWithDisplayText("Select an image you would like to share and try again.");
    }
    return succeeded;
}

void ShareDelayRenderedFiles::OnDeferredImageRequestedHandler(DataProviderRequest^ request)
{
    // In this delegate we provide updated Bitmap data using delayed rendering.

    if (imageFile != nullptr)
    {
        // If the delegate is calling any asynchronous operations it needs to acquire the deferral first. This lets the
        // system know that you are performing some operations that might take a little longer and that the call to
        // SetData could happen after the delegate returns. Once you acquired the deferral object you must call Complete
        // on it after your final call to SetData.
        auto deferral = request->GetDeferral();
        auto inMemoryStream = ref new InMemoryRandomAccessStream();

        // Use a reference-counted pointer to store the requested dimensions across function calls.
        std::shared_ptr<Size> newSize(new Size);

        create_task(imageFile->OpenAsync(FileAccessMode::Read)).then(
            [this](IRandomAccessStream^ stream) -> IAsyncOperation<BitmapDecoder^>^
        {
            return BitmapDecoder::CreateAsync(stream);
        }).then([this, inMemoryStream, newSize](BitmapDecoder^ imageDecoder) -> IAsyncOperation<BitmapEncoder^>^
        {
            newSize.get()->Height = imageDecoder->OrientedPixelHeight * 0.5f;
            newSize.get()->Width = imageDecoder->OrientedPixelWidth * 0.5f;
            return BitmapEncoder::CreateForTranscodingAsync(inMemoryStream, imageDecoder);
        }).then([this, newSize](BitmapEncoder^ imageEncoder) -> IAsyncAction^
        {
            imageEncoder->BitmapTransform->ScaledWidth = safe_cast<unsigned int>(newSize.get()->Width);
            imageEncoder->BitmapTransform->ScaledHeight = safe_cast<unsigned int>(newSize.get()->Height);
            return imageEncoder->FlushAsync();
        }).then([this, deferral, request, inMemoryStream](task<void> newTask)
        {
            // Forces any exceptions from previous operations to get thrown here.
            // That way, we can ensure that the deferral completed method always gets called.
            try
            {
                newTask.get();
                request->SetData(RandomAccessStreamReference::CreateFromStream(inMemoryStream));
            }
            catch (Exception^)
            {
                deferral->Complete();
                throw;
            }

            // If the operation completed successfully, end the deferral.
            deferral->Complete();
        });
    }
}

void ShareDelayRenderedFiles::SelectImageButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto imagePicker = ref new FileOpenPicker();
    imagePicker->ViewMode = PickerViewMode::Thumbnail;
    imagePicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    imagePicker->FileTypeFilter->Append(".jpg");
    imagePicker->FileTypeFilter->Append(".png");
    imagePicker->FileTypeFilter->Append(".bmp");
    imagePicker->FileTypeFilter->Append(".gif");
    imagePicker->FileTypeFilter->Append(".tif");

    create_task(imagePicker->PickSingleFileAsync()).then([this](StorageFile^ pickedFile)
    {
        if (pickedFile != nullptr)
        {
            imageFile = pickedFile;

            // Display the image in the UI.
            create_task(pickedFile->OpenAsync(FileAccessMode::Read)).then([this](IRandomAccessStream^ displayStream)
            {
                auto bitmapImage = ref new BitmapImage();
                bitmapImage->SetSource(displayStream);
                ImageHolder->Source = bitmapImage;
            });
            MainPage::Current->NotifyUser("Selected " + imageFile->Name +".", NotifyType::StatusMessage);

            ShareStep->Visibility = Windows::UI::Xaml::Visibility::Visible;
        }
    });
}
