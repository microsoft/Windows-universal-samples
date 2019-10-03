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
    var page = WinJS.UI.Pages.define("/html/Scenario1_SetRecordProfile.html", {
        ready: function (element, options) {
            document.getElementById("initRecordProfileBtn").addEventListener("click", initRecordProfileBtn_click, false);
            document.getElementById("initCustomProfileBtn").addEventListener("click", initCustomProfileBtn_click, false);
        }
    });

    /// <summary>
    /// Locates a video profile for the back camera and then queries if
    /// the discovered profile supports 640x480 30 FPS recording.
    /// If a supported profile is located, then we configure media
    /// settings to the matching profile. Else we use default profile.
    /// </summary>
    function initRecordProfileBtn_click() {
        var Capture = Windows.Media.Capture;

        // Look for a video capture device Id with a video profile matching panel location
        GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.back)
        .then(function (videoDeviceId) {
            if (videoDeviceId === "") {
                logError("ERROR: No Video Device Id found, verify your device supports profiles");
                return;
            }

            logStatus("Found video capture device that supports Video Profile, Device ID: " + videoDeviceId);
            logStatus("Retrieving all Video Profiles");

            var mediaCapture = new Capture.MediaCapture();
            var profiles = Capture.MediaCapture.findAllVideoProfiles(videoDeviceId);

            logStatus("Number of Video Profiles found: " + profiles.length)
            logStatus("Querying for matching WVGA 30FPS Profile");

            // Output all Video Profiles found, frame rate is rounded up for display purposes
            // Also looking for WVGA 30FPS profile
            var mediaInitSettings = new Capture.MediaCaptureInitializationSettings();
            var profileFound = false;
            profiles.forEach(function (profile) {
                var description = profile.supportedRecordMediaDescription;
                description.forEach(function (desc) {
                    PrintProfileInformation(profile, desc);

                    if (!profileFound) {
                        if (desc.width == 640 &&
                            desc.height == 480 &&
                            Math.round(desc.frameRate) == 30) {
                            mediaInitSettings.videoProfile = profile;
                            mediaInitSettings.recordMediaDescription = desc;
                            profileFound = true;
                        }
                    }
                });
            });

            if (profileFound) {
                logStatus("WVGA 30FPS profile found");
                PrintProfileInformation(mediaInitSettings.videoProfile, mediaInitSettings.recordMediaDescription);
            }
            else {
                // Could not locate a WVGA 30FPS profile, use default video recording profile
                mediaInitSettings.videoProfile = profiles.getAt(0);
                logError("Could not locate a matching profile, setting to default recording profile");
            }

            return mediaCapture.initializeAsync(mediaInitSettings);
        }, function (error) {
            logError(error.message);
        }).done();
    }

    /// <summary>
    /// Locates a video profile for the back camera and then queries if
    /// the discovered profile is a matching custom profile.
    /// If a custom profile is located, we configure media
    /// settings to the custom profile. Else we use default profile.
    /// </summary>
    function initCustomProfileBtn_click() {
        var Capture = Windows.Media.Capture;

        // Look for a video capture device Id with a video profile matching panel location
        GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.back)
        .then(function (videoDeviceId) {
            if (videoDeviceId === "") {
                logError("ERROR: No Video Device Id found, verify your device supports profiles");
                return;
            }

            var mediaCapture = new Capture.MediaCapture();
            var mediaInitSettings = new Capture.MediaCaptureInitializationSettings();

            // For demonstration purposes, we use the Profile Id from WVGA 640x480 30 FPS profile available
            // on desktop simulator and phone emulators 
            var customProfileId = "{A0E517E8-8F8C-4F6F-9A57-46FC2F647EC0},0";

            logStatus("Found video capture device that supports Video Profile, Device ID: " + videoDeviceId);

            logStatus("Querying device for custom profile support");
            mediaInitSettings.videoProfile = null;
            var allVideoProfiles = Capture.MediaCapture.findAllVideoProfiles(videoDeviceId);
            
            profile_loop:
            for (var i = 0; i < allVideoProfiles.length; i++) {
                var profile = allVideoProfiles.getAt(i);
                if (profile.id === customProfileId) {
                    mediaInitSettings.videoProfile = profile;
                    break profile_loop;
                }
            }

            // In the event the profile isn't located, we set Video Profile to the default
            if (mediaInitSettings.videoProfile === null) {
                logError("Could not locate a matching profile, setting to default recording profile");
                mediaInitSettings.videoProfile = allVideoProfiles.getAt(0);
            }

            logStatus("Custom recording profile located: " + customProfileId);
            return mediaCapture.initializeAsync(mediaInitSettings);
        }, function (error) {
            logError(error.message);
        }).done();
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
            })

            return deviceId;
        });
    }

    /// <summary>
    /// Helper to print MediaCapture profile information
    /// </summary>
    /// <param name="message">MediaCapture video profile</param>
    /// <param name="type">MediaCapture video profile description</param>
    function PrintProfileInformation(profile, description) {
        logStatus("VideoProfile: Profile Id: " + profile.id +
            " Width: " + description.width +
            " Height: " + description.height +
            " FrameRate: " + Math.round(description.frameRate));
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
