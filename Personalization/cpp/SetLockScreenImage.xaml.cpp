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
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "SetLockscreenImage.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Pickers;
using namespace Windows::System::UserProfile;

SetLockscreenImage::SetLockscreenImage()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void SetLockscreenImage::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

/// <summary>
/// This loads the File Picker so that the user may pick an image. Upon valid image selection, the image is then set as the lock screen image.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void SetLockscreenImage::PickAndSetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    rootPage->NotifyUser(" ", NotifyType::StatusMessage);
    FileOpenPicker^ imagePicker = ref new FileOpenPicker();
    imagePicker->ViewMode = PickerViewMode::Thumbnail;
    imagePicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    imagePicker->FileTypeFilter->Append(".jpg");
    imagePicker->FileTypeFilter->Append(".jpeg");
    imagePicker->FileTypeFilter->Append(".png");
    imagePicker->FileTypeFilter->Append(".bmp");

    create_task(imagePicker->PickSingleFileAsync()).then([this](StorageFile^ imageFile)
    {
        if (imageFile != nullptr)
        {
            rootPage->NotifyUser("Selected image: " + imageFile->Name, NotifyType::StatusMessage);

            // Application now has access to the picked file, setting image to lockscreen.
            create_task(Windows::System::UserProfile::LockScreen::SetImageFileAsync(imageFile)).then([this](task<void> result)
            {
                try
                {
                    // Use result.get() to trigger exception if invalid image is chosen
                    result.get();
                    // Retrieve the lock screen image that was set
                    auto imageStream = Windows::System::UserProfile::LockScreen::GetImageStream();
                    if (imageStream != nullptr)
                    {
                        auto bitmapImage = ref new BitmapImage();
                        bitmapImage->SetSource(imageStream);
                        SetLockscreenImageOutputImage->Source = bitmapImage;
                        SetLockscreenImageOutputImage->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    }
                    else
                    {
                        SetLockscreenImageOutputImage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
                        rootPage->NotifyUser("Setting the lock screen image failed.  Make sure your copy of Windows is activated.", NotifyType::ErrorMessage);
                    }
                }
                catch (Exception ^ex)
                {
                    SetLockscreenImageOutputImage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
                    rootPage->NotifyUser("Invalid image selected or error opening stream.", NotifyType::ErrorMessage);
                }
            });
        }
        else
        {
            rootPage->NotifyUser("No image was selected", NotifyType::StatusMessage);
            create_task(Package::Current->InstalledLocation->CreateFileAsync("Assets\\placeholder-sdk.png",
                CreationCollisionOption::OpenIfExists))
                .then([this](StorageFile^ imageFile) -> IAsyncOperation<IRandomAccessStreamWithContentType^>^
            {
                return imageFile->OpenReadAsync();
            }).then([this](IRandomAccessStreamWithContentType^ imageStream)
            {
                auto bitmapImage = ref new BitmapImage();
                bitmapImage->SetSource(imageStream);
                SetLockscreenImageOutputImage->Source = bitmapImage;
            });
        }
    });
}
