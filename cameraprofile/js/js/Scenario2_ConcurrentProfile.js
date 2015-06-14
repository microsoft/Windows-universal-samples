//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/Scenario2_ConcurrentProfile.html", {
        ready: function (element, options) {
            document.getElementById("checkConcurrentProfileBtn").addEventListener("click", checkConcurrentProfileBtn_click, false);
        }
    });

    /// <summary>
    /// Finds the video profile for the front and back camera and queries if
    /// both cameras have a matching video recording profile that supports concurrency.
    /// If a matching concurrent profile is supported, then we configure media initialization
    /// settings for both cameras to the matching profile. 
    /// </summary>
    function checkConcurrentProfileBtn_click() {
        var Capture = Windows.Media.Capture;
        // Disabling the button since initializing 2 cameras could be a little slow
        checkConcurrentProfileBtn.disabled = true;

        // Find front and back Device Id of capture device that supports Video Profile
        logStatus("Looking for all video capture devices on front panel");
        GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.front)
            .then(function (frontVideoDeviceId) {
            logStatus("Looking for all video capture devices on back panel");
            return GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.back)
                .then(function (backVideoDeviceId) {
                // First check if the devices support video profiles, if not there's no reason to proceed
                if (frontVideoDeviceId === "" || backVideoDeviceId === "") {
                    logError("ERROR: A capture device doesn't support Video Profile");
                    return;
                }

                logStatus("Video Profiles are supported on both cameras");

                var mediaCaptureFront = new Capture.MediaCapture();
                var mediaCaptureBack = new Capture.MediaCapture();

                // Create MediaCaptureInitilization settings for each video capture device
                var mediaInitSettingsFront = new Capture.MediaCaptureInitializationSettings();
                var mediaInitSettingsBack = new Capture.MediaCaptureInitializationSettings();

                mediaInitSettingsFront.videoDeviceId = frontVideoDeviceId;
                mediaInitSettingsBack.videoDeviceId = backVideoDeviceId;
                logStatus("Front Device Id located: " + frontVideoDeviceId);
                logStatus("Back Device Id located: " + backVideoDeviceId);

                logStatus("Querying for concurrent profile");

                // Acquire concurrent profiles available to front and back capture devices, then locate a concurrent
                // profile Id that matches for both devices
                var allFrontProfiles = Capture.MediaCapture.findAllVideoProfiles(frontVideoDeviceId);
                var allBackProfiles = Capture.MediaCapture.findConcurrentProfiles(backVideoDeviceId);
                var frontProfile = null;
                var backProfile = null;
                allFrontProfiles.forEach (function (fProfile) {
                    allBackProfiles.forEach (function (bProfile) {
                        if (fProfile.id == bProfile.id) {
                            frontProfile = fProfile;
                            backProfile = bProfile;
                        }
                    });
                });

                // Initialize our concurrency support flag.
                var concurrencySupported = false;
                if (frontProfile  && backProfile) {
                    mediaInitSettingsFront.videoProfile = frontProfile;
                    mediaInitSettingsBack.videoProfile = backProfile;
                    concurrencySupported = true;

                    logStatus("Concurrent profile located, device supports concurrency");
                }
                else {
                    // There are no concurrent profiles available on this device, thus
                    // there is not profile to select. With a lack of concurrent profile
                    // each camera will have to be managed manually and cannot support
                    // simultaneous streams. So we set the Video profile to null to 
                    // indicate that camera must be managed individually.
                    mediaInitSettingsFront.videoProfile = null;
                    mediaInitSettingsBack.videoProfile = null;

                    logStatus("No Concurrent profiles located, device does not support concurrency");
                }

                logStatus("Initializing Front camera settings");
                
                var cameraInitPromises = null;
                var frontInitPromise = mediaCaptureFront.initializeAsync(mediaInitSettingsFront);
               
                if (concurrencySupported) {
                    logStatus("Device supports concurrency, initializing Back camera settings");

                    // Only initialize the back camera if concurrency is available.  Otherwise,
                    // we can only use one camera at a time (which in this case will be the front
                    // camera).
                    var backInitPromise = mediaCaptureBack.initializeAsync(mediaInitSettingsBack);
                    cameraInitPromises = WinJS.Promise.join(frontInitPromise, backInitPromise);
                }
                return cameraInitPromises;
            });
        }, function (error) {
            logError(error.message);
        }).done (function()
        {
            // Re-enable our button
            checkConcurrentProfileBtn.disabled = false;
        })
    }

    /// <summary>
    /// Helper method to obtain a device Id that supports a Video Profile.
    /// If no Video Profile is found we return empty string indicating that there isn't a device on 
    /// the desired panel with a supported Video Profile.
    /// </summary>
    /// <param name="panel">Contains Device Enumeration information regarding camera panel we are looking for</param>
    /// <returns>Device Information Id string</returns>
    function GetVideoProfileSupportedDeviceIdAsync(panel) {
        var deviceInfo = Windows.Devices.Enumeration.DeviceInformation;

        return deviceInfo.findAllAsync(Windows.Devices.Enumeration.DeviceClass.videoCapture)
        .then(function (devices) {
            var deviceId = "";
            logStatus("Number of video capture devices found: " + devices.length);

            // Loop through devices looking for device that supports Video Profile
            devices.forEach(function (cameraDeviceInfo) {
                if (Windows.Media.Capture.MediaCapture.isVideoProfileSupported(cameraDeviceInfo.id) && cameraDeviceInfo.enclosureLocation.panel === panel) {
                    deviceId = cameraDeviceInfo.id;
                    return deviceId;
                }
            });

            return deviceId;
        });
    }

    function logError(msg) {
        WinJS.log && WinJS.log(msg, "", "error");
    }

    function logStatus(msg) {
        msg += "\r\n";
        outputBox.textContent += msg;
        WinJS.log && WinJS.log(msg, "", "status");
    }
})();
