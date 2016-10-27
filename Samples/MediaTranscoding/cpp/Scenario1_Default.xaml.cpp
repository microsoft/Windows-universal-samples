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
#include "Scenario1_Default.xaml.h"

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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1_Default::Scenario1_Default()
{
    InitializeComponent();

    // Hook up UI
    PickFileButton->Click += ref new RoutedEventHandler(this, &Scenario1_Default::PickFile);
    SetOutputButton->Click += ref new RoutedEventHandler(this, &Scenario1_Default::PickOutput);
    TargetFormat->SelectionChanged += ref new SelectionChangedEventHandler(this, &Scenario1_Default::OnTargetFormatChanged);
    Transcode->Click += ref new RoutedEventHandler(this, &Scenario1_Default::TranscodePreset);
    Cancel->Click += ref new RoutedEventHandler(this, &Scenario1_Default::TranscodeCancel);

    // Media Controls
    InputPlayButton->Click += ref new RoutedEventHandler(this, &Scenario1_Default::InputPlayButton_Click);
    InputPauseButton->Click += ref new RoutedEventHandler(this, &Scenario1_Default::InputPauseButton_Click);
    InputStopButton->Click += ref new RoutedEventHandler(this, &Scenario1_Default::InputStopButton_Click);
    OutputPlayButton->Click += ref new RoutedEventHandler(this, &Scenario1_Default::OutputPlayButton_Click);
    OutputPauseButton->Click += ref new RoutedEventHandler(this, &Scenario1_Default::OutputPauseButton_Click);
    OutputStopButton->Click += ref new RoutedEventHandler(this, &Scenario1_Default::OutputStopButton_Click);

    // File is not selected, disable all buttons but PickFileButton
    DisableButtons();
    SetPickFileButton(true);
    SetCancelButton(false);

    // Initialize Objects
    _Transcoder = ref new MediaTranscoder();
    _Profile = nullptr;
    _InputFile = nullptr;
    _OutputFile = nullptr;
    _OutputFileName = "TranscodeSampleOutput.mp4";
    _CTS = cancellation_token_source();

    // Initial values
    _OutputType = "MP4";
    _OutputFileExtension = ".mp4";
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1_Default::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in Presets such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void Scenario1_Default::GetPresetProfile(ComboBox^ comboBox)
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

void Scenario1_Default::TranscodePreset(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopPlayers();
    DisableButtons();
    GetPresetProfile(ProfileSelect);

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

void Scenario1_Default::TranscodeCancel(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void Scenario1_Default::TranscodeProgress(IAsyncActionWithProgress<double>^ asyncInfo, double percent)
{
    OutputMsg->Foreground = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Green);
    OutputMsg->Text = "Progress:  " + ((int)percent).ToString() + "%";
}

void Scenario1_Default::TranscodeError(Platform::String^ error)
{
    StatusMessage->Foreground = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Red);
    StatusMessage->Text = error;
    EnableButtons();
    SetCancelButton(false);
}

void Scenario1_Default::TranscodeFailure(TranscodeFailureReason reason)
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

void Scenario1_Default::SetCancelButton(bool isEnabled)
{
    Cancel->IsEnabled = isEnabled;
}

void Scenario1_Default::EnableButtons()
{
    PickFileButton->IsEnabled = true;
    SetOutputButton->IsEnabled = true;
    TargetFormat->IsEnabled = true;
    ProfileSelect->IsEnabled = true;
    EnableMrfCrf444->IsEnabled = true;


}

void Scenario1_Default::DisableButtons()
{
    ProfileSelect->IsEnabled = false;
    Transcode->IsEnabled = false;
    PickFileButton->IsEnabled = false;
    SetOutputButton->IsEnabled = false;
    TargetFormat->IsEnabled = false;
    EnableMrfCrf444->IsEnabled = false;
}

void Scenario1_Default::EnableNonSquarePARProfiles()
{
    ComboBoxItem_NTSC->IsEnabled = true;
    ComboBoxItem_PAL->IsEnabled = true;
}

void Scenario1_Default::DisableNonSquarePARProfiles()
{
    ComboBoxItem_NTSC->IsEnabled = false;
    ComboBoxItem_PAL->IsEnabled = false;

    // Ensure a valid profile is set
    if ((ProfileSelect->SelectedIndex == 3) || (ProfileSelect->SelectedIndex == 4))
    {
        ProfileSelect->SelectedIndex = 2;
    }
}

void Scenario1_Default::PickFile(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void Scenario1_Default::PickOutput(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void Scenario1_Default::OnTargetFormatChanged(Object^ sender, SelectionChangedEventArgs^ e)
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

void Scenario1_Default::InputPlayButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (InputVideo->DefaultPlaybackRate == 0)
    {
        InputVideo->DefaultPlaybackRate = 1.0;
        InputVideo->PlaybackRate = 1.0;
    }

    InputVideo->Play();
}

void Scenario1_Default::InputStopButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InputVideo->Stop();
}

void Scenario1_Default::InputPauseButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InputVideo->Pause();
}

void Scenario1_Default::OutputPlayButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (OutputVideo->DefaultPlaybackRate == 0)
    {
        OutputVideo->DefaultPlaybackRate = 1.0;
        OutputVideo->PlaybackRate = 1.0;
    }

    OutputVideo->Play();
}

void Scenario1_Default::OutputStopButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    OutputVideo->Stop();
}

void Scenario1_Default::OutputPauseButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    OutputVideo->Pause();
}

void Scenario1_Default::SetPickFileButton(bool isEnabled)
{
    PickFileButton->IsEnabled = isEnabled;
}

void Scenario1_Default::SetTranscodeButton(bool isEnabled)
{
    Transcode->IsEnabled = isEnabled;
}

void Scenario1_Default::StopPlayers()
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

void Scenario1_Default::PlayFile(Windows::Storage::StorageFile^ MediaFile)
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
