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
#include "Scenario2_Custom.xaml.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Media;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Transcoding;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage::Streams;
using namespace concurrency;
using namespace SDKTemplate;

Scenario2_Custom::Scenario2_Custom()
{
    InitializeComponent();

    // Hook up the UI
    PickFileButton->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::PickFile);
    SetOutputButton->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::PickOutput);
    TargetFormat->SelectionChanged += ref new SelectionChangedEventHandler(this, &Scenario2_Custom::OnTargetFormatChanged);
    Transcode->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::TranscodeCustom);
    Cancel->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::TranscodeCancel);

    // Media Controls
    InputPlayButton->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::InputPlayButton_Click);
    InputPauseButton->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::InputPauseButton_Click);
    InputStopButton->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::InputStopButton_Click);
    OutputPlayButton->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::OutputPlayButton_Click);
    OutputPauseButton->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::OutputPauseButton_Click);
    OutputStopButton->Click += ref new RoutedEventHandler(this, &Scenario2_Custom::OutputStopButton_Click);

    // File is not selected, disable all buttons but PickFileButton
    DisableButtons();
    SetPickFileButton(true);
    SetCancelButton(false);
    SetOutputFileButton(false);

    // Initialize Objects
    _Transcoder = ref new MediaTranscoder();
    _Profile = nullptr;
    _InputFile = nullptr;
    _OutputFile = nullptr;
    _OutputFileName = "TranscodeSampleOutput";
    _OutputFileExtension = ".mp4";
    _OutputType = "MP4";
    _CTS = cancellation_token_source();

    // Initialize UI with default settings
    MediaEncodingProfile^ defaultProfile = MediaEncodingProfile::CreateMp4(VideoEncodingQuality::Wvga);
    VideoW->Text = defaultProfile->Video->Width.ToString();
    VideoH->Text = defaultProfile->Video->Height.ToString();
    VideoBR->Text = defaultProfile->Video->Bitrate.ToString();
    VideoFR->Text = defaultProfile->Video->FrameRate->Numerator.ToString();
    AudioBPS->Text = defaultProfile->Audio->BitsPerSample.ToString();
    AudioCC->Text = defaultProfile->Audio->ChannelCount.ToString();
    AudioBR->Text = defaultProfile->Audio->Bitrate.ToString();
    AudioSR->Text = defaultProfile->Audio->SampleRate.ToString();

    _UseMp4 = true;
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2_Custom::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void Scenario2_Custom::GetCustomProfile()
{
    if (_UseMp4)
    {
        _Profile = MediaEncodingProfile::CreateMp4(VideoEncodingQuality::Wvga);
    }
    else
    {
        _Profile = MediaEncodingProfile::CreateWmv(VideoEncodingQuality::Wvga);
    }

    // TryParse() is not available. Using _wtoi() with #include <stdlib.h>
    try
    {
        _Profile->Video->Width = _wtoi(VideoW->Text->Data());
        _Profile->Video->Height = _wtoi(VideoH->Text->Data());
        _Profile->Video->Bitrate = _wtoi(VideoBR->Text->Data());
        _Profile->Video->FrameRate->Numerator = _wtoi(VideoFR->Text->Data());
        _Profile->Video->FrameRate->Denominator = 1;
        _Profile->Audio->BitsPerSample = _wtoi(AudioBPS->Text->Data());
        _Profile->Audio->ChannelCount = _wtoi(AudioCC->Text->Data());
        _Profile->Audio->Bitrate = _wtoi(AudioBR->Text->Data());
        _Profile->Audio->SampleRate = _wtoi(AudioSR->Text->Data());
    }
    catch (Exception^ exception)
    {
        TranscodeError(exception->Message);
    }
}

void Scenario2_Custom::TranscodeCustom(Object^ sender, RoutedEventArgs^ e)
{
    StopPlayers();
    DisableButtons();
    GetCustomProfile();

    if ((EnableMrfCrf444->IsChecked != nullptr) && (EnableMrfCrf444->IsChecked->Value))
    {
        _Transcoder->VideoProcessingAlgorithm = Windows::Media::Transcoding::MediaVideoProcessingAlgorithm::MrfCrf444;
    }
    else
    {
        _Transcoder->VideoProcessingAlgorithm = Windows::Media::Transcoding::MediaVideoProcessingAlgorithm::Default;
    }

    // Clear messages
    StatusMessage->Text = "";

    try
    {
        if ((_InputFile != nullptr) && (_OutputFile != nullptr))
        {
            create_task(_Transcoder->PrepareFileTranscodeAsync(_InputFile, _OutputFile, _Profile)).then(
                [this](PrepareTranscodeResult^ transcode)
            {
                try
                {
                    if (transcode->CanTranscode)
                    {
                        OutputMsg->Text = "";
                        SetCancelButton(true);
                        Windows::Foundation::IAsyncActionWithProgress<double>^ transcodeOp = transcode->TranscodeAsync();

                        // Set Progress Callback
                        transcodeOp->Progress = ref new AsyncActionProgressHandler<double>(
                            [this](IAsyncActionWithProgress<double>^ asyncInfo, double percent) {
                            TranscodeProgress(asyncInfo, percent);
                        }, Platform::CallbackContext::Same);

                        return transcodeOp;
                    }
                    else
                    {
                        TranscodeFailure(transcode->FailureReason);
                    }
                }
                catch (Platform::Exception^ exception)
                {
                    TranscodeError(exception->Message);
                }

                cancel_current_task();
            }).then(
                [this](task<void> transcodeTask)
            {
                try
                {
                    transcodeTask.get();
                    OutputMsg->Text = "Transcode Completed.";
                    OutputPath->Text = "Output (" + _OutputFile->Path + ")";
                    PlayFile(_OutputFile);
                }
                catch (task_canceled&)
                {
                    OutputMsg->Foreground = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Red);
                    OutputMsg->Text = "Transcode Cancelled.";
                }
                catch (Exception^ exception)
                {
                    TranscodeError(exception->Message);
                }

                EnableButtons();
                SetCancelButton(false);
            });
        }
    }
    catch (Platform::Exception^ exception)
    {
        TranscodeError(exception->Message);
    }
}

void Scenario2_Custom::TranscodeCancel(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        _CTS.cancel();
        _CTS = cancellation_token_source();

        if (_OutputFile != nullptr)
        {
            create_task(_OutputFile->DeleteAsync()).then(
                [this](task<void> deleteTask)
            {
                try
                {
                    deleteTask.get();
                }
                catch (Platform::Exception^ exception)
                {
                    TranscodeError(exception->Message);
                }
            });
        }
    }
    catch (Platform::Exception^ exception)
    {
        TranscodeError(exception->Message);
    }
}

void Scenario2_Custom::TranscodeProgress(IAsyncActionWithProgress<double>^ asyncInfo, double percent)
{
    OutputMsg->Foreground = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Green);
    OutputMsg->Text = "Progress:  " + ((int)percent).ToString() + "%";
}

void Scenario2_Custom::TranscodeError(Platform::String^ error)
{
    StatusMessage->Foreground = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Red);
    StatusMessage->Text = error;
    EnableButtons();
    SetCancelButton(false);
}

void Scenario2_Custom::TranscodeFailure(TranscodeFailureReason reason)
{
    try
    {
        if (_OutputFile != nullptr)
        {
            create_task(_OutputFile->DeleteAsync()).then(
                [this](task<void> deleteTask)
            {
                try
                {
                    deleteTask.get();
                }
                catch (Platform::Exception^ exception)
                {
                    TranscodeError(exception->Message);
                }
            });
        }
    }
    catch (Platform::Exception^ exception)
    {
        TranscodeError(exception->Message);
    }

    switch (reason)
    {
    case TranscodeFailureReason::CodecNotFound:
        TranscodeError("Codec not found.");
        break;
    case TranscodeFailureReason::InvalidProfile:
        TranscodeError("Invalid profile.");
        break;
    default:
        TranscodeError("Unknown failure.");
    }
}

void Scenario2_Custom::SetCancelButton(bool isEnabled)
{
    Cancel->IsEnabled = isEnabled;
}

void Scenario2_Custom::EnableButtons()
{
    PickFileButton->IsEnabled = true;
    SetOutputButton->IsEnabled = true;
    TargetFormat->IsEnabled = true;
    EnableMrfCrf444->IsEnabled = true;

    // The transcode button's initial state should be disabled until an output
    // file has been set.
    Transcode->IsEnabled = false;
}

void Scenario2_Custom::DisableButtons()
{
    PickFileButton->IsEnabled = false;
    SetOutputButton->IsEnabled = false;
    TargetFormat->IsEnabled = false;
    Transcode->IsEnabled = false;
    EnableMrfCrf444->IsEnabled = false;
}

void Scenario2_Custom::PickFile(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        FileOpenPicker^ picker = ref new FileOpenPicker();
        picker->SuggestedStartLocation = PickerLocationId::VideosLibrary;
        picker->FileTypeFilter->Append(".wmv");
        picker->FileTypeFilter->Append(".mp4");

        create_task(picker->PickSingleFileAsync()).then(
            [this](StorageFile^ inputFile)
        {
            try
            {
                _InputFile = inputFile;

                if (_InputFile != nullptr)
                {
                    return inputFile->OpenAsync(FileAccessMode::Read);
                }
            }
            catch (Platform::Exception^ exception)
            {
                TranscodeError(exception->Message);
            }

            cancel_current_task();
        }).then(
            [this](IRandomAccessStream^ inputStream)
        {
            try
            {
                InputVideo->SetSource(inputStream, _InputFile->ContentType);;
                InputVideo->Play();

                //Enable buttons
                EnableButtons();
            }
            catch (Platform::Exception^ exception)
            {
                TranscodeError(exception->Message);
            }
        });
    }
    catch (Exception^ exception)
    {
        TranscodeError(exception->Message);
    }
}

void Scenario2_Custom::PickOutput(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        FileSavePicker^ picker = ref new FileSavePicker();
        picker->SuggestedStartLocation = PickerLocationId::VideosLibrary;
        picker->SuggestedFileName = _OutputFileName;

        auto extensions = ref new Platform::Collections::Vector<String^>();
        extensions->Append(_OutputFileExtension);
        picker->FileTypeChoices->Insert(_OutputType, extensions);

        create_task(picker->PickSaveFileAsync()).then([this](StorageFile^ file) {
            if (file != nullptr)
            {
                _OutputFile = file;
                SetTranscodeButton(true);
            }
        });
    }
    catch (Exception^ exception)
    {
        TranscodeError(exception->Message);
    }
}

void Scenario2_Custom::OnTargetFormatChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    if (TargetFormat->SelectedIndex > 0)
    {
        _OutputFileExtension = ".wmv";
        _OutputType = "WMV";
        _UseMp4 = false;
    }
    else
    {
        _OutputFileExtension = ".mp4";
        _OutputType = "MP4";
        _UseMp4 = true;
    }
}

void Scenario2_Custom::InputPlayButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (InputVideo->DefaultPlaybackRate == 0)
    {
        InputVideo->DefaultPlaybackRate = 1.0;
        InputVideo->PlaybackRate = 1.0;
    }

    InputVideo->Play();
}

void Scenario2_Custom::InputStopButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InputVideo->Stop();
}

void Scenario2_Custom::InputPauseButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InputVideo->Pause();
}

void Scenario2_Custom::OutputPlayButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (OutputVideo->DefaultPlaybackRate == 0)
    {
        OutputVideo->DefaultPlaybackRate = 1.0;
        OutputVideo->PlaybackRate = 1.0;
    }

    OutputVideo->Play();
}

void Scenario2_Custom::OutputStopButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    OutputVideo->Stop();
}

void Scenario2_Custom::OutputPauseButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    OutputVideo->Pause();
}

void Scenario2_Custom::SetPickFileButton(bool isEnabled)
{
    PickFileButton->IsEnabled = isEnabled;
}

void Scenario2_Custom::SetOutputFileButton(bool isEnabled)
{
    SetOutputButton->IsEnabled = isEnabled;
}

void Scenario2_Custom::SetTranscodeButton(bool isEnabled)
{
    Transcode->IsEnabled = isEnabled;
}


void Scenario2_Custom::StopPlayers()
{
    if (InputVideo->CurrentState != Media::MediaElementState::Paused)
    {
        InputVideo->Pause();
    }
    if (OutputVideo->CurrentState != Media::MediaElementState::Paused)
    {
        OutputVideo->Pause();
    }
}

void Scenario2_Custom::PlayFile(Windows::Storage::StorageFile^ MediaFile)
{
    try
    {
        create_task(MediaFile->OpenAsync(FileAccessMode::Read)).then(
            [&, this, MediaFile](IRandomAccessStream^ outputStream)
        {
            try
            {
                OutputVideo->SetSource(outputStream, MediaFile->ContentType);
                OutputVideo->Play();
            }
            catch (Platform::Exception^ exception)
            {
                TranscodeError(exception->Message);
            }
        });
    }
    catch (Exception^ exception)
    {
        TranscodeError(exception->Message);
    }
}