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

(function () {
    "use strict";

    var Capture = Windows.Media.Capture;
    var DeviceInformation = Windows.Devices.Enumeration.DeviceInformation;
    var DeviceClass = Windows.Devices.Enumeration.DeviceClass;
    var DisplayOrientations = Windows.Graphics.Display.DisplayOrientations;
    var Imaging = Windows.Graphics.Imaging;
    var Media = Windows.Media;
    var StorageLibrary = Windows.Storage.StorageLibrary;
    var KnownLibraryId = Windows.Storage.KnownLibraryId;
    var ApplicationData = Windows.Storage.ApplicationData;

    // Receive notifications about rotation of the device and UI and apply any necessary rotation to the preview stream and UI controls
    var oDisplayInformation = Windows.Graphics.Display.DisplayInformation.getForCurrentView(),
        oDisplayOrientation = DisplayOrientations.landscape;

    // Prevent the screen from sleeping while the camera is running
    var oDisplayRequest = new Windows.System.Display.DisplayRequest();

    // For listening to media property changes
    var oSystemMediaControls = Media.SystemMediaTransportControls.getForCurrentView();

    // MediaCapture and its state variables
    var oMediaCapture = null,
        isInitialized = false,
        isPreviewing = false;

    // Information about the camera device
    var externalCamera = false,
        mirroringPreview = false;

    // Rotation metadata to apply to the preview stream and recorded videos (MF_MT_VIDEO_ROTATION)
    // Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
    var RotationKey = "C380465D-2271-428C-9B83-ECEA3B4A85C1";

    // Folder in which the captures will be stored (initialized in SetupUiAsync)
    var oCaptureFolder;

    // Initialization
    var app = WinJS.Application;
    var activation = Windows.ApplicationModel.Activation;
    app.onactivated = function (args) {
        if (args.detail.kind === activation.ActivationKind.launch) {
            if (args.detail.previousExecutionState !== activation.ApplicationExecutionState.terminated) {
                document.getElementById("getPreviewFrameButton").addEventListener("click", getPreviewFrameButton_tapped);
                previewFrameImage.src = "";
            }

            oDisplayInformation.addEventListener("orientationchanged", displayInformation_orientationChanged);
            initializeCameraAsync();
            args.setPromise(WinJS.UI.processAll());
        }
    };

    // About to be suspended
    app.oncheckpoint = function (args) {
        // Handling of this event is included for completeness, as it will only fire when navigating between pages and this sample only includes one page
        oDisplayInformation.removeEventListener("orientationchanged", displayInformation_orientationChanged);
        args.setPromise(cleanupCameraAsync());
    };

    // Closing
    app.onunload = function (args) {
        oDisplayInformation.removeEventListener("orientationchanged", displayInformation_orientationChanged);
        document.getElementById("getPreviewFrameButton").removeEventListener("click", getPreviewFrameButton_tapped);
        oSystemMediaControls.removeEventListener("propertychanged", systemMediaControls_PropertyChanged);

        args.setPromise(cleanupCameraAsync());
    };

    // Resuming from a user suspension
    Windows.UI.WebUI.WebUIApplication.addEventListener("resuming", function () {
        oDisplayInformation.addEventListener("orientationchanged", displayInformation_orientationChanged);
        initializeCameraAsync();
    }, false);

    /// <summary>
    /// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, starts preview and unlocks the UI
    /// </summary>
    /// <returns></returns>
    function initializeCameraAsync() {
        console.log("InitializeCameraAsync");

        // Get available devices for capturing pictures
        return findCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.back)
        .then(function (camera) {
            if (!camera) {
                console.log("No camera device found!");
                return;
            }
            // Figure out where the camera is located
            if (!camera.enclosureLocation || camera.enclosureLocation.panel === Windows.Devices.Enumeration.Panel.unknown) {
                // No information on the location of the camera, assume it's an external camera, not integrated on the device
                externalCamera = true;
            }
            else {
                // Camera is fixed on the device
                externalCamera = false;

                // Only mirror the preview if the camera is on the front panel
                mirroringPreview = (camera.enclosureLocation.panel === Windows.Devices.Enumeration.Panel.front);
            }

            oMediaCapture = new Capture.MediaCapture();

            // Register for a notification when something goes wrong
            oMediaCapture.addEventListener("failed", mediaCapture_failed);

            var settings = new Capture.MediaCaptureInitializationSettings();
            settings.videoDeviceId = camera.id;

            // Initialize media capture and start the preview
            return oMediaCapture.initializeAsync(settings);
        }, function (error) {
            console.log(error.message);
        }).then(function () {
            isInitialized = true;
            return startPreviewAsync();
        }).then(function () {
            return StorageLibrary.getLibraryAsync(KnownLibraryId.pictures)
        }).then(function (picturesLibrary) {
            // Fall back to the local app storage if the Pictures Library is not available
            oCaptureFolder = picturesLibrary.saveFolder || ApplicationData.current.localFolder;
        }).done();
    }

    /// <summary>
    /// Cleans up the camera resources (after stopping any video recording and/or preview if necessary) and unregisters from MediaCapture events
    /// </summary>
    /// <returns></returns>
    function cleanupCameraAsync() {
        console.log("cleanupCameraAsync");

        var promiseList = {};

        if (isInitialized) {
            if (isPreviewing) {
                // The call to stop the preview is included here for completeness, but can be
                // safely removed if a call to MediaCapture.close() is being made later,
                // as the preview will be automatically stopped at that point
                stopPreview();
            }

            isInitialized = false;
        }

        // When all our tasks complete, clean up MediaCapture
        return WinJS.Promise.join(promiseList)
        .then(function () {
            if (oMediaCapture != null) {
                oMediaCapture.removeEventListener("failed", mediaCapture_failed);
                oMediaCapture.close();
                oMediaCapture = null;
            }
        });
    }

    /// <summary>
    /// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on
    /// </summary>
    function startPreviewAsync() {
        // Prevent the device from sleeping while the preview is running
        oDisplayRequest.requestActive();

        // Register to listen for media property changes
        oSystemMediaControls.addEventListener("propertychanged", systemMediaControls_PropertyChanged);

        // Set the preview source in the UI and mirror it if necessary
        var previewVidTag = document.getElementById("cameraPreview");
        if (mirroringPreview) {
            cameraPreview.style.transform = "scale(-1, 1)";
        }

        var previewUrl = URL.createObjectURL(oMediaCapture);
        previewVidTag.src = previewUrl;
        previewVidTag.play();

        previewVidTag.addEventListener("playing", function () {
            isPreviewing = true;
            setPreviewRotationAsync();
        });
    }

    /// <summary>
    /// Gets the current orientation of the UI in relation to the device (when AutoRotationPreferences cannot be honored) and applies a corrective rotation to the preview
    /// </summary>
    /// <returns></returns>
    function setPreviewRotationAsync() {
        // Only need to update the orientation if the camera is mounted on the device
        if (externalCamera) {
            return WinJS.Promise.as();
        }

        // Calculate which way and how far to rotate the preview
        var rotationDegrees = convertDisplayOrientationToDegrees(oDisplayOrientation);

        // The rotation direction needs to be inverted if the preview is being mirrored
        if (mirroringPreview) {
            rotationDegrees = (360 - rotationDegrees) % 360;
        }

        // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames
        var props = oMediaCapture.videoDeviceController.getMediaStreamProperties(Capture.MediaStreamType.videoPreview);
        props.properties.insert(RotationKey, rotationDegrees);
        return oMediaCapture.setEncodingPropertiesAsync(Capture.MediaStreamType.videoPreview, props, null);
    }

    /// <summary>
    /// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes
    /// </summary>
    /// <returns></returns>
    function stopPreview() {
        isPreviewing = false;

        // Cleanup the UI
        var previewVidTag = document.getElementById("cameraPreview");
        previewVidTag.pause();
        previewVidTag.src = "";

        // Allow the device screen to sleep now that the preview is stopped
        oDisplayRequest.requestRelease();
    }

    /// <summary>
    /// Gets the current preview frame as a SoftwareBitmap, displays its properties in a TextBlock, and can optionally display the image
    /// in the UI and/or save it to disk as a jpg
    /// </summary>
    /// <returns></returns>
    function getPreviewFrameAsSoftwareBitmapAsync() {
        // Get information about the preview
        var previewProperties = oMediaCapture.videoDeviceController.getMediaStreamProperties(Capture.MediaStreamType.videoPreview);
        var videoFrameWidth = previewProperties.width;
        var videoFrameHeight = previewProperties.height;

        // Create the video frame to request a SoftwareBitmap preview frame
        var videoFrame = new Windows.Media.VideoFrame(Imaging.BitmapPixelFormat.bgra8, videoFrameWidth, videoFrameHeight);

        // Capture the preview frame
        return oMediaCapture.getPreviewFrameAsync(videoFrame)
        .then(function (currentFrame) {
            // Collect the resulting frame
            var frameBitmap = currentFrame.softwareBitmap;

            // Show the frame information
            frameInfoTextBlock.textContent = frameBitmap.pixelWidth + "x" + frameBitmap.pixelHeight + " " +
                stringOfEnumeration(Windows.Graphics.DirectX.DirectXPixelFormat, frameBitmap.bitmapPixelFormat);

            // Save and show the frame (as is, no rotation is being applied)
            if (saveShowFrameCheckBox.checked === true) {
                return oCaptureFolder.createFileAsync("PreviewFrame.jpg", Windows.Storage.CreationCollisionOption.generateUniqueName)
                .then(function (file) {
                    return saveAndShowSoftwareBitmapAsync(frameBitmap, file);
                })
            }
            else {
                return WinJS.Promise.as();
            }
        });
    }

    /// <summary>
    /// Gets the current preview frame as a Direct3DSurface and displays its properties in a TextBlock
    /// </summary>
    /// <returns></returns>
    function getPreviewFrameAsD3DSurfaceAsync() {
        // Capture the preview frame as a D3D surface
        return oMediaCapture.getPreviewFrameAsync()
        .then(function (currentFrame) {
            // Check that the Direct3DSurface isn't null. It's possible that the device does not support getting the frame
            // as a D3D surface
            if (currentFrame.direct3DSurface != null) {
                // Collect the resulting frame
                var surface = currentFrame.direct3DSurface;

                // Show the frame information
                frameInfoTextBlock.textContent = surface.description.width + "x" + surface.description.height + " " +
                    stringOfEnumeration(Windows.Graphics.DirectX.DirectXPixelFormat, surface.description.format);
            }
            else { // Fall back to software bitmap
                // Collect the resulting frame
                var frameBitmap = currentFrame.softwareBitmap;

                // Show the frame information
                frameInfoTextBlock.textContent = frameBitmap.pixelWidth + "x" + frameBitmap.pixelHeight + " " +
                    stringOfEnumeration(Windows.Graphics.DirectX.DirectXPixelFormat, frameBitmap.bitmapPixelFormat);
            }

            // Clear the image
            previewFrameImage.src = "";
        });
    }

    /// <summary>
    /// Saves a SoftwareBitmap to the specified StorageFile
    /// </summary>
    /// <param name="bitmap">SoftwareBitmap to save</param>
    /// <param name="file">Target StorageFile to save to</param>
    /// <returns></returns>
    function saveAndShowSoftwareBitmapAsync(bitmap, file) {
        var imageStream = null;
        return file.openAsync(Windows.Storage.FileAccessMode.readWrite)
        .then(function (outputStream) {
            imageStream = outputStream;
            return Imaging.BitmapEncoder.createAsync(Imaging.BitmapEncoder.jpegEncoderId, imageStream);
        }).then(function (encoder) {
            // Grab the data from the SoftwareBitmap
            encoder.setSoftwareBitmap(bitmap);
            return encoder.flushAsync();
        }).then(function () {
            var imageBlob = window.MSApp.createBlobFromRandomAccessStream("image/jpg", imageStream);
            // Finally display the image at the correct orientation
            previewFrameImage.src = URL.createObjectURL(imageBlob, { oneTimeOnly: true });
            previewFrameImage.style.transform = "rotate(" + convertDisplayOrientationToDegrees(oDisplayOrientation) + "deg)";
        })
    }


    /// <summary>
    /// Attempts to find and return a device mounted on the panel specified, and on failure to find one it will return the first device listed
    /// </summary>
    /// <param name="panel">The desired panel on which the returned device should be mounted, if available</param>
    /// <returns></returns>
    function findCameraDeviceByPanelAsync(panel) {
        var deviceInfo = null;
        // Get available devices for capturing pictures
        return DeviceInformation.findAllAsync(DeviceClass.videoCapture)
        .then(function (devices) {
            devices.forEach(function (cameraDeviceInfo) {
                if (cameraDeviceInfo.enclosureLocation != null && cameraDeviceInfo.enclosureLocation.panel === panel) {
                    deviceInfo = cameraDeviceInfo;
                    return;
                }
            });

            // Nothing matched, just return the first
            if (!deviceInfo && devices.length > 0) {
                deviceInfo = devices.getAt(0);
            }

            return deviceInfo;
        });
    }

    /// <summary>
    /// Converts the given orientation of the app on the screen to the corresponding rotation in degrees
    /// </summary>
    /// <param name="orientation">The orientation of the app on the screen</param>
    /// <returns>An orientation in degrees</returns>
    function convertDisplayOrientationToDegrees(orientation) {
        switch (orientation) {
            case DisplayOrientations.portrait:
                return 90;
            case DisplayOrientations.landscapeFlipped:
                return 180;
            case DisplayOrientations.portraitFlipped:
                return 270;
            case DisplayOrientations.landscape:
            default:
                return 0;
        }
    }

    /// <summary>
    /// This event will fire when the page is rotated, when the DisplayInformation.AutoRotationPreferences value set in the setupUiAsync() method cannot be not honored.
    /// </summary>
    /// <param name="sender">The event source.</param>
    function displayInformation_orientationChanged(args) {
        oDisplayOrientation = args.target.currentOrientation;

        if (isPreviewing) {
            setPreviewRotationAsync();
        }
    }

    function getPreviewFrameButton_tapped() {
        // If preview is not running, no preview frames can be acquired
        if (!isPreviewing) {
            return;
        }

        if (saveShowFrameCheckBox.checked === true) {
            getPreviewFrameAsSoftwareBitmapAsync().done();
        }
        else {
            getPreviewFrameAsD3DSurfaceAsync().done();
        }
    }

    /// <summary>
    /// In the event of the app being minimized this method handles media property change events. If the app receives a mute
    /// notification, it is no longer in the foregroud.
    /// </summary>
    /// <param name="args"></param>
    function systemMediaControls_PropertyChanged(args) {
        // Check to see if the app is being muted. If so, it is being minimized.
        // Otherwise if it is not initialized, it is being brought into focus.
        if (args.target.soundLevel === Media.SoundLevel.muted) {
            cleanupCameraAsync();
        }
        else if (!isInitialized) {
            initializeCameraAsync();
        }
    }

    function mediaCapture_failed(errorEventArgs) {
        console.log("MediaCapture_Failed: 0x" + errorEventArgs.code + ": " + errorEventArgs.message);

        cleanupCameraAsync().done();
    }

    /// <summary>
    /// Converts an enum to a readable string
    /// </summary>
    /// <param name="enumeration">The actual enumeration</param>
    /// <param name="enumeration">The value of the given enumeration</param>
    /// <returns>String of the enumeration value</returns>
    function stringOfEnumeration(enumeration, value) {
        for (var k in enumeration) if (enumeration[k] == value) {
            return k;
        }

        return null;
    }

    app.start();
})();
