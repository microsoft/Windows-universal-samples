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
    using System.Linq;
    using System.Threading.Tasks;
    using Windows.Devices.Enumeration;
    using Windows.Media.Capture;
    using Windows.UI.Xaml;
    using Windows.UI.Xaml.Controls;
    using Windows.UI.Xaml.Navigation;
    using SDKTemplate;

    /// <summary>
    /// SetRecordProfile page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_SetRecordProfile : Page
    {
        /// <summary>
        /// Private MainPage object for status updates
        /// </summary>
        private MainPage rootPage;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario1_SetRecordProfile"/> class.
        /// </summary>
        public Scenario1_SetRecordProfile()
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
        /// Locates a video profile for the back camera and then queries if
        /// the discovered profile supports 640x480 30 FPS recording.
        /// If a supported profile is located, then we configure media
        /// settings to the matching profile. Else we use default profile.
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void InitRecordProfileBtn_Click(object sender, RoutedEventArgs e)
        {
            MediaCapture mediaCapture = new MediaCapture();
            MediaCaptureInitializationSettings mediaInitSettings = new MediaCaptureInitializationSettings();
            string videoDeviceId = string.Empty;

            // Look for a video capture device Id with a video profile matching panel location
            videoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Back);

            if (string.IsNullOrEmpty(videoDeviceId))
            {
                await LogStatus("ERROR: No Video Device Id found, verify your device supports profiles", NotifyType.ErrorMessage);
                return;
            }

            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Found video capture device that supports Video Profile, Device Id:\r\n {0}", videoDeviceId));
            await LogStatusToOutputBox("Retrieving all Video Profiles");
            IReadOnlyList<MediaCaptureVideoProfile> profiles = MediaCapture.FindAllVideoProfiles(videoDeviceId);

            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Number of Video Profiles found: {0}", profiles.Count));

            // Output all Video Profiles found, frame rate is rounded up for display purposes
            foreach (var profile in profiles)
            {
                var description = profile.SupportedRecordMediaDescription;
                foreach (var desc in description)
                {
                    await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture,
                        "Video Profile Found: Profile Id: {0}\r\n Width: {1} Height: {2} FrameRate: {3}", profile.Id, desc.Width, desc.Height, Math.Round(desc.FrameRate).ToString()));
                }
            }

            // Look for WVGA 30FPS profile 
            await LogStatusToOutputBox("Querying for matching WVGA 30FPS Profile");
            var match = (from profile in profiles
                         from desc in profile.SupportedRecordMediaDescription
                         where desc.Width == 640 && desc.Height == 480 && Math.Round(desc.FrameRate) == 30
                         select new { profile, desc }).FirstOrDefault();

            if (match != null)
            {
                mediaInitSettings.VideoProfile = match.profile;
                mediaInitSettings.RecordMediaDescription = match.desc;
                await LogStatus(string.Format(CultureInfo.InvariantCulture,
                        "Matching WVGA 30FPS Video Profile found: \r\n Profile Id: {0}\r\n Width: {1} Height: {2} FrameRate: {3}",
                        mediaInitSettings.VideoProfile.Id, mediaInitSettings.RecordMediaDescription.Width,
                        mediaInitSettings.RecordMediaDescription.Height, Math.Round(mediaInitSettings.RecordMediaDescription.FrameRate).ToString()), NotifyType.StatusMessage);
            }
            else
            {
                // Could not locate a WVGA 30FPS profile, use default video recording profile
                mediaInitSettings.VideoProfile = profiles[0];
                await LogStatus("Could not locate a matching profile, setting to default recording profile", NotifyType.ErrorMessage);
            }

            await mediaCapture.InitializeAsync(mediaInitSettings);
            await LogStatusToOutputBox("Media Capture settings initialized to selected profile");

        }

        /// <summary>
        /// Locates a video profile for the back camera and then queries if
        /// the discovered profile is a matching custom profile.
        /// If a custom profile is located, we configure media
        /// settings to the custom profile. Else we use default profile.
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void InitCustomProfileBtn_Click(object sender, RoutedEventArgs e)
        {
            MediaCapture mediaCapture = new MediaCapture();
            MediaCaptureInitializationSettings mediaInitSettings = new MediaCaptureInitializationSettings();
            string videoDeviceId = string.Empty;

            // For demonstration purposes, we use the Profile Id from WVGA 640x480 30 FPS profile available
            // on desktop simulator and phone emulators 
            string customProfileId = "{A0E517E8-8F8C-4F6F-9A57-46FC2F647EC0},0";

            videoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Back);

            if (string.IsNullOrEmpty(videoDeviceId))
            {
                await LogStatus("ERROR: No Video Device Id found, verify your device supports profiles", NotifyType.ErrorMessage);
                return;
            }

            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Found video capture device that supports Video Profile, Device Id:\r\n {0}", videoDeviceId));
            await LogStatusToOutputBox("Querying device for custom profile support");
            mediaInitSettings.VideoProfile =  (from profile in MediaCapture.FindAllVideoProfiles(videoDeviceId)
                                 where profile.Id == customProfileId
                                 select profile).FirstOrDefault();

            // In the event the profile isn't located, we set Video Profile to the default
            if (mediaInitSettings.VideoProfile == null)
            {
                await LogStatus("Could not locate a matching profile, setting to default recording profile", NotifyType.ErrorMessage);
                mediaInitSettings.VideoProfile = MediaCapture.FindAllVideoProfiles(videoDeviceId).FirstOrDefault();
            }

            await LogStatus(string.Format(CultureInfo.InvariantCulture, "Custom recording profile located: {0}", customProfileId), NotifyType.StatusMessage);
            await mediaCapture.InitializeAsync(mediaInitSettings);
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

            // Find all video capture devices
            await LogStatusToOutputBox("Looking for all video capture devices");
            DeviceInformationCollection devices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);
            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Number of video capture devices found: {0}", devices.Count.ToString()));

            // Loop through devices looking for device that supports Video Profile
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
