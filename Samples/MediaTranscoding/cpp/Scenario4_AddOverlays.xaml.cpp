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
#include "Scenario4_AddOverlays.xaml.h"

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

Scenario4_AddOverlays::Scenario4_AddOverlays() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario4_AddOverlays::OnNavigatedFrom(NavigationEventArgs^ e)
{
    mediaElement->Source = nullptr;
    mediaStreamSource = nullptr;
}

void Scenario4_AddOverlays::ChooseBaseVideo_Click(Object^ sender, RoutedEventArgs^ e)
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

        this->baseVideoFile = videoFile;
        create_task(this->baseVideoFile->OpenReadAsync()).then([this](IRandomAccessStreamWithContentType^ videoSource)
        {
            mediaElement->SetSource(videoSource, baseVideoFile->ContentType);
            chooseOverlayVideo->IsEnabled = true;

            rootPage->NotifyUser("Video file added", NotifyType::StatusMessage);
        });
    });
}

void Scenario4_AddOverlays::ChooseOverlayVideo_Click(Object^ sender, RoutedEventArgs^ e)
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

        this->overlayVideoFile = videoFile;

        CreateOverlays();
        rootPage->NotifyUser("Overlay video chosen", NotifyType::StatusMessage);
    });
}

void Scenario4_AddOverlays::CreateOverlays()
{
    create_task(MediaClip::CreateFromFileAsync(baseVideoFile)).then([this](MediaClip^ baseClip)
    {
        composition = ref new MediaComposition();
        composition->Clips->Append(baseClip);

        create_task(MediaClip::CreateFromFileAsync(overlayVideoFile)).then([this](MediaClip^ overlayClip)
        {
            // Overlay video in upper left corner, retain its native aspect ratio
            Rect videoOverlayPosition;
            auto encodingProperties = overlayClip->GetVideoEncodingProperties();
            videoOverlayPosition.Height = (float)(mediaElement->ActualHeight / 3);
            videoOverlayPosition.Width = (float)encodingProperties->Width / (float)encodingProperties->Height * videoOverlayPosition.Height;
            videoOverlayPosition.X = 0;
            videoOverlayPosition.Y = 0;

            auto videoOverlay = ref new MediaOverlay(overlayClip);
            videoOverlay->Position = videoOverlayPosition;
            videoOverlay->Opacity = 0.75;

            auto overlayLayer = ref new MediaOverlayLayer();
            overlayLayer->Overlays->Append(videoOverlay);
            composition->OverlayLayers->Append(overlayLayer);

            // Render to MediaElement
            mediaStreamSource = composition->GeneratePreviewMediaStreamSource((int)mediaElement->ActualWidth, (int)mediaElement->ActualHeight);
            mediaElement->SetMediaStreamSource(mediaStreamSource);
            rootPage->NotifyUser("Overlays created", NotifyType::StatusMessage);
        });
    });
}
