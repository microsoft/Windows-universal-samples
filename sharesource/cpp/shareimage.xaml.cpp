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
// ShareImage.xaml.cpp
// Implementation of the ShareImage class
//

#include "pch.h"
#include "ShareImage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Pickers;
using namespace concurrency;
using namespace Windows::ApplicationModel::DataTransfer;

ShareImage::ShareImage()
{
    InitializeComponent();
}

bool ShareImage::GetShareContent(DataRequest^ request)
{
    bool succeeded = false;

    if (imageFile != nullptr)
    {
        auto requestData = request->Data;
        requestData->Properties->Title = TitleInputBox->Text;

        // The description is optional.
        auto dataPackageDescription = DescriptionInputBox->Text;
        if (dataPackageDescription != nullptr)
        {
            requestData->Properties->Description = dataPackageDescription;
        }

        // It's recommended to use both SetBitmap and SetStorageItems for sharing a single image
        // since the target app may only support one or the other.

        auto imageItems = ref new Vector<IStorageItem^>();
        imageItems->Append(imageFile);
        requestData->SetStorageItems(imageItems);

        auto imageStreamRef = RandomAccessStreamReference::CreateFromFile(imageFile);
        requestData->Properties->Thumbnail = imageStreamRef;
        requestData->SetBitmap(imageStreamRef);
        succeeded = true;
    }
    else
    {
        request->FailWithDisplayText("Select an image you would like to share and try again.");
    }
    return succeeded;
}

void ShareImage::SelectImageButton_Click(Object^ sender, RoutedEventArgs^ e)
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
            MainPage::Current->NotifyUser("Selected " + imageFile->Name + ".", NotifyType::StatusMessage);

            ShareStep->Visibility = Windows::UI::Xaml::Visibility::Visible;
        }
    });
}
