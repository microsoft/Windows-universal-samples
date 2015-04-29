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
    using System.Globalization;
    using System.Linq;
    using System.Threading.Tasks;
    using Windows.Devices.Enumeration;
    using Windows.Media.Capture;
    using Windows.UI.Xaml.Controls;
    using Windows.UI.Xaml.Navigation;
    using SDKTemplate;

    /// <summary>
    /// ConcurrenProfile page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_ConcurrentProfile : Page
    {
        /// <summary>
        /// Private MainPage object for status updates
        /// </summary>
        private MainPage rootPage;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario2_ConcurrentProfile"/> class.
        /// </summary>
        public Scenario2_ConcurrentProfile()
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
        /// Finds the video profile for the front and back camera and queries if
        /// both cameras have a matching video recording profile that supports concurrency.
        /// If a matching concurrent profile is supported, then we configure media initialization
        /// settings for both cameras to the matching profile. 
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void CheckConcurrentProfileBtn_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Initialize our concurrency support flag.
            bool concurrencySupported = false;

            string frontVideoDeviceId = string.Empty;
            string backVideoDeviceId = string.Empty;

            MediaCapture mediaCaptureFront = new MediaCapture();
            MediaCapture mediaCaptureBack = new MediaCapture();

            // Find front and back Device Id of capture device that supports Video Profile
            await LogStatusToOutputBox("Looking for all video capture devices on front panel");
            frontVideoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Front);
            await LogStatusToOutputBox("Looking for all video capture devices on back panel");
            backVideoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Back);

            // First check if the devices support video profiles, if not there's no reason to proceed
            if (string.IsNullOrEmpty(frontVideoDeviceId) || string.IsNullOrEmpty(backVideoDeviceId))
            {
                await LogStatus("ERROR: A capture device doesn't support Video Profile", NotifyType.ErrorMessage);
                return;
            }

            await LogStatusToOutputBox("Video Profiles are supported on both cameras");

            // Create MediaCaptureInitilization settings for each video capture device
            MediaCaptureInitializationSettings mediaInitSettingsFront = new MediaCaptureInitializationSettings();
            MediaCaptureInitializationSettings mediaInitSettingsBack = new MediaCaptureInitializationSettings();

            mediaInitSettingsFront.VideoDeviceId = frontVideoDeviceId;
            mediaInitSettingsBack.VideoDeviceId = backVideoDeviceId;
            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Front Device Id located: {0}", frontVideoDeviceId.ToString()));
            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Back Device Id located: {0}", backVideoDeviceId.ToString()));

            await LogStatusToOutputBox("Querying for concurrent profile");

            // Acquire concurrent profiles available to front and back capture devices, then locate a concurrent
            // profile Id that matches for both devices
            var concurrentProfile = (from frontProfile in MediaCapture.FindConcurrentProfiles(frontVideoDeviceId)
                                     from backProfile in MediaCapture.FindConcurrentProfiles(backVideoDeviceId)
                                     where frontProfile.Id == backProfile.Id
                                     select new { frontProfile, backProfile }).FirstOrDefault();

            if (concurrentProfile != null)
            {
                mediaInitSettingsFront.VideoProfile = concurrentProfile.frontProfile;
                mediaInitSettingsBack.VideoProfile = concurrentProfile.backProfile;
                concurrencySupported = true;

                await LogStatus("Concurrent profile located, device supports concurrency", NotifyType.StatusMessage);
            }
            else
            {
                // There are no concurrent profiles available on this device, thus
                // there is not profile to select. With a lack of concurrent profile
                // each camera will have to be managed manually and cannot support
                // simultaneous streams. So we set the Video profile to null to 
                // indicate that camera must be managed individually.
                mediaInitSettingsFront.VideoProfile = null;
                mediaInitSettingsBack.VideoProfile = null;

                await LogStatus("No Concurrent profiles located, device does not support concurrency", NotifyType.ErrorMessage);
            }

            await LogStatusToOutputBox("Initializing Front camera settings");
            await mediaCaptureFront.InitializeAsync(mediaInitSettingsFront);
            if (concurrencySupported)
            {
                await LogStatusToOutputBox("Device supports concurrency, initializing Back camera settings");
                
                // Only initialize the back camera if concurrency is available.  Otherwise,
                // we can only use one camera at a time (which in this case will be the front
                // camera).
                await mediaCaptureBack.InitializeAsync(mediaInitSettingsBack);
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
