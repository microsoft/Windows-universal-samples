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

(function() {
    "use strict";

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
        isInitialized = false,
        isPreviewing = false;

    // Holds the index of the current Advanced Capture mode (-1 for no Advanced Capture active)
    var advancedCaptureMode = -1;

    // Information about the camera device
    var externalCamera = false,
        mirroringPreview = false;

    // The value at which the certainty from the HDR analyzer maxes out in the graphical representation
    const certaintyCap = 0.7;

    // Advanced Capture and Scene Analysis instances
    var oAdvancedCapture = null;
    var oSceneAnalysisEffect = null;

    // Rotation metadata to apply to the preview stream and recorded videos (MF_MT_VIDEO_ROTATION)
    // Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
    var RotationKey = "C380465D-2271-428C-9B83-ECEA3B4A85C1";

    // Folder in which the captures will be stored (initialized in SetupUiAsync)
    var oCaptureFolder;

    // Initialization
    var app = WinJS.Application;
    var activation = Windows.ApplicationModel.Activation;
    app.onactivated = function(args) {
        if (args.detail.kind === activation.ActivationKind.launch) {
            if (args.detail.previousExecutionState !== activation.ApplicationExecutionState.terminated) {
                document.getElementById("photoButton").addEventListener("click", photoButton_tapped);
                document.getElementById("cycleModeButton").addEventListener("click", cycleAdvancedCaptureMode);

                // Set the maximum on the progress bar
                hdrImpactBar.max = certaintyCap;
            }

            setupUiAsync();
            initializeCameraAsync();
            args.setPromise(WinJS.UI.processAll());
        }
    };

    // About to be suspended
    app.oncheckpoint = function(args) {
        cleanupCameraAsync()
            .then(function() {
                args.setPromise(cleanupUiAsync());
            }).done();
    };

    // Resuming from a user suspension
    Windows.UI.WebUI.WebUIApplication.addEventListener("resuming", function() {
        setupUiAsync();
        initializeCameraAsync();
    }, false);

    // Closing
    app.onunload = function(args) {
        document.getElementById("photoButton").removeEventListener("click", photoButton_tapped);
        document.getElementById("cycleModeButton").removeEventListener("click", cycleAdvancedCaptureMode);

        cleanupCameraAsync()
        .then(function() {
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
            .then(function(camera) {
                if (camera === null) {
                    console.log("No camera device found!");
                    return;
                }
                // Figure out where the camera is located
                if (!camera.enclosureLocation || camera.enclosureLocation.panel === Windows.Devices.Enumeration.Panel.unknown) {
                    // No information on the location of the camera, assume it's an external camera, not integrated on the device
                    externalCamera = true;
                } else {
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
                settings.streamingCaptureMode = Capture.StreamingCaptureMode.audioAndVideo;

                // Initialize media capture and start the preview
                return oMediaCapture.initializeAsync(settings)
                .then(function() {
                    isInitialized = true;

                    // Start the preview, kick off scene analysis, and enable Advanced Capture
                    startPreview();

                    return createSceneAnalysisEffectAsync()
                    .then(function() {
                        return enableAdvancedCaptureAsync();
                    });
                });
            }, function(error) {
                console.log(error.message);
            }).then(function() {
                // Update the controls to reflect the current state of the app
                updateUi();
            }).done();
    }

    /// <summary>
    /// Cleans up the camera resources (after stopping preview if necessary) and unregisters from MediaCapture events
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

            if (oAdvancedCapture) {
                var disableAdvancedCapturePromise = disableAdvancedCaptureAsync();
                promiseList[promiseList.length] = disableAdvancedCapturePromise;
            }

            if (oSceneAnalysisEffect) {
                var cleanSceneAnalysisPromise = cleanSceneAnalysisEffectAsync();
                promiseList[promiseList.length] = cleanSceneAnalysisPromise;
            }

            isInitialized = false;
        }

        // When all our tasks complete, clean up MediaCapture
        return WinJS.Promise.join(promiseList)
            .then(function() {
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

        previewVidTag.addEventListener("playing", function() {
            isPreviewing = true;
            updateUi();
            setPreviewRotationAsync();
        });
    }

    /// <summary>
    /// Gets the current orientation of the UI in relation to the device (when AutoRotationPreferences cannot be honored) and applies a corrective rotation to the preview
    /// </summary>
    /// <returns></returns>
    function setPreviewRotationAsync() {
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
        previewVidTag.src = null;

        // Allow the device screen to sleep now that the preview is stopped
        oDisplayRequest.requestRelease();
    }

    /// <summary>
    /// Adds scene analysis to the video preview stream, registers for its event, enables it, and gets the effect instance
    /// </summary>
    /// <returns></returns>
    function createSceneAnalysisEffectAsync() {
        // Create the definition, which will contain some initialization settings
        var definition = new Media.Core.SceneAnalysisEffectDefinition();

        // Add the effect to the video preview stream
        return oMediaCapture.addVideoEffectAsync(definition, Capture.MediaStreamType.videoPreview)
        .then(function(effect) {
            oSceneAnalysisEffect = effect;

            console.log("SA effect added to pipeline");

            // Subscribe to notifications about scene information
            oSceneAnalysisEffect.addEventListener("sceneanalyzed", sceneAnalysisEffect_sceneAnalyzed);

            // Enable HDR analysis
            oSceneAnalysisEffect.highDynamicRangeAnalyzer.enabled = true;
        });
    }

    /// <summary>
    /// Disables and removes the scene analysis effect, and unregisters the event handler for the SceneAnalyzed event of the effect
    /// </summary>
    /// <returns></returns>
    function cleanSceneAnalysisEffectAsync() {
        // Disable detection and unsubscribe from the events
        oSceneAnalysisEffect.highDynamicRangeAnalyzer.enabled = false;
        oSceneAnalysisEffect.removeEventListener("sceneanalyzed", sceneAnalysisEffect_sceneAnalyzed);

        // Remove the effect from the pipeline (see ClearEffectsAsync method to remove all effects from a given stream)
        return oMediaCapture.removeEffectAsync(oSceneAnalysisEffect)
        .then(function() {
            console.log("SceneAnalysis effect removed from pipeline");

            // Clear the member variable that held the effect instance
            oSceneAnalysisEffect = null;
        });
    }

    /// <summary>
    /// Creates an instance of the AdvancedPhotoCapture and registers for its events
    /// </summary>
    /// <returns></returns>
    function enableAdvancedCaptureAsync() {
        // No work to be done if there already is an AdvancedCapture
        if (oAdvancedCapture != null) {
            return WinJS.Promise.as();
        }

        // Configure one of the modes in the control
        cycleAdvancedCaptureMode();

        // Prepare for an advanced capture
        return oMediaCapture.prepareAdvancedPhotoCaptureAsync(Media.MediaProperties.ImageEncodingProperties.createJpeg())
            .then(function(advancedCapture) {
                console.log("EnabledAdvanced Capture");

                oAdvancedCapture = advancedCapture;

                // Register for events published by the AdvancedCapture
                oAdvancedCapture.addEventListener("allphotoscaptured", advancedCapture_allPhotosCaptured);
                oAdvancedCapture.addEventListener("optionalreferencephotocaptured", advancedCapture_optionalReferencePhotoCaptured);
            });
    }

    /// <summary>
    /// Configures the AdvancedPhotoControl to the next supported mode
    /// </summary>
    /// <remarks>
    /// Note that this method can be safely called regardless of whether the AdvancedPhotoCapture is in the "prepared
    /// state" or not. Internal changes to the mode will be applied  when calling Prepare, or when calling Capture if
    /// the mode has been changed since the call to Prepare. This allows for fast changing of desired capture modes,
    /// that can more quickly adapt to rapidly changing shooting conditions.
    /// </remarks>
    function cycleAdvancedCaptureMode()
    {
        // Calculate the index for the next supported mode
        advancedCaptureMode = (advancedCaptureMode + 1) % oMediaCapture.videoDeviceController.advancedPhotoControl.supportedModes.length;


        // Configure the settings object to the mode at the calculated index
        var settings = new Media.Devices.AdvancedPhotoCaptureSettings();
        settings.mode = oMediaCapture.videoDeviceController.advancedPhotoControl.supportedModes[advancedCaptureMode];

        // Configure the mode on the control
        oMediaCapture.videoDeviceController.advancedPhotoControl.configure(settings);

        // Update the button text to reflect the current mode
        var mode = stringOfEnumeration(Media.Devices.AdvancedPhotoMode, oMediaCapture.videoDeviceController.advancedPhotoControl.mode);
        cycleModeButton.winControl.label = mode;
    }

    /// <summary>
    /// Cleans up the instance of the AdvancedCapture
    /// </summary>
    /// <returns></returns>
    function disableAdvancedCaptureAsync() {
        // No work to be done if there is no AdvancedCapture
        if (oAdvancedCapture === null) {
            return WinJS.Promise.as();
        }

        return oAdvancedCapture.finishAsync()
        .then(function() {
            oAdvancedCapture = null;

            // Reset the Advanced Capture Mode index
            advancedCaptureMode = -1;

            console.log("Disabled Advanced Capture");
        });
    }

    /// <summary>
    /// Takes an Advanced Capture photo to a StorageFile and adds rotation metadata to it
    /// </summary>
    /// <returns></returns>
    function takeAdvancedCaptureAsync() {
        photoButton.disabled = true;
        cycleModeButton.disabled = true;

        var captureFile = null;
        // Take the picture
        console.log("Taking Advanced Capture photo...");

        // Create a context object to identify the capture in the OptionalReferencePhotoCaptured event
        var context = new Windows.Foundation.Collections.PropertySet();
        context.insert("fileName", "AdvancedCapturePhoto_" + getTimeStr() + ".jpg");
        context.insert("orientation", convertOrientationToPhotoOrientation(getCameraOrientation()));

        return oCaptureFolder.createFileAsync(context.fileName, Windows.Storage.CreationCollisionOption.GenerateUniqueName)
        .then(function (file) {
            captureFile = file;
            return oAdvancedCapture.captureAsync(context);
        }) //TODO error function for file IO
        .then(function (advancedCapturedPhoto) {
            console.log("Advanced Capture photo taken! Saving to " + captureFile.path);
            return reencodeAndSavePhotoAsync(advancedCapturedPhoto.frame, captureFile, context.orientation);
        })
        .then(function() {
            // Re-enable the buttons to reflect the current state of the app
            updateUi();
        });
    }

    /// <summary>
    /// Helper that gets the current time in HH-MM-SS format
    /// </summary>
    /// <returns>String in HH-MM-SS format</returns>
    function getTimeStr() {
        var now = new Date();
        var str = ("0" + now.getHours()).slice(-2) + "-" +
            ("0" + now.getMinutes()).slice(-2) + "-" +
            ("0" + now.getSeconds()).slice(-2);
        return str;
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
        }).then(function() {
            return bitmapEncoder.flushAsync();
        }).then(function () {
            inputStream.close();
            outputStream.close();
        });
    }

    /// <summary>
    /// This method will update the icons, enable/disable and show/hide the photo/video buttons depending on the current state of the app and the capabilities of the device
    /// </summary>
    function updateUi() {
        // The buttons should only be enabled if the preview started sucessfully
        photoButton.disabled = !isPreviewing;
        cycleModeButton.disabled = !isPreviewing;
    }

    /// <summary>
    /// Attempts to lock the page orientation, hide the StatusBar (on Phone) and registers event handlers for hardware buttons and orientation sensors
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
    /// Unregisters event handlers for hardware buttons and orientation sensors, allows the StatusBar (on Phone) to show, and removes the page orientation lock
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
    /// Registers event handlers for hardware buttons and orientation sensors, and performs an initial update of the UI rotation
    /// </summary>
    function registerEventHandlers()
    {
        if (Windows.Foundation.Metadata.ApiInformation.isTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
        {
            Windows.Phone.UI.Input.HardwareButtons.addEventListener("camerapressed", hardwareButtons_cameraPressed);
        }

        // If there is an orientation sensor present on the device, register for notifications
        if (oOrientationSensor != null) {
            oOrientationSensor.addEventListener("orientationchanged", orientationSensor_orientationChanged);
        
            // Update orientation of buttons with the current orientation
            updateButtonOrientation();
        }

        oDisplayInformation.addEventListener("orientationchanged", displayInformation_orientationChanged);
        oSystemMediaControls.addEventListener("propertychanged", systemMediaControls_propertyChanged);
    }

    /// <summary>
    /// Unregisters event handlers for hardware buttons and orientation sensors
    /// </summary>
    function unregisterEventHandlers()
    {
        if (Windows.Foundation.Metadata.ApiInformation.isTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
        {
            Windows.Phone.UI.Input.HardwareButtons.removeEventListener("camerapressed", hardwareButtons_cameraPressed);
        }

        if (oOrientationSensor != null) {
            oOrientationSensor.removeEventListener("orientationchanged", orientationSensor_orientationChanged);
        }

        oDisplayInformation.removeEventListener("orientationchanged", displayInformation_orientationChanged);
        oSystemMediaControls.removeEventListener("propertychanged", systemMediaControls_propertyChanged);
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
        cycleModeButton.style.transform = "rotate(" + angle + "deg)";
        photoButton.style.transform = "rotate(" + angle + "deg)";
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

    function sceneAnalysisEffect_sceneAnalyzed(args) {
       // Update the graphical representation of how much detail could be recovered through an HDR capture, ranging
       // from 0 ("everything is within the dynamic range of the camera") to CERTAINTY_CAP ("at this point the app
       // strongly recommends an HDR capture"), where CERTAINTY_CAP can be any number between 0 and 1 that the app chooses.
       hdrImpactBar.value = Math.min(certaintyCap, args.detail[0].resultFrame.highDynamicRange.certainty);
    }

    function photoButton_tapped() {
        takeAdvancedCaptureAsync();
    }

    function hardwareButtons_cameraPressed() {
        takeAdvancedCaptureAsync();
    }

    /// <summary>
    /// This event will be raised only on devices that support returning a reference photo, which is a normal exposure of the scene
    /// without processing, also referred to as "EV0".
    /// </summary>
    /// <param name="args">The event data.</param>
    function advancedCapture_optionalReferencePhotoCaptured(args) {
        // Retrieve the context (i.e. the information about the capture this event belongs to)
        var context = args.context;

        // Hold a reference to the frame and filename before the async file creation operation so they're not cleaned up before reencodeAndSavePhotoAsync
        var frame = args.frame;
        var fileName = context.fileName.replace(".jpg", "_Reference.jpg");

        return oCaptureFolder.createFileAsync(fileName, Windows.Storage.CreationCollisionOption.GenerateUniqueName)
        .then(function(file) {
            console.log("advancedCapture_optionalReferencePhotoCaptured for " + context.fileName + ". Saving to " + file.path);
            return reencodeAndSavePhotoAsync(frame, file, context.orientation);
        });
    }

    /// <summary>
    /// This event will be raised when all the necessary captures are completed, and at this point the camera is technically ready
    /// to capture again while the processing takes place.
    /// </summary>
    /// <param name="args">The event data.</param>
    function advancedCapture_allPhotosCaptured(args) {
        console.log("advancedCapture_allPhotosCaptured");
    }

    /// <summary>
    /// In the event of the app being minimized this method handles media property change events. If the app receives a mute
    /// notification, it is no longer in the foregroud.
    /// </summary>
    /// <param name="args"></param>
    function systemMediaControls_propertyChanged(args) {
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
            updateUi();
        });
    }

    function mediaCapture_failed(errorEventArgs)
    {
        console.log("MediaCapture_Failed: 0x" + errorEventArgs.code + ": " + errorEventArgs.message);

        cleanupCameraAsync()
        .done(function() {
            updateUi();
        });
    }

    app.start();
})();
