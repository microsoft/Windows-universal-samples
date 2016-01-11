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
    var MediaProperties = Windows.Media.MediaProperties;

    var mediaCapture = null,
        previewProperties = [],
        videoProperties = [];

    var isPreviewing = false;
    var isRecording = false;

    var page = WinJS.UI.Pages.define("/html/Scenario3_AspectRatio.html", {
        ready: function (element, options) {
            document.getElementById("initCameraBtn").addEventListener("click", initCameraBtn_click, false);
            document.getElementById("previewSettings").addEventListener("change", previewSettings_changed, false);
            document.getElementById("videoSettings").addEventListener("change", videoSettings_changed, false);
            document.getElementById("videoButton").addEventListener("click", videoButton_click, false);
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
        var cleanupPromise = null;
        if (isRecording) {
            cleanupPromise = mediaCapture.stopRecordAsync();
            isRecording = false;
        }
        else {
            cleanupPromise = WinJS.Promise.as();
        }

        cleanupPromise.then(function () {
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
        });
    }

    /// <summary>
    /// Initializes the camera and populates the UI
    /// </summary>
    function initCameraBtn_click() {
        initializeCameraAsync()
        .then(function () {
            initCameraBtn.style.visibility = "hidden";
            checkIfStreamsAreIdentical();
            populateComboBoxes();
            videoButton.disabled = false;
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

            // The preview just changed, update the video combo box
            MatchPreviewAspectRatio();
        }
    }

    /// <summary>
    ///  Event handler for video settings combo box. Updates stream resolution based on the selection.
    /// </summary>
    function videoSettings_changed() {
        if (isPreviewing) {
            // The first element is just text
            if (videoSettings.value == "")
                return;

            mediaCapture.videoDeviceController.setMediaStreamPropertiesAsync(Capture.MediaStreamType.videoRecord, videoProperties[videoSettings.value]);
        }
    }

    /// <summary>
    /// Records an MP4 video to a StorageFile
    /// </summary>
    function videoButton_click() {
        if (isPreviewing) {
            if (!isRecording) {
                var Storage = Windows.Storage;
                var file = null;

                // Create a storage file and begin recording
                Storage.KnownFolders.videosLibrary.createFileAsync("SimpleVideo.mp4", Storage.CreationCollisionOption.generateUniqueName)
                .then(function (videoFile) {
                    file = videoFile;
                    var encodingProfile = MediaProperties.MediaEncodingProfile.createMp4(MediaProperties.VideoEncodingQuality.auto);
                    mediaCapture.startRecordToStorageFileAsync(encodingProfile, videoFile)
                    .then(function () {
                        // Reflect changes in the UI
                        videoButton.innerHTML = "Stop Video";
                        isRecording = true;

                        logStatus("Recording file, saving to: " + file.path, "status");
                    });
                }, function (error) {
                    logStatus(error.message, "error");
                });
            }
            else {
                // Reflect the changes in the UI and stop recording
                videoButton.innerHTML = "Record Video";
                isRecording = false;

                mediaCapture.stopRecordAsync().then(function () {
                    logStatus("Stopped recording", "status");
                })
            }
        }
    }

    /// <summary>
    /// On some devices there may not be seperate streams for preview/photo/video. In this case, changing any of them
    /// will change all of them since they are the same stream
    /// </summary>
    function checkIfStreamsAreIdentical() {
        if (mediaCapture.mediaCaptureSettings.videoDeviceCharacteristic == Capture.VideoDeviceCharacteristic.allStreamsIdentical ||
            mediaCapture.mediaCaptureSettings.videoDeviceCharacteristic == Capture.VideoDeviceCharacteristic.previewRecordStreamsIdentical) {
            logStatus("Warning: Preview and video streams for this device are identical, changing one will affect the other", "error");
        }
    }

    /// <summary>
    /// Small helper to add an option to the given comboBox
    /// </summary>
    /// <param name="comboBox"></param>
    /// <param name="text"></param>
    function addOptionToComboBox(comboBox, text, index) {
        var option = document.createElement("option");
        option.innerHTML = text;
        option.value = index;
        comboBox.appendChild(option);
    }

    /// <summary>
    /// Populates the combo boxes with preview settings and matching ratio settings for the video stream
    /// </summary>
    function populateComboBoxes() {
        // Query all properties preview properties of the device
        previewProperties = mediaCapture.videoDeviceController.getAvailableMediaStreamProperties(Capture.MediaStreamType.videoPreview);

        // Populate the combo box with the entries
        previewProperties.forEach(function (properties, index) {
            var streamResolution = new streamResolutionHelper(properties);
            addOptionToComboBox(previewSettings, streamResolution.getFriendlyName(true), index);
        });

        MatchPreviewAspectRatio();
    }

    /// <summary>
    /// Finds all the available video resolutions that match the aspect ratio of the preview stream
    /// Note: This should also be done with photos as well. This same method can be modified for photos
    /// by changing the MediaStreamType from VideoPreview to Photo.
    /// </summary>
    function MatchPreviewAspectRatio() {
        // Clear out old entries leaving only the default
        videoSettings.options.length = 1;

        // Query all video properties of the device
        videoProperties = mediaCapture.videoDeviceController.getAvailableMediaStreamProperties(Capture.MediaStreamType.videoRecord);

        // Query the current preview settings
        var previewProperties = new streamResolutionHelper(mediaCapture.videoDeviceController.getMediaStreamProperties(Capture.MediaStreamType.videoPreview));

        // Get all formats that have the same-ish aspect ratio as the preview and create new entries in the UI
        // Allow for some tolerance in the aspect ratio comparison
        const ASPECT_RATIO_TOLERANCE = 0.015;
        videoProperties.forEach(function (properties) {
            var streamHelper = new streamResolutionHelper(properties);
            if (Math.abs(streamHelper.aspectRatio() - previewProperties.aspectRatio()) < ASPECT_RATIO_TOLERANCE) {
                addOptionToComboBox(videoSettings, streamHelper.getFriendlyName(true));
            }
        })
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
