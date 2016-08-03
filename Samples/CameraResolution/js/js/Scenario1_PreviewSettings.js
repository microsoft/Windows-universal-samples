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

    var Capture = Windows.Media.Capture;

    var mediaCapture = null,
        allProperties = [];

    var isPreviewing = false;

    var page = WinJS.UI.Pages.define("/html/Scenario1_PreviewSettings.html", {
        ready: function (element, options) {
            document.getElementById("initCameraBtn").addEventListener("click", initCameraBtn_click, false);
            document.getElementById("cameraSettings").addEventListener("change", comboBoxSettings_changed, false);
        }
    });

    /// <summary>
    /// Initializes the camera
    /// </summary>
    function initializeCameraAsync() {
        mediaCapture = new Capture.MediaCapture();
        mediaCapture.addEventListener("failed", mediaCapture_failed);

        return mediaCapture.initializeAsync()
        .then(function () {
            var previewVidTag = document.getElementById("cameraPreview");
            var previewUrl = URL.createObjectURL(mediaCapture);
            previewVidTag.src = previewUrl;
            previewVidTag.play();

            isPreviewing = true;
        }, function (error) {
            logStatus(error.message, "error");
        });
    }

    /// <summary>
    /// Cleans up the camera and state variables
    /// </summary>
    function cleanupCamera() {
        if (isPreviewing) {
            var previewVidTag = document.getElementById("cameraPreview");
            previewVidTag.pause();
            previewVidTag.src = null;

            isPreviewing = false;
        }

        if (mediaCapture != null) {
            mediaCapture.removeEventListener("failed", mediaCapture_failed);
            mediaCapture = null;
        }
    }

    /// <summary>
    /// Initializes the camera and populates the UI
    /// </summary>
    function initCameraBtn_click() {
        initializeCameraAsync()
        .then(function () {
            initCameraBtn.style.visibility = "hidden";
            populateSettingsComboBox();
        });
    }

    /// <summary>
    ///  Event handler for preview settings combo box. Updates stream resolution based on the selection.
    /// </summary>
    function comboBoxSettings_changed() {
        if (isPreviewing) {
            // The first element is just text
            if (cameraSettings.value == "")
                return;

            mediaCapture.videoDeviceController.setMediaStreamPropertiesAsync(Capture.MediaStreamType.videoPreview, allProperties[cameraSettings.value]);
        }
    }
    
    /// <summary>
    /// Populates the combo box with all possible combinations of settings returned by the camera driver
    /// </summary>
    function populateSettingsComboBox() {
        // Query all properties of the device
        allProperties = mediaCapture.videoDeviceController.getAvailableMediaStreamProperties(Capture.MediaStreamType.videoPreview);

        // Populate the combo box with the entries
        allProperties.forEach(function (properties, index) {
            var option = document.createElement("option");
            var streamResolution = new streamResolutionHelper(properties);
            option.innerHTML = streamResolution.getFriendlyName(true);
            option.value = index;
            cameraSettings.appendChild(option);
        });
    }

    function logStatus(msg, type) {
        WinJS.log && WinJS.log(msg, "", type);
    }

    function mediaCapture_failed(e) {
        logStatus("Preview stopped: " + e.message, "error");
        isPreviewing = false;
        cleanupCamera();
    }
})();
