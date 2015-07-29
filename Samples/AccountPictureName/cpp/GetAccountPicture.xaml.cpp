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
#include "GetAccountPicture.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

GetAccountPicture::GetAccountPicture() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void GetAccountPicture::GetSmallImageButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    HideImageAndVideoControls();

    if (UserInformation::NameAccessAllowed)
    {
        auto smallImageFile = UserInformation::GetAccountPicture(AccountPictureKind::SmallImage);
        if (smallImageFile != nullptr)
        {
            rootPage->NotifyUser("Path = " + smallImageFile->Path, NotifyType::StatusMessage);
            create_task(smallImageFile->OpenReadAsync()).then(
                [this](task<IRandomAccessStreamWithContentType^> imageStreamTask)
                {
                    try
                    {
                        auto imageStream = imageStreamTask.get();
                        auto bitmapImage = ref new BitmapImage();
                        bitmapImage->SetSource(imageStream);
                        SmallImage->Source = bitmapImage;
                        SmallImage->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    }
                    catch (Exception^ ex)
                    {
                        rootPage->NotifyUser("Error opening stream: " + ex->Message, NotifyType::ErrorMessage);
                    }
                });
        }
        else
        {
            rootPage->NotifyUser("Small account picture is not available.", NotifyType::ErrorMessage);
        }
    }
    else
    {
        rootPage->NotifyUser("Access to account picture disabled by Privacy Setting or Group Policy.", NotifyType::ErrorMessage);
    }
}

void GetAccountPicture::GetLargeImageButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    HideImageAndVideoControls();

    if (UserInformation::NameAccessAllowed)
    {
        auto largeImageFile = UserInformation::GetAccountPicture(AccountPictureKind::LargeImage);
        if (largeImageFile != nullptr)
        {
            rootPage->NotifyUser("Path = " + largeImageFile->Path, NotifyType::StatusMessage);
            create_task(largeImageFile->OpenReadAsync()).then(
                [this](task<IRandomAccessStreamWithContentType^> imageStreamTask)
                {
                    try
                    {
                        auto imageStream = imageStreamTask.get();
                        auto bitmapImage = ref new BitmapImage();
                        bitmapImage->SetSource(imageStream);
                        LargeImage->Source = bitmapImage;
                        LargeImage->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    }
                    catch (Exception^ ex)
                    {
                        rootPage->NotifyUser("Error opening stream: " + ex->Message, NotifyType::ErrorMessage);
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
}

void GetAccountPicture::GetVideoButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    HideImageAndVideoControls();

    if (UserInformation::NameAccessAllowed)
    {
        auto videoFile = UserInformation::GetAccountPicture(AccountPictureKind::Video);
        if (videoFile != nullptr)
        {
            rootPage->NotifyUser("Path = " + videoFile->Path, NotifyType::StatusMessage);
            create_task(videoFile->OpenReadAsync()).then(
                [this](task<IRandomAccessStreamWithContentType^> videoStreamTask)
                {
                    try
                    {
                        auto videoStream = videoStreamTask.get();
                        MediaPlayer->SetSource(videoStream, "video/mp4");
                        MediaPlayer->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    }
                    catch (Exception^ ex)
                    {
                        rootPage->NotifyUser("Error opening stream: " + ex->Message, NotifyType::ErrorMessage);
                    }
                });
        }
        else
        {
            rootPage->NotifyUser("Video is not available.", NotifyType::ErrorMessage);
        }
    }
    else
    {
        rootPage->NotifyUser("Access to account picture disabled by Privacy Setting or Group Policy.", NotifyType::ErrorMessage);
    }
}

void GetAccountPicture::HideImageAndVideoControls()
{
    SmallImage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    LargeImage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    MediaPlayer->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}
