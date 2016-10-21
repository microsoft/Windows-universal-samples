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

    // Constant to decide whether to use the capture and encoding recommendations from the Video Stabilization effect
    const usVsRecommendation = 1;

    var Capture = Windows.Media.Capture;
    var DeviceInformation = Windows.Devices.Enumeration.DeviceInformation;
    var DeviceClass = Windows.Devices.Enumeration.DeviceClass;
    var DisplayOrientations = Windows.Graphics.Display.DisplayOrientations;
    var FileProperties = Windows.Storage.FileProperties;
    var Media = Windows.Media;
    var SimpleOrientation = Windows.Devices.Sensors.SimpleOrientation;
    var SimpleOrientationSensor = Windows.Devices.Sensors.SimpleOrientationSensor;
    var StorageLibrary = Windows.Storage.StorageLibrary;
    var KnownLibraryId = Windows.Storage.KnownLibraryId;
    var ApplicationData = Windows.Storage.ApplicationData;

    // Receive notifications about rotation of the device and UI and apply any necessary rotation to the preview stream and UI controls
    var oOrientationSensor = SimpleOrientationSensor.getDefault(),
        oDisplayInformation = Windows.Graphics.Display.DisplayInformation.getForCurrentView(),
        oDeviceOrientation = SimpleOrientation.notRotated,
        oDisplayOrientation = DisplayOrientations.portrait;

    // Prevent the screen from sleeping while the camera is running
    var oDisplayRequest = new Windows.System.Display.DisplayRequest();

    // For listening to media property changes
    var oSystemMediaControls = Media.SystemMediaTransportControls.getForCurrentView();

    // MediaCapture and its state variables
    var oMediaCapture = null,
        oEncodingProfile = null,
        isInitialized = false,
        isPreviewing = false,
        isRecording = false;

    // Information about the camera device
    var externalCamera = false,
        mirroringPreview = false;

    // Stabilization effect and properties
    var oVideoStabilizationEffect = null,
        oInputPropertiesBackup = null,
        oOutputPropertiesBackup = null;
    
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
                document.getElementById("vsToggleButton").addEventListener("click", vsToggleButton_tapped);
                document.getElementById("videoButton").addEventListener("click", videoButton_tapped);
            }
            
            setupUiAsync();
            initializeCameraAsync();
            args.setPromise(WinJS.UI.processAll());
        }
    };
    
    // About to be suspended
    app.oncheckpoint = function (args) {
        cleanupCameraAsync()
        .then(function () {
            args.setPromise(cleanupUiAsync());
        }).done();
    };

    // Resuming from a user suspension
    Windows.UI.WebUI.WebUIApplication.addEventListener("resuming", function () {
        setupUiAsync();
        initializeCameraAsync();
    }, false);
    
    // Closing
    app.onunload = function (args) {
        document.getElementById("vsToggleButton").removeEventListener("click", vsToggleButton_tapped);
        document.getElementById("videoButton").removeEventListener("click", videoButton_tapped);

        cleanupCameraAsync()
        .then(function () {
            args.setPromise(cleanupUiAsync());
        }).done();
    };

    /// <summary>
    /// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, starts preview and unlocks the UI
    /// </summary>
    /// <returns></returns>
    function initializeCameraAsync() {
        console.log("InitializeCameraAsync");

        // Get available devices for capturing pictures
        return findCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.back)
        .then(function (camera) {
            if (camera === null) {
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

            // Register for a notification when video recording has reached the maximum time and when something goes wrong
            oMediaCapture.addEventListener("recordlimitationexceeded", mediaCapture_recordLimitationExceeded);
            oMediaCapture.addEventListener("failed", mediaCapture_failed);

            var settings = new Capture.MediaCaptureInitializationSettings();
            settings.videoDeviceId = camera.id;
            settings.streamingCaptureMode = Capture.StreamingCaptureMode.audioAndVideo;

            // Initialize media capture and start the preview
            return oMediaCapture.initializeAsync(settings)
            .then(function () {
                // Set up the encoding profile for video recording
                oEncodingProfile = Windows.Media.MediaProperties.MediaEncodingProfile.createMp4(Windows.Media.MediaProperties.VideoEncodingQuality.auto);
                isInitialized = true;

                startPreview();
            });
        }, function (error) {
            console.log(error.message);
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
            // If a recording is in progress during cleanup, stop it to save the recording
            if (isRecording) {
                var stopRecordPromise = stopRecordingAsync();
                promiseList[promiseList.length] = stopRecordPromise;
            }

            if (oVideoStabilizationEffect != null) {
                var cleanStabilizationTask = cleanUpVideoStabilizationEffectAsync();
                promiseList[promiseList.length] = cleanStabilizationTask;
            }

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
                oMediaCapture.removeEventListener("recordlimitationexceeded", mediaCapture_recordLimitationExceeded);
                oMediaCapture.removeEventListener("failed", mediaCapture_failed);
                oMediaCapture.close();
                oMediaCapture = null;
            }
        });
    }

    /// <summary>
    /// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on
    /// </summary>
    function startPreview() {
        // Prevent the device from sleeping while the preview is running
        oDisplayRequest.requestActive();

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
            updateCaptureControls();
            setPreviewRotationAsync();
        });
    }

    /// <summary>
    /// Gets the current orientation of the UI in relation to the device (when AutoRotationPreferences cannot be honored) and applies a corrective rotation to the preview
    /// </summary>
    /// <returns></returns>
    function setPreviewRotationAsync()
    {
        // Calculate which way and how far to rotate the preview
        var rotationDegrees = convertDisplayOrientationToDegrees(oDisplayOrientation);

        // The rotation direction needs to be inverted if the preview is being mirrored
        if (mirroringPreview)
        {
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
        previewVidTag.src = null;

        // Allow the device screen to sleep now that the preview is stopped
        oDisplayRequest.requestRelease();
    }

    /// <summary>
    /// Adds video stabilization to the video record stream, registers for its event, enables it, and gets the effect instance
    /// </summary>
    /// <returns></returns>
    function createVideoStabilizationEffectAsync() {
        // No work to be done if there already is an effect
        if (oVideoStabilizationEffect != null) {
            return WinJS.Promise.as();
        }

        // Create the definition, which will contain some initialization settings
        var definition = new Windows.Media.Core.VideoStabilizationEffectDefinition();

        // Add the effect to the video record stream 
        return oMediaCapture.addVideoEffectAsync(definition, Capture.MediaStreamType.videoRecord)
        .then(function (mediaExtension) {
            console.log("VS effect added to camera pipeline");

            oVideoStabilizationEffect = mediaExtension;

            // Subscribe to notifications for changes in the enabled state for the effect
            oVideoStabilizationEffect.addEventListener("enabledchanged", videoStabilizationEffect_EnabledChanged);

            // Enable the effect
            oVideoStabilizationEffect.enabled = true;

            if (usVsRecommendation) {
                // Configure the camera pipeline to use the optimal settings for VS
                return setUpVideoStabilizationRecommendationAsync();

                // At this point, one of two things has happened:
                //
                // a) If a more suitable capture resolution was available:
                //    1. Such resolution will have been set up for video capture ("input")
                //    2. The MediaEncodingProfile ("output") will have been changed to specify dimensions reflecting the amount of cropping
                //       done on said capture resolution (possibly even none if the new resolution offers enough padding)
                // b) If no better suited capture resolution was available:
                //    1. The video capture resolution will not have changed
                //    2. The MediaEncodingProfile will have been changed to specify smaller dimensions than the capture resolution
                //       so that the video isn't scaled back up to the capture resolution after cropping, which could cause a loss in quality
            }     
            else {
                console.log("Not setting up VS recommendation");

                // Not setting up VS recommendation means that the video will be captured at the desired resolution,
                // then cropped by the VS effect as part of the stabilization process, and then scaled back up to the
                // original capture resolution
                return WinJS.Promise.as();
            }
        });
    }

    /// <summary>
    /// Configures the camera pipeline to use the optimal resolutions for VS based on the settings currently in use
    /// </summary>
    /// <returns></returns>
    function setUpVideoStabilizationRecommendationAsync() {
        console.log("Setting up VS recommendation...");

        // Get the recommendation from the effect based on our current input and output configuration
        var recommendation = oVideoStabilizationEffect.getRecommendedStreamConfiguration(oMediaCapture.videoDeviceController, oEncodingProfile.video);

        // Handle the recommendations for the output from the effect
        if (recommendation.outputProperties != null) {
            // Back up the current output properties from before VS was activated
            oOutputPropertiesBackup = oEncodingProfile.video;

            // Apply the recommended encoding profile for the output, which will result in a video with the same dimensions as configured
            // before VideoStabilization was added if an appropriate padded capture resolution was available. Otherwise, it will be slightly
            // smaller (due to cropping). This prevents upscaling back to the original size, which can result in a loss of quality
            oEncodingProfile.video = recommendation.outputProperties;
            console.log("VS recommendation for the MediaEncodingProfile (output) has been applied");
        }

        // Handle the recommendation for the input into the effect, which can contain a larger resolution than currently configured, so cropping is minimized
        if (recommendation.inputProperties != null)
        {
            // Back up the current input properties from before VS was activated
            oInputPropertiesBackup = oMediaCapture.videoDeviceController.getMediaStreamProperties(Capture.MediaStreamType.videoRecord);

            // Set the recommendation from the effect (a resolution higher than the current one to allow for cropping) on the input
            return oMediaCapture.videoDeviceController.setMediaStreamPropertiesAsync(Capture.MediaStreamType.videoRecord, recommendation.inputProperties)
            .then(function ()
            {
                console.log("VS recommendation for the MediaStreamProperties (input) has been applied");
            });
        }
        else
        {
            return WinJS.Promise.as();
        }
    }

    /// <summary>
    ///  Disables and removes the video stabilization effect, and unregisters the event handler for the EnabledChanged event of the effect
    /// </summary>
    /// <returns></returns>
    function cleanUpVideoStabilizationEffectAsync() {
        // No work to be done if there is no effect
        if (!oVideoStabilizationEffect) {
            return WinJS.Promise.as();
        }

        // Disable the effect
        oVideoStabilizationEffect.enabled = false;

        oVideoStabilizationEffect.removeEventListener("enablechanged", videoStabilizationEffect_EnabledChanged);

        // Remove the effect (see clearEffectsAsync method to remove all effects from a stream)
        return oMediaCapture.removeEffectAsync(oVideoStabilizationEffect)
        .then(function () {
            console.log("VS effect removed from camera pipeline");

            // If backed up settings (stream properties and encoding profile) exist, restore them and clear the backups
            if (!oOutputPropertiesBackup) {
                oEncodingProfile.video = oOutputPropertiesBackup;
                oOutputPropertiesBackup = null;
            }

            var promiseToExecute = null;
            if (oInputPropertiesBackup != null) {
                promiseToExecute = oMediaCapture.videoDeviceController.setMediaStreamPropertiesAsync(Capture.MediaStreamType.videoRecord, oInputPropertiesBackup);
                oInputPropertiesBackup = null;
            }
            else {
                promiseToExecute = WinJS.Promise.as();
            }

            return promiseToExecute;
        }).then(function () {
            oVideoStabilizationEffect = null;
            return WinJS.Promise.as();
        });
    }

    /// <summary>
    /// Records an MP4 video to a StorageFile and adds rotation metadata to it
    /// </summary>
    /// <returns></returns>
    function startRecordingAsync() {
        return oCaptureFolder.createFileAsync("SimpleVideo.mp4", Windows.Storage.CreationCollisionOption.generateUniqueName)
        .then(function (file) {
            // Calculate rotation angle, taking mirroring into account if necessary
            var rotationAngle = 360 - convertDeviceOrientationToDegrees(getCameraOrientation());
            var encodingProfile = Windows.Media.MediaProperties.MediaEncodingProfile.createMp4(Windows.Media.MediaProperties.VideoEncodingQuality.auto);
            encodingProfile.video.properties.insert(RotationKey, rotationAngle);

            console.log("Starting recording to " + file.path);
            return oMediaCapture.startRecordToStorageFileAsync(encodingProfile, file)
            .then(function () {
                isRecording = true;
                console.log("Started recording!");
            });
        });
    }

    /// <summary>
    /// Stops recording a video
    /// </summary>
    /// <returns></returns>
    function stopRecordingAsync() {
        console.log("Stopping recording...");
        return oMediaCapture.stopRecordAsync()
        .then(function () {
            isRecording = false;
            console.log("Stopped recording!");

            if (oVideoStabilizationEffect != null) {
                if (!oVideoStabilizationEffect.enabled) {
                    return cleanUpVideoStabilizationEffectAsync();
                }
            }
            return WinJS.Promise.as();
        });
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
    /// Applies the given orientation to a photo stream and saves it as a StorageFile
    /// </summary>
    /// <param name="stream">The photo stream</param>
    /// <param name="photoOrientation">The orientation metadata to apply to the photo</param>
    /// <returns></returns>
    function reencodeAndSavePhotoAsync(inputStream, file, orientation) {
        var Imaging = Windows.Graphics.Imaging;
        var bitmapDecoder = null,
            bitmapEncoder = null,
            outputStream = null;

        return Imaging.BitmapDecoder.createAsync(inputStream)
        .then(function (decoder) {
            bitmapDecoder = decoder;
            return file.openAsync(Windows.Storage.FileAccessMode.readWrite);
        }).then(function (outStream) {
            outputStream = outStream;
            return Imaging.BitmapEncoder.createForTranscodingAsync(outputStream, bitmapDecoder);
        }).then(function (encoder) {
            bitmapEncoder = encoder;
            var properties = new Imaging.BitmapPropertySet();
            properties.insert("System.Photo.Orientation", new Imaging.BitmapTypedValue(orientation, Windows.Foundation.PropertyType.uint16));
            return bitmapEncoder.bitmapProperties.setPropertiesAsync(properties)
        }).then(function () {
            return bitmapEncoder.flushAsync();
        }).then(function () {
            inputStream.close();
            outputStream.close();
        });
    }

    /// <summary>
    /// This method will update the icons, enable/disable and show/hide the photo/video buttons depending on the current state of the app and the capabilities of the device
    /// </summary>
    function updateCaptureControls() {
        // The button should only be enabled if the preview started sucessfully
        videoButton.disabled = !isPreviewing;

        // Allow toggling VS only while not recording or when the effect exists but it has been disabled
        if (isPreviewing && !isRecording) {
            vsToggleButton.disabled = false;
        }
        else {
            if (oVideoStabilizationEffect != null) {
                if (oVideoStabilizationEffect.enabled === false) {
                    vsToggleButton.disabled = false;
                } else {
                    vsToggleButton.disabled = true;
                }
            }
            else {
                vsToggleButton.disabled = true;
            }
        }

        // Update recording button to show "Stop" icon instead of red "Record" icon
        var vidButton = document.getElementById("videoButton").winControl;
        if (isRecording) {
            vidButton.icon = "stop";
        }
        else {
            vidButton.icon = "video";
        }

        // Show "VS On" when effect exists and is enabled, otherwise show "VS Off"
        var vsButton = document.getElementById("vsToggleButton").winControl;
        if (oVideoStabilizationEffect != null)
        {
            if (oVideoStabilizationEffect.enabled === false) {
                vsButton.icon = "remote";
                vsButton.label = "VS Off";
            }
            else {
                vsButton.icon = "link";
                vsButton.label = "VS On";
            }
        }
        else
        {
            vsButton.icon = "remote";
            vsButton.label = "VS Off";
        }
    }
    
    /// <summary>
    /// Attempts to lock the page orientation, hide the StatusBar (on Phone) and registers event handlers for orientation sensors
    /// </summary>
    function setupUiAsync() {
        var Display = Windows.Graphics.Display;

        // Attempt to lock page to landscape orientation to prevent the CaptureElement from rotating, as this gives a better experience
        Display.DisplayInformation.autoRotationPreferences = Display.DisplayOrientations.landscape;
        
        registerEventHandlers();

        // Populate orientation variables with the current state
        oDisplayOrientation = oDisplayInformation.currentOrientation;
        if (oOrientationSensor != null) {
            oDeviceOrientation = oOrientationSensor.getCurrentOrientation();
        }

        return StorageLibrary.getLibraryAsync(KnownLibraryId.pictures)
        .then(function (picturesLibrary) {
            // Fall back to the local app storage if the Pictures Library is not available
            oCaptureFolder = picturesLibrary.saveFolder || ApplicationData.current.localFolder;

            // Hide the status bar
            if (Windows.Foundation.Metadata.ApiInformation.isTypePresent("Windows.UI.ViewManagement.StatusBar")) {
                return Windows.UI.ViewManagement.StatusBar.getForCurrentView().hideAsync();
            }
            else {
                return WinJS.Promise.as();
            }
        })
    }

    /// <summary>
    /// Unregisters event handlers for orientation sensors, allows the StatusBar (on Phone) to show, and removes the page orientation lock
    /// </summary>
    /// <returns></returns>
    function cleanupUiAsync() {
        unregisterEventHandlers();
        
        // Revert orientation preferences
        oDisplayInformation.AutoRotationPreferences = DisplayOrientations.none;

        // Show the status bar
        if (Windows.Foundation.Metadata.ApiInformation.isTypePresent("Windows.UI.ViewManagement.StatusBar")) {
            return Windows.UI.ViewManagement.StatusBar.getForCurrentView().showAsync();
        }
        else {
            return WinJS.Promise.as();
        }
    }

    /// <summary>
    /// Registers event handlers for orientation sensors, and performs an initial update of the UI rotation
    /// </summary>
    function registerEventHandlers()
    {
        // If there is an orientation sensor present on the device, register for notifications
        if (oOrientationSensor != null) {
            oOrientationSensor.addEventListener("orientationchanged", orientationSensor_orientationChanged);
        
            // Update orientation of buttons with the current orientation
            updateButtonOrientation();
        }

        oDisplayInformation.addEventListener("orientationchanged", displayInformation_orientationChanged);
        oSystemMediaControls.addEventListener("propertychanged", systemMediaControls_PropertyChanged);
    }

    /// <summary>
    /// Unregisters event handlers for orientation sensors
    /// </summary>
    function unregisterEventHandlers()
    {
        if (oOrientationSensor != null) {
            oOrientationSensor.removeEventListener("orientationchanged", orientationSensor_orientationChanged);
        }

        oDisplayInformation.removeEventListener("orientationchanged", displayInformation_orientationChanged);
        oSystemMediaControls.removeEventListener("propertychanged", systemMediaControls_PropertyChanged);
    }

    /// <summary>
    /// Calculates the current camera orientation from the device orientation by taking into account whether the camera is external or facing the user
    /// </summary>
    /// <returns>The camera orientation in space, with an inverted rotation in the case the camera is mounted on the device and is facing the user</returns>
    function getCameraOrientation() {
        if (externalCamera) {
            // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
            return SimpleOrientation.notRotated;
        }

        var result = oDeviceOrientation;

        // Account for the fact that, on portrait-first devices, the camera sensor is mounted at a 90 degree offset to the native orientation
        if (oDisplayInformation.nativeOrientation === DisplayOrientations.portrait) {
            switch (result) {
                case SimpleOrientation.rotated90DegreesCounterclockwise:
                    result = SimpleOrientation.notRotated;
                    break;
                case SimpleOrientation.rotated180DegreesCounterclockwise:
                    result = SimpleOrientation.rotated90DegreesCounterclockwise;
                    break;
                case SimpleOrientation.rotated270DegreesCounterclockwise:
                    result = SimpleOrientation.rotated180DegreesCounterclockwise;
                    break;
                case SimpleOrientation.notRotated:
                default:
                    result = SimpleOrientation.rotated270DegreesCounterclockwise;
                    break;
            }
        }

        // If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
        if (mirroringPreview) {
            // This only affects the 90 and 270 degree cases, because rotating 0 and 180 degrees is the same clockwise and counter-clockwise
            switch (result) {
                case SimpleOrientation.rotated90DegreesCounterclockwise:
                    return SimpleOrientation.rotated270DegreesCounterclockwise;
                case SimpleOrientation.rotated270DegreesCounterclockwise:
                    return SimpleOrientation.rotated90DegreesCounterclockwise;
            }
        }

        return result;
    }

    /// <summary>
    /// Converts the given orientation of the device in space to the metadata that can be added to captured photos
    /// </summary>
    /// <param name="orientation">The orientation of the device in space</param>
    /// <returns></returns>
    function convertOrientationToPhotoOrientation(orientation) {
        switch (orientation) {
            case SimpleOrientation.rotated90DegreesCounterclockwise:
                return FileProperties.PhotoOrientation.rotate90;
            case SimpleOrientation.rotated180DegreesCounterclockwise:
                return FileProperties.PhotoOrientation.rotate180;
            case SimpleOrientation.rotated270DegreesCounterclockwise:
                return FileProperties.PhotoOrientation.rotate270;
            case SimpleOrientation.notRotated:
            default:
                return FileProperties.PhotoOrientation.normal;
        }
    }

    /// <summary>
    /// Converts the given orientation of the device in space to the corresponding rotation in degrees
    /// </summary>
    /// <param name="orientation">The orientation of the device in space</param>
    /// <returns>An orientation in degrees</returns>
    function convertDeviceOrientationToDegrees(orientation) {
        switch (orientation) {
            case SimpleOrientation.rotated90DegreesCounterclockwise:
                return 90;
            case SimpleOrientation.rotated180DegreesCounterclockwise:
                return 180;
            case SimpleOrientation.rotated270DegreesCounterclockwise:
                return 270;
            case SimpleOrientation.notRotated:
            default:
                return 0;
        }
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
            case DisplayOrientations.LandscapeFlipped:
                return 180;
            case DisplayOrientations.PortraitFlipped:
                return 270;
            case DisplayOrientations.Landscape:
            default:
                return 0;
        }
    }

    /// <summary>
    /// Uses the current device orientation in space and page orientation on the screen to calculate the rotation
    /// transformation to apply to the controls
    /// </summary>
    function updateButtonOrientation() {
        var currDeviceOrientation = convertDeviceOrientationToDegrees(oDeviceOrientation);
        var currDisplayOrientation = convertDisplayOrientationToDegrees(oDisplayOrientation);

        if (oDisplayInformation.nativeOrientation === DisplayOrientations.portrait) {
            currDeviceOrientation -= 90;
        }

        // Combine both rotations and make sure that 0 <= result < 360
        var angle = (360 + currDisplayOrientation + currDeviceOrientation) % 360;

        // Rotate the buttons in the UI to match the rotation of the device
        videoButton.style.transform = "rotate(" + angle + "deg)";
        vsToggleButton.style.transform = "rotate(" + angle + "deg)";
    }

    /// <summary>
    /// This event will fire when the page is rotated, when the DisplayInformation.AutoRotationPreferences value set in the setupUiAsync() method cannot be not honored.
    /// </summary>
    /// <param name="sender">The event source.</param>
    function displayInformation_orientationChanged(sender) {
        oDisplayOrientation = sender.currentOrientation;

        if (isPreviewing) {
            setPreviewRotationAsync();
        }
       
        updateButtonOrientation();
    }

    function vsToggleButton_tapped() {
        // Note that for the most part, this button is disabled during recording, except when VS is turned off automatically
        var promiseToExecute = null;

        if (!isRecording)
        {
            // While *not* recording, either the effect doesn't exist (and should be created) or it exists and is enabled.
            // This sample doesn't allow the user to disable the effect while recording. For the case that the effect disables
            // itself while recording, StopRecordingAsync() will clean up the effect after the recording session ends.

            if (oVideoStabilizationEffect == null) {
                // VS didn't exist, so create and enable the effect
                promiseToExecute = createVideoStabilizationEffectAsync();
            }
            else if (oVideoStabilizationEffect.enabled) {
                // VS was enabled, so clean it up and restore any capture settings (MediaStreamType and EncodingProfile)
                promiseToExecute = cleanUpVideoStabilizationEffectAsync();
            }
            else {
                promiseToExecute = WinJS.Promise.as();
            }
        }
        else if (!oVideoStabilizationEffect.enabled) {
            // While recording this button is diabled. The situation the user is allowed to tap it while recording, is when
            // VS turns itself off (see VideoStabilizationEffect_EnabledChanged), so proceed to re-enable the effect
            oVideoStabilizationEffect.enabled = true;
            promiseToExecute = WinJS.Promise.as();
        }
        else { // Button enabled during recording
            console.log("Warning: VS toggle button enabled during recording");
            promiseToExecute = WinJS.Promise.as();
        }

        promiseToExecute.then(function() {
            updateCaptureControls();
        }).done();
    }

    function videoButton_tapped() {
        var promiseToExecute = null;
        if (!isRecording) {
            promiseToExecute = startRecordingAsync();
        }
        else {
            promiseToExecute = stopRecordingAsync();
        }
        
        promiseToExecute
        .then(function () {
            updateCaptureControls();
        }, function (error) {
            console.log(error.message);
        }).done();
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

    /// <summary>
    /// Triggers when the Enabled property of the VideoStabilizationEffect changes.
    /// </summary>
    /// <param name="sender">The instance of the effect firing the event.</param>
    /// <param name="args">Information about the event.</param>
    function videoStabilizationEffect_EnabledChanged(sender, args) {
        console.log("VS Enabled: " + sender.target.enabled + ". Reason: " + sender.reason);
        updateCaptureControls();
    }

    /// <summary>
    /// Occurs each time the simple orientation sensor reports a new sensor reading.
    /// </summary>
    /// <param name="args">The event data.</param>
    function orientationSensor_orientationChanged(args) {
        // If the device is parallel to the ground, keep the last orientation used. This allows users to take pictures of documents (FaceUp)
        // or the ceiling (FaceDown) in any orientation, by first holding the device in the desired orientation, and then pointing the camera
        // at the desired subject.
        if (args.orientation != SimpleOrientation.faceup && args.orientation != SimpleOrientation.facedown) {
            oDeviceOrientation = args.orientation;
            updateButtonOrientation();
        }
    }

    /// <summary>
    /// This is a notification that recording has to stop, and the app is expected to finalize the recording
    /// </summary>
    function mediaCapture_recordLimitationExceeded() {
        stopRecordingAsync()
        .done(function () {
            updateCaptureControls();
        });
    }

    function mediaCapture_failed(errorEventArgs)
    {
        console.log("MediaCapture_Failed: 0x" + errorEventArgs.code + ": " + errorEventArgs.message);

        cleanupCameraAsync()
        .done(function() {
            updateCaptureControls();
        });    
    }

    app.start();
})();
