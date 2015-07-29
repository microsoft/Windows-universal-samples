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
#include "Scenario3_AddAudioTracks.xaml.h"

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

Scenario3_AddAudioTracks::Scenario3_AddAudioTracks() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario3_AddAudioTracks::OnNavigatedFrom(NavigationEventArgs^ e)
{
    mediaElement->Source = nullptr;
    mediaStreamSource = nullptr;
}

void Scenario3_AddAudioTracks::ChooseVideo_Click(Object^ sender, RoutedEventArgs^ e)
{
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

        this->pickedFile = videoFile;
        create_task(this->pickedFile->OpenReadAsync()).then([this](IRandomAccessStreamWithContentType^ videoSource)
        {
            mediaElement->SetSource(videoSource, pickedFile->ContentType);
            addAudio->IsEnabled = true;

            rootPage->NotifyUser("Video file added", NotifyType::StatusMessage);
        });
    });
}

void Scenario3_AddAudioTracks::AddAudio_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Create the original MediaComposition
    create_task(MediaClip::CreateFromFileAsync(pickedFile)).then([this](MediaClip^ clip)
    {
        composition = ref new MediaComposition();
        composition->Clips->Append(clip);

        // Add background audio
        auto picker = ref new FileOpenPicker();
        picker->SuggestedStartLocation = PickerLocationId::MusicLibrary;
        picker->FileTypeFilter->Append(".mp3");
        picker->FileTypeFilter->Append(".wav");
        picker->FileTypeFilter->Append(".flac");
        
        create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ audioFile)
        {
            if (audioFile == nullptr)
            {
                rootPage->NotifyUser("File picking cancelled", NotifyType::ErrorMessage);
                return;
            }

            create_task(BackgroundAudioTrack::CreateFromFileAsync(audioFile)).then([this](BackgroundAudioTrack^ backgroundTrack)
            {
                composition->BackgroundAudioTracks->Append(backgroundTrack);

                // Render to MediaElement
                mediaStreamSource = composition->GeneratePreviewMediaStreamSource((int)mediaElement->ActualWidth, (int)mediaElement->ActualHeight);
                mediaElement->SetMediaStreamSource(mediaStreamSource);

                rootPage->NotifyUser("Background audio added", NotifyType::StatusMessage);
            });
        });
    });
}
