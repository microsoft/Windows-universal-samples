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
#include "Scenario1_TrimAndSaveClip.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Media::Core;
using namespace Windows::Media::Editing;
using namespace Windows::Media::Transcoding;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::AccessCache;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1_TrimAndSaveClip::Scenario1_TrimAndSaveClip() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario1_TrimAndSaveClip::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Make sure we don't run out of entries in StoreItemAccessList.
    // As we don't need to persist this across app sessions/pages, clearing
    // every time is sufficient for this sample
    storageItemAccessList = StorageApplicationPermissions::FutureAccessList;
    storageItemAccessList->Clear();
}

void Scenario1_TrimAndSaveClip::OnNavigatedFrom(NavigationEventArgs^ e)
{
    mediaElement->Source = nullptr;
    mediaStreamSource = nullptr;
}

void Scenario1_TrimAndSaveClip::ChooseFile_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Get file
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

        // These files could be picked from a location that we won't have access to later
        // (especially if persisting the MediaComposition to disk and loading it later). 
        // Use the StorageItemAccessList in order to keep access permissions to that
        // file for later use. Be aware that this access list needs to be cleared
        // periodically or the app will run out of entries.
        storageItemAccessList->Add(this->pickedFile);

        create_task(this->pickedFile->OpenReadAsync()).then([this](IRandomAccessStreamWithContentType^ videoSource)
        {
            mediaElement->SetSource(videoSource, this->pickedFile->ContentType);
            trimClip->IsEnabled = true;
        });
    });
}

void Scenario1_TrimAndSaveClip::TrimClip_Click(Object^ sender, RoutedEventArgs^ e)
{
    create_task(MediaClip::CreateFromFileAsync(this->pickedFile)).then([this](MediaClip^ clip)
    {
        // Trim the front and back 25% from the clip
        TimeSpan trimFromStart;
        trimFromStart.Duration = (long long)(clip->OriginalDuration.Duration * 0.25);
        clip->TrimTimeFromStart = trimFromStart;
        TimeSpan trimFromEnd;
        trimFromEnd.Duration = (long long)(clip->OriginalDuration.Duration * 0.25);
        clip->TrimTimeFromEnd = trimFromEnd;

        // Create a MediaComposition containing the clip and set it on the MediaElement.
        composition = ref new MediaComposition();
        composition->Clips->Append(clip);
        mediaStreamSource = composition->GeneratePreviewMediaStreamSource((int)mediaElement->ActualWidth, (int)mediaElement->ActualHeight);
        mediaElement->SetMediaStreamSource(mediaStreamSource);
        rootPage->NotifyUser("Clip trimmed", NotifyType::StatusMessage);
        save->IsEnabled = true;
    });
}

void Scenario1_TrimAndSaveClip::Save_Click(Object^ sender, RoutedEventArgs^ e)
{
    EnableButtons(false);
    rootPage->NotifyUser("Requesting file to save to", NotifyType::StatusMessage);

    auto picker = ref new FileSavePicker();
    picker->SuggestedStartLocation = PickerLocationId::VideosLibrary;

    auto filter = ref new Platform::Collections::Vector<String^>();
    filter->Append(".mp4");
    picker->FileTypeChoices->Insert("MP4 files", filter);
    picker->SuggestedFileName = "TrimmedClip.mp4";

    create_task(picker->PickSaveFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            auto saveOperation = composition->RenderToFileAsync(file, MediaTrimmingPreference::Precise);
            saveOperation->Progress = ref new AsyncOperationProgressHandler<TranscodeFailureReason, double>([this](
                IAsyncOperationWithProgress<TranscodeFailureReason, double>^ info, double value)
            {
                create_task(this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this,info,value]() 
                {
                    rootPage->NotifyUser("Saving file... Progress: " + value.ToString() + "%", NotifyType::StatusMessage);
                }))).wait();
            });

            saveOperation->Completed = ref new AsyncOperationWithProgressCompletedHandler<TranscodeFailureReason, double>([this](
                IAsyncOperationWithProgress<TranscodeFailureReason, double>^ info, AsyncStatus status)
            {
                create_task(this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, info, status]()
                {
                    auto results = info->GetResults();
                    if (results != TranscodeFailureReason::None || status != AsyncStatus::Completed)
                    {
                        rootPage->NotifyUser("Saving was unsuccessful", NotifyType::ErrorMessage);
                    }
                    else
                    {
                        rootPage->NotifyUser("Trimmed clip saved to file", NotifyType::StatusMessage);
                    }

                    EnableButtons(true);
                }))).wait();
            });
        }
        else
        {
            rootPage->NotifyUser("User cancelled the file selection", NotifyType::StatusMessage);
            EnableButtons(true);
        }
    });
}


void Scenario1_TrimAndSaveClip::EnableButtons(bool isEnabled)
{
    chooseFile->IsEnabled = isEnabled;
    save->IsEnabled = isEnabled;
    trimClip->IsEnabled = isEnabled;
}