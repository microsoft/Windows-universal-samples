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
#include "Scenario2_AppendClips.xaml.h"

using namespace SDKTemplate;
using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Media::Core;
using namespace Windows::Media::Editing;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario2_AppendClips::Scenario2_AppendClips() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario2_AppendClips::OnNavigatedFrom(NavigationEventArgs^ e)
{
    mediaElement->Source = nullptr;
    mediaStreamSource = nullptr;
}

void Scenario2_AppendClips::ChooseFirstVideo_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Get first file
    auto picker = ref new FileOpenPicker();
    picker->SuggestedStartLocation = PickerLocationId::VideosLibrary;
    picker->FileTypeFilter->Append(".mp4");
    
    create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ videoFile)
    {
        if (videoFile == nullptr)
        {           
            rootPage->NotifyUser("File picking cancelled", NotifyType::ErrorMessage);
            return;
        }

        this->firstVideoFile = videoFile;
        create_task(this->firstVideoFile->OpenReadAsync()).then([this](IRandomAccessStreamWithContentType^ videoSource)
        {
            mediaElement->SetSource(videoSource, this->firstVideoFile->ContentType);
            chooseSecondFile->IsEnabled = true;
        });
    });
}

void Scenario2_AppendClips::ChooseSecondVideo_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Get second file
    auto picker = ref new FileOpenPicker();
    picker->SuggestedStartLocation = PickerLocationId::VideosLibrary;
    picker->FileTypeFilter->Append(".mp4");
    
    create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ videoFile)
    {
        if (videoFile == nullptr)
        {
            rootPage->NotifyUser("File picking cancelled", NotifyType::ErrorMessage);
            return;
        }

        this->secondVideoFile = videoFile;
        create_task(this->secondVideoFile->OpenReadAsync()).then([this](IRandomAccessStreamWithContentType^ videoSource)
        {
            mediaElement->SetSource(videoSource, this->secondVideoFile->ContentType);
            appendFiles->IsEnabled = true;
        });
    });
}

void Scenario2_AppendClips::AppendVideos_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Combine two video files together into one
    create_task(MediaClip::CreateFromFileAsync(firstVideoFile)).then([this](MediaClip^ firstClip)
    {
        create_task(MediaClip::CreateFromFileAsync(secondVideoFile)).then([this, firstClip](MediaClip^ secondClip)
        {
            composition = ref new MediaComposition();
            composition->Clips->Append(firstClip);
            composition->Clips->Append(secondClip);

            // Render to MediaElement
            mediaStreamSource = composition->GeneratePreviewMediaStreamSource((int)mediaElement->ActualWidth, (int)mediaElement->ActualHeight);
            mediaElement->SetMediaStreamSource(mediaStreamSource);
            rootPage->NotifyUser("Clips appended", NotifyType::StatusMessage);
        });
    });
}