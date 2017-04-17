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

using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Media.MediaProperties;
using Windows.Media.Transcoding;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_Trim : Page
    {
        MainPage rootPage = MainPage.Current;

        Windows.UI.Core.CoreDispatcher _dispatcher = Window.Current.Dispatcher;
        CancellationTokenSource _cts;
        string _OutputFileName = "TranscodeSampleOutput";
        Windows.Media.MediaProperties.MediaEncodingProfile _Profile;
        Windows.Storage.StorageFile _InputFile = null;
        Windows.Storage.StorageFile _OutputFile = null;
        Windows.Media.Transcoding.MediaTranscoder _Transcoder = new Windows.Media.Transcoding.MediaTranscoder();
        string _OutputType = "MP4";
        string _OutputFileExtension = ".mp4";

        TimeSpan _Start = new TimeSpan(0);
        TimeSpan _Stop = new TimeSpan(0);
        TimeSpan _c_sec0 = new TimeSpan(0, 0, 0); // 0s
        TimeSpan _c_sec1 = new TimeSpan(0, 0, 1); // 1s

        public Scenario3_Trim()
        {
            this.InitializeComponent();
            _cts = new CancellationTokenSource();

            // Hook up UI
            PickFileButton.Click += new RoutedEventHandler(PickFile);
            SetOutputButton.Click += new RoutedEventHandler(PickOutput);
            TargetFormat.SelectionChanged += new SelectionChangedEventHandler(OnTargetFormatChanged);
            Transcode.Click += new RoutedEventHandler(TranscodeTrim);
            Cancel.Click += new RoutedEventHandler(TranscodeCancel);
            MarkInButton.Click += new RoutedEventHandler(MarkIn);
            MarkOutButton.Click += new RoutedEventHandler(MarkOut);

            // Disable manual manipulation of trim points
            StartTimeText.Text = ((float)(_Start.TotalMilliseconds) / 1000).ToString();
            EndTimeText.Text = ((float)(_Stop.TotalMilliseconds) / 1000).ToString();
            StartTimeText.IsEnabled = false;
            EndTimeText.IsEnabled = false;


            // Media Controls
            InputPlayButton.Click += new RoutedEventHandler(InputPlayButton_Click);
            InputPauseButton.Click += new RoutedEventHandler(InputPauseButton_Click);
            InputStopButton.Click += new RoutedEventHandler(InputStopButton_Click);
            OutputPlayButton.Click += new RoutedEventHandler(OutputPlayButton_Click);
            OutputPauseButton.Click += new RoutedEventHandler(OutputPauseButton_Click);
            OutputStopButton.Click += new RoutedEventHandler(OutputStopButton_Click);

            // File is not selected, disable all buttons but PickFileButton
            DisableButtons();
            SetPickFileButton(true);
            SetOutputFileButton(true);
            SetCancelButton(false);
        }

        public void Dispose()
        {
            _cts.Dispose();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        async void TranscodeTrim(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            StopPlayers();
            DisableButtons();
            GetPresetProfile(ProfileSelect);

            // Clear messages
            StatusMessage.Text = "";

            _Transcoder.TrimStartTime = _Start;
            _Transcoder.TrimStopTime = _Stop;

            try
            {
                if (_InputFile != null && _OutputFile != null)
                {   
                    var preparedTranscodeResult = await _Transcoder.PrepareFileTranscodeAsync(_InputFile, _OutputFile, _Profile);

                    if (EnableMrfCrf444.IsChecked.HasValue && (bool)EnableMrfCrf444.IsChecked)
                    {
                        _Transcoder.VideoProcessingAlgorithm = MediaVideoProcessingAlgorithm.MrfCrf444;
                    }
                    else
                    {
                        _Transcoder.VideoProcessingAlgorithm = MediaVideoProcessingAlgorithm.Default;
                    }

                    if (preparedTranscodeResult.CanTranscode)
                    {
                        SetCancelButton(true);
                        var progress = new Progress<double>(TranscodeProgress);
                        await preparedTranscodeResult.TranscodeAsync().AsTask(_cts.Token, progress);
                        TranscodeComplete();
                    }
                    else
                    {
                        TranscodeFailure(preparedTranscodeResult.FailureReason);
                    }
                }
            }
            catch (TaskCanceledException)
            {
                OutputText("");
                TranscodeError("Transcode Canceled");
            }
            catch (Exception exception)
            {
                TranscodeError(exception.Message);
            }
        }

        void GetPresetProfile(ComboBox combobox)
        {
            _Profile = null;
            VideoEncodingQuality videoEncodingProfile = VideoEncodingQuality.Wvga;
            switch (combobox.SelectedIndex)
            {
                case 0:
                    videoEncodingProfile = VideoEncodingQuality.HD1080p;
                    break;
                case 1:
                    videoEncodingProfile = VideoEncodingQuality.HD720p;
                    break;
                case 2:
                    videoEncodingProfile = VideoEncodingQuality.Wvga;
                    break;
                case 3:
                    videoEncodingProfile = VideoEncodingQuality.Ntsc;
                    break;
                case 4:
                    videoEncodingProfile = VideoEncodingQuality.Pal;
                    break;
                case 5:
                    videoEncodingProfile = VideoEncodingQuality.Vga;
                    break;
                case 6:
                    videoEncodingProfile = VideoEncodingQuality.Qvga;
                    break;
            }

            switch (_OutputType)
            {
                case "AVI":
                    _Profile = MediaEncodingProfile.CreateAvi(videoEncodingProfile);
                    break;
                case "WMV":
                    _Profile = MediaEncodingProfile.CreateWmv(videoEncodingProfile);
                    break;
                default:
                    _Profile = Windows.Media.MediaProperties.MediaEncodingProfile.CreateMp4(videoEncodingProfile);
                    break;
            }

            /*
            For transcoding to audio profiles, create the encoding profile using one of these APIs:
                MediaEncodingProfile.CreateMp3(audioEncodingProfile)
                MediaEncodingProfile.CreateM4a(audioEncodingProfile)
                MediaEncodingProfile.CreateWma(audioEncodingProfile)
                MediaEncodingProfile.CreateWav(audioEncodingProfile)

            where audioEncodingProfile is one of these presets:
                AudioEncodingQuality.High
                AudioEncodingQuality.Medium
                AudioEncodingQuality.Low
            */
        }

        void MarkIn(Object sender, RoutedEventArgs e)
        {
            _Start = InputVideo.Position;
            StartTimeText.Text = ((float)(_Start.TotalMilliseconds) / 1000).ToString();
            // Make sure end time is after start time.
            if ((_Start.CompareTo(_Stop) > 0) || (_Start.CompareTo(_Stop) == 0 && _Start != _c_sec0))
            {
                _Stop = _Start.Add(_c_sec1);
                EndTimeText.Text = ((float)(_Stop.TotalMilliseconds) / 1000).ToString();
            }
        }

        void MarkOut(Object sender, RoutedEventArgs e)
        {
            _Stop = InputVideo.Position;
            EndTimeText.Text = ((float)(_Stop.TotalMilliseconds) / 1000).ToString();
            // Make sure end time is after start time.
            if ((_Start.CompareTo(_Stop) > 0) || (_Start.CompareTo(_Stop) == 0 && _Start != _c_sec0))
            {
                if (_Stop.CompareTo(_c_sec1) > 0)
                {
                    _Start = _Stop.Subtract(_c_sec1);
                }
                else
                {
                    _Start = _c_sec0;
                }
                StartTimeText.Text = ((float)(_Start.TotalMilliseconds) / 1000).ToString();
            }
        }

        void TranscodeProgress(double percent)
        {
            OutputText("Progress:  " + percent.ToString().Split('.')[0] + "%");
        }

        async void TranscodeComplete()
        {
            OutputText("Transcode completed.");
            OutputPathText("Output (" + _OutputFile.Path + ")");
            Windows.Storage.Streams.IRandomAccessStream stream = await _OutputFile.OpenAsync(Windows.Storage.FileAccessMode.Read);
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                OutputVideo.SetSource(stream, _OutputFile.ContentType);
            });

            EnableButtons();
            SetCancelButton(false);
        }

        async void TranscodeCancel(object sender, RoutedEventArgs e)
        {
            try
            {
                _cts.Cancel();
                _cts.Dispose();
                _cts = new CancellationTokenSource();

                if (_OutputFile != null)
                {
                    await _OutputFile.DeleteAsync();
                }
            }
            catch (Exception exception)
            {
                TranscodeError(exception.Message);
            }
        }

        async void TranscodeFailure(TranscodeFailureReason reason)
        {
            try
            {
                if (_OutputFile != null)
                {
                    await _OutputFile.DeleteAsync();
                }
            }
            catch (Exception exception)
            {
                TranscodeError(exception.Message);
            }

            switch (reason)
            {
                case TranscodeFailureReason.CodecNotFound:
                    TranscodeError("Codec not found.");
                    break;
                case TranscodeFailureReason.InvalidProfile:
                    TranscodeError("Invalid profile.");
                    break;
                default:
                    TranscodeError("Unknown failure.");
                    break;
            }
        }

        async void PickFile(object sender, RoutedEventArgs e)
        {
            Windows.Storage.Pickers.FileOpenPicker picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.VideosLibrary;
            picker.FileTypeFilter.Add(".wmv");
            picker.FileTypeFilter.Add(".mp4");

            Windows.Storage.StorageFile file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                Windows.Storage.Streams.IRandomAccessStream stream = await file.OpenAsync(Windows.Storage.FileAccessMode.Read);

                _InputFile = file;
                InputVideo.SetSource(stream, file.ContentType);
                InputVideo.Play();

                // Enable buttons
                EnableButtons();
            }
        }

        async void PickOutput(object sender, RoutedEventArgs e)
        {
            FileSavePicker picker = new FileSavePicker();
            picker.SuggestedStartLocation = PickerLocationId.VideosLibrary;
            picker.SuggestedFileName = _OutputFileName;
            picker.FileTypeChoices.Add(_OutputType, new System.Collections.Generic.List<string>() { _OutputFileExtension });

            _OutputFile = await picker.PickSaveFileAsync();

            if (_OutputFile != null)
            {
                SetTranscodeButton(true);
            }
        }

        void OnTargetFormatChanged(object sender, SelectionChangedEventArgs e)
        {
            switch (TargetFormat.SelectedIndex)
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

        void InputPlayButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (InputVideo.DefaultPlaybackRate == 0)
            {
                InputVideo.DefaultPlaybackRate = 1.0;
                InputVideo.PlaybackRate = 1.0;
            }

            InputVideo.Play();
        }

        void InputStopButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            InputVideo.Stop();
        }

        void InputPauseButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            InputVideo.Pause();
        }

        void OutputPlayButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (OutputVideo.DefaultPlaybackRate == 0)
            {
                OutputVideo.DefaultPlaybackRate = 1.0;
                OutputVideo.PlaybackRate = 1.0;
            }

            OutputVideo.Play();
        }

        void OutputStopButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            OutputVideo.Stop();
        }

        void OutputPauseButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            OutputVideo.Pause();
        }

        async void SetPickFileButton(bool isEnabled)
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                PickFileButton.IsEnabled = isEnabled;
            });
        }

        async void SetOutputFileButton(bool isEnabled)
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                SetOutputButton.IsEnabled = isEnabled;
            });
        }

        async void SetTranscodeButton(bool isEnabled)
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                Transcode.IsEnabled = isEnabled;
            });
        }

        async void SetCancelButton(bool isEnabled)
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                Cancel.IsEnabled = isEnabled;
            });
        }

        async void EnableButtons()
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                PickFileButton.IsEnabled = true;
                SetOutputButton.IsEnabled = true;
                TargetFormat.IsEnabled = true;
                ProfileSelect.IsEnabled = true;
                MarkInButton.IsEnabled = true;
                MarkOutButton.IsEnabled = true;
                EnableMrfCrf444.IsEnabled = true;

                // The transcode button's initial state should be disabled until an output
                // file has been set.
                Transcode.IsEnabled = false;
            });
        }

        async void DisableButtons()
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                ProfileSelect.IsEnabled = false;
                Transcode.IsEnabled = false;
                PickFileButton.IsEnabled = false;
                SetOutputButton.IsEnabled = false;
                TargetFormat.IsEnabled = false;
                MarkInButton.IsEnabled = false;
                MarkOutButton.IsEnabled = false;
                EnableMrfCrf444.IsEnabled = false;
            });
        }

        void EnableNonSquarePARProfiles()
        {
            ComboBoxItem_NTSC.IsEnabled = true;
            ComboBoxItem_PAL.IsEnabled = true;
        }

        void DisableNonSquarePARProfiles()
        {
            ComboBoxItem_NTSC.IsEnabled = false;
            ComboBoxItem_PAL.IsEnabled = false;

            // Ensure a valid profile is set
            if ((ProfileSelect.SelectedIndex == 3) || (ProfileSelect.SelectedIndex == 4))
            {
                ProfileSelect.SelectedIndex = 2;
            }
        }

        async void StopPlayers()
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                if (InputVideo.CurrentState != MediaElementState.Paused)
                {
                    InputVideo.Pause();
                }
                if (OutputVideo.CurrentState != MediaElementState.Paused)
                {
                    OutputVideo.Pause();
                }
            });
        }

        async void PlayFile(Windows.Storage.StorageFile MediaFile)
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                try
                {
                    Windows.Storage.Streams.IRandomAccessStream stream = await MediaFile.OpenAsync(FileAccessMode.Read);
                    OutputVideo.SetSource(stream, MediaFile.ContentType);
                    OutputVideo.Play();
                }
                catch (Exception exception)
                {
                    TranscodeError(exception.Message);
                }
            });
        }

        async void TranscodeError(string error)
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                StatusMessage.Foreground = new Windows.UI.Xaml.Media.SolidColorBrush(Windows.UI.Colors.Red);
                StatusMessage.Text = error;
            });

            EnableButtons();
            SetCancelButton(false);
        }

        async void OutputText(string text)
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                OutputMsg.Foreground = new Windows.UI.Xaml.Media.SolidColorBrush(Windows.UI.Colors.Green);
                OutputMsg.Text = text;
            });
        }

        async void OutputPathText(string text)
        {
            await _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                OutputPath.Text = text;
            });
        }
    }
}
