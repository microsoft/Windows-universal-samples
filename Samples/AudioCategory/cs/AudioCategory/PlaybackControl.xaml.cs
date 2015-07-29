//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.UI.Xaml.Media;
using Windows.UI.Core;
using Windows.Foundation;
using Windows.Media;

namespace PlaybackManager
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class PlaybackControl
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        CoreWindow cw = Window.Current.CoreWindow;
        bool IsPlaying = false;

        SystemMediaTransportControls systemMediaControls = null;

        public PlaybackControl()
        {
            this.InitializeComponent();

            // add new handlers
            systemMediaControls = SystemMediaTransportControls.GetForCurrentView();
            systemMediaControls.PropertyChanged += SystemMediaControls_PropertyChanged;
            systemMediaControls.ButtonPressed += SystemMediaControls_ButtonPressed;
            systemMediaControls.IsPlayEnabled = true;
            systemMediaControls.IsPauseEnabled = true;
            systemMediaControls.IsStopEnabled = true;
            systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Closed;
        }

        ~PlaybackControl()
        {
            if (systemMediaControls != null)
            {
                systemMediaControls.PropertyChanged -= SystemMediaControls_PropertyChanged;
                systemMediaControls.ButtonPressed -= SystemMediaControls_ButtonPressed;
            }
        }

        async public void Play()
        {
            await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                OutputMedia.Play();
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Playing;
            });
        }

        async public void Pause()
        {
            await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                OutputMedia.Pause();
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Paused;
            });
        }

        async public void Stop()
        {
            await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                OutputMedia.Stop();
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Stopped;
            });
        }

        public void SetAudioCategory(AudioCategory category)
        {
            OutputMedia.AudioCategory = category;
        }

        public void SetAudioDeviceType(AudioDeviceType devicetype)
        {
            OutputMedia.AudioDeviceType = devicetype;
        }

        public async void SelectFile()
        {
            Windows.Storage.Pickers.FileOpenPicker picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.MusicLibrary;
            picker.FileTypeFilter.Add(".mp3");
            picker.FileTypeFilter.Add(".mp4");
            picker.FileTypeFilter.Add(".m4a");
            picker.FileTypeFilter.Add(".wma");
            picker.FileTypeFilter.Add(".wav");
            Windows.Storage.StorageFile file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                var stream = await file.OpenAsync(Windows.Storage.FileAccessMode.Read);
                OutputMedia.AutoPlay = false;
                await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                {
                    OutputMedia.SetSource(stream, file.ContentType);
                });
            }
        }

        async void DisplayStatus(string text)
        {
            await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                Status.Text += text + "\n";
            });
        }

        string GetTimeStampedMessage(string eventText)
        {
            return eventText + "   " + DateTime.Now.Hour.ToString() + ":" + DateTime.Now.Minute.ToString() + ":" + DateTime.Now.Second.ToString();
        }

        //If your app is playing media you feel that a user should not miss if a VOIP call comes in, you may
        //want to consider pausing playback when your app receives a SoundLevel(Low) notification.
        //A SoundLevel(Low) means your app volume has been attenuated by the system (likely for a VOIP call).

        string SoundLevelToString(SoundLevel level)
        {
            string LevelString;

            switch (level)
            {
                case SoundLevel.Muted:
                    LevelString = "Muted";
                    break;
                case SoundLevel.Low:
                    LevelString = "Low";
                    break;
                case SoundLevel.Full:
                    LevelString = "Full";
                    break;
                default:
                    LevelString = "Unknown";
                    break;
            }
            return LevelString;
        }

        async void AppMuted()
        {
            await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                if (OutputMedia.CurrentState != MediaElementState.Paused)
                {
                    IsPlaying = true;
                    Pause();
                }
                else
                {
                    IsPlaying = false;
                }
            });
            DisplayStatus(GetTimeStampedMessage("Muted"));
        }

        async void AppUnmuted()
        {
            await cw.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                if (IsPlaying)
                {
                    Play();
                }
            });
            DisplayStatus(GetTimeStampedMessage("Unmuted"));
        }

        void SystemMediaControls_PropertyChanged(SystemMediaTransportControls sender, SystemMediaTransportControlsPropertyChangedEventArgs e)
        {
            if (e.Property == SystemMediaTransportControlsProperty.SoundLevel)
            {
                var soundLevelState = sender.SoundLevel;

                DisplayStatus(GetTimeStampedMessage("App sound level is [" + SoundLevelToString(soundLevelState) + "]"));
                if (soundLevelState == SoundLevel.Muted)
                {
                    AppMuted();
                }
                else
                {
                    AppUnmuted();
                }
            }
        }

        void SystemMediaControls_ButtonPressed(SystemMediaTransportControls sender, SystemMediaTransportControlsButtonPressedEventArgs e)
        {
            switch (e.Button)
            {
                case SystemMediaTransportControlsButton.Play:
                    Play();
                    DisplayStatus(GetTimeStampedMessage("Play Pressed"));
                    break;

                case SystemMediaTransportControlsButton.Pause:
                    Pause();
                    DisplayStatus(GetTimeStampedMessage("Pause Pressed"));
                    break;

                case SystemMediaTransportControlsButton.Stop:
                    Stop();
                    DisplayStatus(GetTimeStampedMessage("Stop Pressed"));
                    break;

                default:
                    break;
            }
        }

        private void Button_Play_Click(object sender, RoutedEventArgs e)
        {
            Play();
        }

        private void Button_Pause_Click(object sender, RoutedEventArgs e)
        {
            Pause();
        }

        private void Button_Stop_Click(object sender, RoutedEventArgs e)
        {
            Stop();
        }
    }
}
