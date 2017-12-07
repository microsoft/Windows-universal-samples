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
    var StorageLibrary = Windows.Storage.StorageLibrary;
    var KnownLibraryId = Windows.Storage.KnownLibraryId;
    var ApplicationData = Windows.Storage.ApplicationData;

    var mediaCapture = null,
        previewProperties = [],
        photoProperties = [];

    var isPreviewing = false;

    // Folder in which the captures will be stored (initialized in SetupUiAsync)
    var captureFolder;

    var page = WinJS.UI.Pages.define("/html/Scenario2_PhotoSettings.html", {
        ready: function (element, options) {
            document.getElementById("initCameraBtn").addEventListener("click", initCameraBtn_click, false);
            document.getElementById("previewSettings").addEventListener("change", previewSettings_changed, false);
            document.getElementById("photoSettings").addEventListener("change", photoSettings_changed, false);
            document.getElementById("photoButton").addEventListener("click", photoButton_click, false);
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
        return StorageLibrary.getLibraryAsync(KnownLibraryId.pictures)
        .then(function (picturesLibrary) {
            // Fall back to the local app storage if the Pictures Library is not available
            captureFolder = picturesLibrary.saveFolder || ApplicationData.current.localFolder;
            return initializeCameraAsync();
        })
        .then(function () {
            initCameraBtn.style.visibility = "hidden";
            checkIfStreamsAreIdentical();
            previewProperties = populateComboBox(Capture.MediaStreamType.videoPreview, previewSettings, true);
            photoProperties = populateComboBox(Capture.MediaStreamType.photo, photoSettings, false);
            photoButton.disabled = false;
        });
    }

    /// <summary>
    ///  Event handler for preview settings combo box. Updates stream resolution based on the selection.
    /// </summary>
    function previewSettings_changed() {
        if (isPreviewing) {
            // The first element is just text
            if (previewSettings.value == "")
                return;

            mediaCapture.videoDeviceController.setMediaStreamPropertiesAsync(Capture.MediaStreamType.videoPreview, previewProperties[previewSettings.value]);
        }
    }

    /// <summary>
    ///  Event handler for photo settings combo box. Updates stream resolution based on the selection.
    /// </summary>
    function photoSettings_changed() {
        if (isPreviewing) {
            // The first element is just text
            if (photoSettings.value == "")
                return;

            mediaCapture.videoDeviceController.setMediaStreamPropertiesAsync(Capture.MediaStreamType.videoPreview, photoProperties[photoSettings.value]);
        }
    }

    /// <summary>
    /// Takes a photo to and saves to a StorageFile
    /// </summary>
    function photoButton_click() {
        if (isPreviewing) {
            // Disable the photo button while taking a photo
            photoButton.disabled = true;

            var Storage = Windows.Storage;

            captureFolder.createFileAsync("SimplePhoto.jpg", Storage.CreationCollisionOption.generateUniqueName)
            .then(function (file) {
                return mediaCapture.capturePhotoToStorageFileAsync(Windows.Media.MediaProperties.ImageEncodingProperties.createJpeg(), file)
                .then(function () {
                    // Done taking the photo, so re-enable the button
                    photoButton.disabled = false;

                    logStatus("Photo taken, saved to: " + file.path, "status");
                })
            }, function (error) {
                logStatus(error.message, "error");
            }).done();
        }
    }

    /// <summary>
    /// On some devices there may not be seperate streams for preview/photo/video. In this case, changing any of them
    /// will change all of them since they are the same stream
    /// </summary>
    function checkIfStreamsAreIdentical() {
        if (mediaCapture.mediaCaptureSettings.videoDeviceCharacteristic == Capture.VideoDeviceCharacteristic.allStreamsIdentical ||
            mediaCapture.mediaCaptureSettings.videoDeviceCharacteristic == Capture.VideoDeviceCharacteristic.previewPhotoStreamsIdentical)
        {
            logStatus("Warning: Preview and photo streams for this device are identical, changing one will affect the other", "error");
        }
    }

    /// <summary>
    /// Populates the given combo box with all possible combinations of the given stream type settings returned by the camera driver
    /// </summary>
    /// <param name="streamType"></param>
    /// <param name="comboBox"></param>
    function populateComboBox(streamType, comboBox, showFrameRate) {
        // Query all properties of the device
        var allProperties = mediaCapture.videoDeviceController.getAvailableMediaStreamProperties(streamType);

        // Populate the combo box with the entries
        allProperties.forEach(function (properties, index) {
            var option = document.createElement("option");
            var streamResolution = new streamResolutionHelper(properties);
            option.innerHTML = streamResolution.getFriendlyName(showFrameRate);
            option.value = index;
            comboBox.appendChild(option);
        });

        return allProperties;
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
