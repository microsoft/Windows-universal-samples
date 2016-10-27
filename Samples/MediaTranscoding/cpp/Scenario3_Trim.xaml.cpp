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
#include "Scenario3_Trim.xaml.h"

using namespace SDKTemplate;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage::Streams;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Media;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Transcoding;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace concurrency;

static const long c_TickToSec = (1000 * 1000 * 10);

Scenario3_Trim::Scenario3_Trim()
{
    InitializeComponent();

    // Hook up UI
    PickFileButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::PickFile);
    SetOutputButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::PickOutput);
    TargetFormat->SelectionChanged += ref new SelectionChangedEventHandler(this, &Scenario3_Trim::OnTargetFormatChanged);
    Transcode->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::TranscodeTrim);
    Cancel->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::TranscodeCancel);
    MarkInButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::MarkIn);
    MarkOutButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::MarkOut);

    // Disable manual manipulation of trim points
    StartTimeText->Text = ((float)(_start.Duration) / c_TickToSec).ToString();
    EndTimeText->Text = ((float)(_stop.Duration) / c_TickToSec).ToString();
    StartTimeText->IsEnabled = false;
    EndTimeText->IsEnabled = false;


    // Media Controls
    InputPlayButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::InputPlayButton_Click);
    InputPauseButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::InputPauseButton_Click);
    InputStopButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::InputStopButton_Click);
    OutputPlayButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::OutputPlayButton_Click);
    OutputPauseButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::OutputPauseButton_Click);
    OutputStopButton->Click += ref new RoutedEventHandler(this, &Scenario3_Trim::OutputStopButton_Click);

    // File is not selected, disable all buttons but PickFileButton
    DisableButtons();
    SetPickFileButton(true);
    SetOutputFileButton(true);
    SetCancelButton(false);

    // Initialize Objects
    _Transcoder = ref new MediaTranscoder();
    _Profile = nullptr;
    _InputFile = nullptr;
    _OutputFile = nullptr;
    _OutputFileName = "TranscodeSampleOutput";

    _OutputType = "MP4";
    _OutputFileExtension = ".mp4";
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3_Trim::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in Presets such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void Scenario3_Trim::GetPresetProfile(ComboBox^ comboBox)
{
    _Profile = nullptr;
    VideoEncodingQuality videoEncodingProfile = VideoEncodingQuality::Wvga;
    switch (comboBox->SelectedIndex)
    {
    case 0:
        videoEncodingProfile = VideoEncodingQuality::HD1080p;
        break;
    case 1:
        videoEncodingProfile = VideoEncodingQuality::HD720p;
        break;
    case 2:
        videoEncodingProfile = VideoEncodingQuality::Wvga;
        break;
    case 3:
        videoEncodingProfile = VideoEncodingQuality::Ntsc;
        break;
    case 4:
        videoEncodingProfile = VideoEncodingQuality::Pal;
        break;
    case 5:
        videoEncodingProfile = VideoEncodingQuality::Vga;
        break;
    case 6:
        videoEncodingProfile = VideoEncodingQuality::Qvga;
        break;
    }

    if (_OutputType == "AVI")
    {
        _Profile = MediaEncodingProfile::CreateAvi(videoEncodingProfile);
    }
    else if (_OutputType == "WMV")
    {
        _Profile = MediaEncodingProfile::CreateWmv(videoEncodingProfile);
    }
    else
    {
        _Profile = MediaEncodingProfile::CreateMp4(videoEncodingProfile);
    }
    /*
    For transcoding to audio profiles, create the encoding profile using one of these APIs:
    MediaEncodingProfile::CreateMp3(audioEncodingProfile)
    MediaEncodingProfile::CreateM4a(audioEncodingProfile)
    MediaEncodingProfile::CreateWma(audioEncodingProfile)
    MediaEncodingProfile::CreateWav(audioEncodingProfile)

    where audioEncodingProfile is one of these presets:
    AudioEncodingQuality::High
    AudioEncodingQuality::Medium
    AudioEncodingQuality::Low
    */
}

void Scenario3_Trim::MarkIn(Object^ sender, RoutedEventArgs^ e)
{
    _start = InputVideo->Position;
    StartTimeText->Text = ((float)(_start.Duration) / c_TickToSec).ToString();
    // Make sure end time is after start time.
    if (_start.Duration > _stop.Duration || (_start.Duration == _stop.Duration && _start.Duration != 0))
    {
        _stop.Duration = _start.Duration + c_TickToSec;
        EndTimeText->Text = ((float)(_stop.Duration) / c_TickToSec).ToString();
    }
}

void Scenario3_Trim::MarkOut(Object^ sender, RoutedEventArgs^ e)
{
    _stop = InputVideo->Position;
    EndTimeText->Text = ((float)(_stop.Duration) / c_TickToSec).ToString();
    // Make sure end time is after start time.
    if ((_start.Duration > _stop.Duration) || (_start.Duration == _stop.Duration && _start.Duration != 0))
    {
        if (_stop.Duration > c_TickToSec)
        {
            _start.Duration = _stop.Duration - c_TickToSec;
        }
        else
        {
            _start.Duration = 0;
        }
        StartTimeText->Text = ((float)(_start.Duration) / c_TickToSec).ToString();
    }
}

void Scenario3_Trim::TranscodeTrim(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopPlayers();
    DisableButtons();
    GetPresetProfile(ProfileSelect);
    _Transcoder->TrimStartTime = _start;
    _Transcoder->TrimStopTime = _stop;

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

void Scenario3_Trim::TranscodeCancel(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void Scenario3_Trim::TranscodeProgress(IAsyncActionWithProgress<double>^ asyncInfo, double percent)
{
    OutputMsg->Foreground = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Green);
    OutputMsg->Text = "Progress:  " + ((int)percent).ToString() + "%";
}

void Scenario3_Trim::TranscodeError(Platform::String^ error)
{
    StatusMessage->Foreground = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Red);
    StatusMessage->Text = error;
    EnableButtons();
    SetCancelButton(false);
}

void Scenario3_Trim::TranscodeFailure(TranscodeFailureReason reason)
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

void Scenario3_Trim::SetCancelButton(bool isEnabled)
{
    Cancel->IsEnabled = isEnabled;
}

void Scenario3_Trim::EnableButtons()
{
    PickFileButton->IsEnabled = true;
    SetOutputButton->IsEnabled = true;
    TargetFormat->IsEnabled = true;
    ProfileSelect->IsEnabled = true;
    EnableMrfCrf444->IsEnabled = true;

    // The transcode button's initial state should be disabled until an output
    // file has been set.
    Transcode->IsEnabled = false;
}

void Scenario3_Trim::DisableButtons()
{
    ProfileSelect->IsEnabled = false;
    Transcode->IsEnabled = false;
    PickFileButton->IsEnabled = false;
    SetOutputButton->IsEnabled = false;
    TargetFormat->IsEnabled = false;
    EnableMrfCrf444->IsEnabled = false;
}

void Scenario3_Trim::EnableNonSquarePARProfiles()
{
    ComboBoxItem_NTSC->IsEnabled = true;
    ComboBoxItem_PAL->IsEnabled = true;
}

void Scenario3_Trim::DisableNonSquarePARProfiles()
{
    ComboBoxItem_NTSC->IsEnabled = false;
    ComboBoxItem_PAL->IsEnabled = false;

    // Ensure a valid profile is set
    if ((ProfileSelect->SelectedIndex == 3) || (ProfileSelect->SelectedIndex == 4))
    {
        ProfileSelect->SelectedIndex = 2;
    }
}

void Scenario3_Trim::PickFile(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void Scenario3_Trim::PickOutput(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void Scenario3_Trim::OnTargetFormatChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    switch (TargetFormat->SelectedIndex)
    {
    case 1:
        _OutputFileExtension = ".wmv";
        _OutputType = "WMV";
        EnableNonSquarePARProfiles();
        break;
    case 2:
        _OutputFileExtension = ".avi";
        _OutputType = "AVI";

        // Disable NTSC and PAL profiles as non-square pixel aspect ratios are not supported by AVI
        DisableNonSquarePARProfiles();
        break;
    default:
        _OutputFileExtension = ".mp4";
        _OutputType = "MP4";
        EnableNonSquarePARProfiles();
        break;
    }
}

void Scenario3_Trim::InputPlayButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (InputVideo->DefaultPlaybackRate == 0)
    {
        InputVideo->DefaultPlaybackRate = 1.0;
        InputVideo->PlaybackRate = 1.0;
    }

    InputVideo->Play();
}

void Scenario3_Trim::InputStopButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InputVideo->Stop();
}

void Scenario3_Trim::InputPauseButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InputVideo->Pause();
}

void Scenario3_Trim::OutputPlayButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (OutputVideo->DefaultPlaybackRate == 0)
    {
        OutputVideo->DefaultPlaybackRate = 1.0;
        OutputVideo->PlaybackRate = 1.0;
    }

    OutputVideo->Play();
}

void Scenario3_Trim::OutputStopButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    OutputVideo->Stop();
}

void Scenario3_Trim::OutputPauseButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    OutputVideo->Pause();
}

void Scenario3_Trim::SetPickFileButton(bool isEnabled)
{
    PickFileButton->IsEnabled = isEnabled;
}

void Scenario3_Trim::SetOutputFileButton(bool isEnabled)
{
    SetOutputButton->IsEnabled = isEnabled;
}

void Scenario3_Trim::SetTranscodeButton(bool isEnabled)
{
    Transcode->IsEnabled = isEnabled;
}

void Scenario3_Trim::StopPlayers()
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

void Scenario3_Trim::PlayFile(Windows::Storage::StorageFile^ MediaFile)
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
