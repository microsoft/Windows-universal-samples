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
#include "SetAccountPicture.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Pickers;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

SetAccountPicture::SetAccountPicture() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void SetAccountPicture::OnNavigatedTo(NavigationEventArgs^ e)
{
    accountPictureChangedToken = UserInformation::AccountPictureChanged += ref new EventHandler<Object^>(this, &SetAccountPicture::PictureChanged);
}

void SetAccountPicture::OnNavigatedFrom(NavigationEventArgs^ e)
{
    UserInformation::AccountPictureChanged -= accountPictureChangedToken;
}

void SetAccountPicture::SetImageButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FileOpenPicker^ imagePicker = ref new FileOpenPicker();
    imagePicker->ViewMode = PickerViewMode::Thumbnail;
    imagePicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    imagePicker->FileTypeFilter->Append(".jpg");
    imagePicker->FileTypeFilter->Append(".jpeg");
    imagePicker->FileTypeFilter->Append(".png");
    imagePicker->FileTypeFilter->Append(".bmp");

    create_task(imagePicker->PickSingleFileAsync()).then(
        [this](StorageFile^ imageFile)
    {
        if (imageFile != nullptr)
        {
            rootPage->NotifyUser("Setting " + imageFile->Name + " as account picture ...",
                NotifyType::StatusMessage);
            create_task(UserInformation::SetAccountPictureAsync(imageFile)).then(
                [this](SetAccountPictureResult setPictureResult)
            {
                if (setPictureResult == SetAccountPictureResult::Success)
                {
                    rootPage->NotifyUser("Successfully updated account picture.", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser("Setting account picture failed.", NotifyType::ErrorMessage);
                }
            });
        }
        else
        {
            rootPage->NotifyUser("No image was selected.", NotifyType::StatusMessage);
        }
    });
}

void SetAccountPicture::SetVideoButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FileOpenPicker^ videoPicker = ref new FileOpenPicker();
    videoPicker->ViewMode = PickerViewMode::Thumbnail;
    videoPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    videoPicker->FileTypeFilter->Append(".mp4");
    videoPicker->FileTypeFilter->Append(".wmv");
    videoPicker->FileTypeFilter->Append(".mpeg");
    videoPicker->FileTypeFilter->Append(".mov");

    create_task(videoPicker->PickSingleFileAsync()).then(
        [this](StorageFile^ videoFile)
    {
        if (videoFile != nullptr)
        {
            rootPage->NotifyUser("Setting " + videoFile->Name + " as account picture ...",
                NotifyType::StatusMessage);
            create_task(UserInformation::SetAccountPictureAsync(videoFile)).then(
                [this](SetAccountPictureResult setPictureResult)
            {
                if (setPictureResult == SetAccountPictureResult::Success)
                {
                    rootPage->NotifyUser("Successfully updated account picture.", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser("Setting account picture failed.", NotifyType::ErrorMessage);
                }
            });
        }
        else
        {
            rootPage->NotifyUser("No video was selected.", NotifyType::StatusMessage);
        }
    });
}

void SetAccountPicture::PictureChanged(Platform::Object^ sender, Platform::Object^ e)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
        if (UserInformation::NameAccessAllowed)
        {
            auto largeImageFile = UserInformation::GetAccountPicture(AccountPictureKind::LargeImage);
            if (largeImageFile != nullptr)
            {
                create_task(largeImageFile->OpenReadAsync()).then(
                    [this](task<IRandomAccessStreamWithContentType^> imageStreamTask)
                {
                    try
                    {
                        auto imageStream = imageStreamTask.get();
                        auto bitmapImage = ref new BitmapImage();
                        bitmapImage->SetSource(imageStream);
                        AccountPictureImage->Source = bitmapImage;
                        AccountPictureImage->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    }
                    catch (Exception^ ex)
                    {
                        rootPage->NotifyUser("Failed to read from stream. " + ex->Message, NotifyType::ErrorMessage);
                    }
                });
            }
            else
            {
                rootPage->NotifyUser("No large account picture image returned for current user.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            rootPage->NotifyUser("Access to account picture disabled by Privacy Setting or Group Policy.", NotifyType::ErrorMessage);
        }
    }));
}
