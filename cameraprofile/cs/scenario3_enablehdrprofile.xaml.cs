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

namespace CameraProfile
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using Windows.Devices.Enumeration;
    using Windows.Media.Capture;
    using Windows.UI.Xaml.Controls;
    using Windows.UI.Xaml.Navigation;
    using SDKTemplate;
    using System.Threading.Tasks;


    /// <summary>
    /// EnableHDRProfile page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3_EnableHdrProfile : Page
    {
        /// <summary>
        /// Private MainPage object for status updates
        /// </summary>
        private MainPage rootPage;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario3_EnableHdrProfile"/> class.
        /// </summary>
        public Scenario3_EnableHdrProfile()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Override default OnNavigateTo to initialize 
        /// the rootPage object for Status message updates
        /// </summary>
        /// <param name="e">Contains state information and event data associated with the event</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        /// <summary>
        /// Finds the video profiles for the back camera. Then queries for a profile that supports
        /// HDR video recording. If a HDR supported video profile is located, then we configure media settings to the
        /// matching profile. Finally we initialize media capture HDR recording mode to auto.
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void CheckHdrSupportBtn_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            string videoDeviceId = string.Empty;
            bool HdrVideoSupported = false;
            MediaCapture mediaCapture = new MediaCapture();
            MediaCaptureInitializationSettings mediaCaptureInitSetttings = new MediaCaptureInitializationSettings();

            // Select the first video capture device found on the back of the device
            await LogStatusToOutputBox("Querying for video capture device on back of the device that supports Video Profile");
            videoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Back);

            if (string.IsNullOrEmpty(videoDeviceId))
            {
                await LogStatus("ERROR: No Video Device Id found, verify your device supports profiles", NotifyType.ErrorMessage);
                return;
            }

            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture,
                "Found device that supports Video Profile, Device Id:\r\n {0}", videoDeviceId));
            IReadOnlyList<MediaCaptureVideoProfile> profiles = MediaCapture.FindKnownVideoProfiles(videoDeviceId, KnownVideoProfile.VideoRecording);

            // Walk through available profiles, look for first profile with HDR supported Video Profile
            foreach (MediaCaptureVideoProfile profile in profiles)
            {
                IReadOnlyList<MediaCaptureVideoProfileMediaDescription> recordMediaDescription = profile.SupportedRecordMediaDescription;
                foreach (MediaCaptureVideoProfileMediaDescription videoProfileMediaDescription in recordMediaDescription)
                {
                    if (videoProfileMediaDescription.IsHdrVideoSupported)
                    {
                        // We've located the profile and description for HDR Video, set profile and flag
                        mediaCaptureInitSetttings.VideoProfile = profile;
                        mediaCaptureInitSetttings.RecordMediaDescription = videoProfileMediaDescription;
                        HdrVideoSupported = true;
                        await LogStatus("HDR supported video profile found, set video profile to current HDR supported profile", NotifyType.StatusMessage);
                        break;
                    }
                }

                if (HdrVideoSupported)
                {
                    break;
                }
            }

            await LogStatusToOutputBox("Initializing Media settings to HDR Supported video profile");
            await mediaCapture.InitializeAsync(mediaCaptureInitSetttings);
            if (HdrVideoSupported)
            {
                await LogStatusToOutputBox("Initializing HDR Video Mode to Auto");
                mediaCapture.VideoDeviceController.HdrVideoControl.Mode = Windows.Media.Devices.HdrVideoMode.Auto;
            }
        }

        /// <summary>
        /// Helper method to obtain a device Id that supports a Video Profile.
        /// If no Video Profile is found we return empty string indicating that there isn't a device on 
        /// the desired panel with a supported Video Profile.
        /// </summary>
        /// <param name="panel">Contains Device Enumeration information regarding camera panel we are looking for</param>
        /// <returns>Device Information Id string</returns>
        public async Task<string> GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel panel)
        {
            string deviceId = string.Empty;

            // Finds all video capture devices
            await LogStatusToOutputBox("Looking for all video capture devices");
            DeviceInformationCollection devices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);
            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Number of video capture devices found: {0}", devices.Count.ToString()));

            foreach (var device in devices)
            {
                // Check if the device on the requested panel supports Video Profile
                if (MediaCapture.IsVideoProfileSupported(device.Id) && device.EnclosureLocation.Panel == panel)
                {
                    // We've located a device that supports Video Profiles on expected panel
                    deviceId = device.Id;
                    break;
                }
            }

            return deviceId;
        }

        /// <summary>
        /// Marshall log message to the UI thread
        /// and update outputBox, this method is for more general messages
        /// </summary>
        /// <param name="message">Message to log to the outputBox</param>
        private async Task LogStatusToOutputBox(string message)
        {
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                outputBox.Text += message + "\r\n";
                outputScrollViewer.ChangeView(0, outputBox.ActualHeight, 1);
            });
        }

        /// <summary>
        /// Method to log Status to Main Status block,
        /// this method is for important status messages to be 
        /// displayed at bottom of scenario page
        /// </summary>
        /// <param name="message">Message to log to the output box</param>
        /// <param name="type">Notification Type for status message</param>
        private async Task LogStatus(string message, NotifyType type)
        {
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(message, type);
            });
            await LogStatusToOutputBox(message);
        }
    }
}

