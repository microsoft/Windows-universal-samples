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
    var page = WinJS.UI.Pages.define("/html/Scenario3_EnableHdrProfile.html", {
        ready: function (element, options) {
            document.getElementById("checkHdrSupportBtn").addEventListener("click", checkHdrSupportBtn_click, false);
        }
    });

    /// <summary>
    /// Finds the video profiles for the back camera. Then queries for a profile that supports
    /// HDR video recording. If a HDR supported video profile is located, then we configure media settings to the
    /// matching profile. Finally we initialize media capture HDR recording mode to auto.
    /// </summary>
    function checkHdrSupportBtn_click() {
        var Capture = Windows.Media.Capture;

        // Select the first video capture device found on the back of the device
        logStatus("Querying for video capture device on back of the device that supports Video Profile");
        GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.back)
        .then(function (videoDeviceId) {
            if (videoDeviceId === "") {
                logError("ERROR: No Video Device Id found, verify your device supports profiles");
                return;
            }

            var mediaCapture = new Capture.MediaCapture();
            var mediaCaptureInitSettings = new Capture.MediaCaptureInitializationSettings();

            logStatus("Found device that supports Video Profile, Device Id: " + videoDeviceId);
            var profiles = Capture.MediaCapture.findAllVideoProfiles(videoDeviceId, Capture.KnownVideoProfile.videoRecording);

            // Walk through available profiles, look for first profile with HDR supported Video Profile
            var HdrVideoSupported = false;
            for (var i = 0; i < profiles.length; i++) {
                var profile = profiles.getAt(i);
                var recordMediaDescription = profile.supportedRecordMediaDescription;
                for (var i = 0; i < recordMediaDescription.length; i++) {
                    var videoProfileMediaDescription = recordMediaDescription.getAt(i);
                    if (videoProfileMediaDescription.isHdrVideoSupported) {
                        // We've located the profile and description for HDR Video, set profile and flag
                        mediaCaptureInitSettings.videoProfile = profile;
                        mediaCaptureInitSettings.recordMediaDescription = videoProfileMediaDescription;
                        HdrVideoSupported = true;
                        logStatus("HDR supported video profile found, set video profile to current HDR supported profile");
                        break;
                    }
                }

                if (HdrVideoSupported) {
                    break;
                }
            }

            logStatus("Initializing Media settings to HDR Supported video profile");
            return mediaCapture.initializeAsync(mediaCaptureInitSettings);
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
