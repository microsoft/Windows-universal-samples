//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Capture = Windows.Media.Capture;
    var DeviceEnumeration = Windows.Devices.Enumeration;
    var DisplayOrientations = Windows.Graphics.Display.DisplayOrientations;
    var Ocr = Windows.Media.Ocr;

    // MediaCapture and its state variables.
    var mediaCapture;
    var isInitialized = false;
    var externalCamera = false;
    var mirroringPreview = false;
    var isPreviewing = false;

    // OCR language.
    var ocrLanguage = new Windows.Globalization.Language("en");

    // Receive notifications about rotation of the UI and apply any necessary rotation to the preview stream.
    var displayInformation = Windows.Graphics.Display.DisplayInformation.getForCurrentView();

    // Prevent the screen from sleeping while the camera is running.
    var displayRequest = null;

    // Processed image ratio.
    var imageRatio;

    // Rotation metadata to apply to the preview stream and recorded videos (MF_MT_VIDEO_ROTATION)
    // Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
    var RotationKey = "C380465D-2271-428C-9B83-ECEA3B4A85C1";

    var page = WinJS.UI.Pages.define("/html/scenario2-camera-image.html", {
        ready: function (element, options) {
            displayInformation.addEventListener("orientationchanged", displayInformation_orientationChanged);
            element.querySelector("#buttonExtract").addEventListener("click", extractHandler);
            element.querySelector("#buttonCamera").addEventListener("click", cameraHandler);
            element.querySelector("#imagePreview").addEventListener("load", updateTextOverlay);
            window.addEventListener("resize", updatePreview);
            
            // Defined in default.html.
            var root = document.getElementById("root").winControl;
            if (root) {
                root.addEventListener("afterclose", updatePreview);
                root.addEventListener("afteropen", updatePreview);
            }

            // Reset media capture state.
            mediaCapture = null;
            isInitialized = false;
            externalCamera = false;
            mirroringPreview = false;
            isPreviewing = false;

            displayRequest = new Windows.System.Display.DisplayRequest();

            if (!Ocr.OcrEngine.isLanguageSupported(ocrLanguage)) {
                WinJS.log && WinJS.log(ocrLanguage.displayName + " language is not supported.", "sample", "error");
                return;
            }

            startCameraAsync().then(null, function (e) {
                WinJS.log && WinJS.log(e, "sample", "error");
            }).then(function () {
                // Update buttons availability.
                document.getElementById("buttonExtract").disabled = !isInitialized;
                document.getElementById("buttonCamera").disabled = isInitialized;

                // Update preview elements size and position.
                updatePreview();
            });
        },
        unload: function (element, options) {
            displayInformation.removeEventListener("orientationchanged", displayInformation_orientationChanged);
            window.removeEventListener("resize", updatePreview);

            // Defined in "default.html".
            var root = document.getElementById("root").winControl;
            if (root) {
                root.removeEventListener("afterclose", updatePreview);
                root.removeEventListener("afteropen", updatePreview);
            }

            cleanUpCamera();
        }
    });

    /// <summary>
    /// This event will fire when the page is rotated, when the DisplayInformation.AutoRotationPreferences value set in the setupUiAsync() method cannot be not honored.
    /// </summary>
    function displayInformation_orientationChanged(args) {
        if (isPreviewing) {
            setPreviewRotationAsync().then(function () {
                // Update preview elements size and position.
                updatePreview();
            });
        }
    }

    /// <summary>
    /// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes
    /// </summary>
    /// <returns></returns>
    function stopPreview() {
        isPreviewing = false;

        // Cleanup the UI.
        var previewControl = document.getElementById("previewControl");
        previewControl.pause();
        previewControl.src = null;

        // Allow the device screen to sleep now that the preview is stopped.
        displayRequest.requestRelease();
    }

    /// <summary>
    /// Cleans up the camera resources and unregisters from MediaCapture events.
    /// </summary>
    function cleanUpCamera() {
        if (isInitialized) {
            if (isPreviewing) {
                // The call to stop the preview is included here for completeness, but can be
                // safely removed if a call to MediaCapture.close() is being made later,
                // as the preview will be automatically stopped at that point.
                stopPreview();
            }

            isInitialized = false;
        }

        if (mediaCapture) {
            mediaCapture.removeEventListener("failed", mediaCapture_failed);
            mediaCapture.close();
            mediaCapture = null;
        }
    }

    /// <summary>
    /// Queries the available video capture devices to try and find one mounted on the desired panel.
    /// </summary>
    function findCameraDeviceByPanelAsync(desiredPanel) {
        // Get available devices for capturing pictures.
        return DeviceEnumeration.DeviceInformation.findAllAsync(DeviceEnumeration.DeviceClass.videoCapture).then(function (allVideoDevices) {
            if (!allVideoDevices || allVideoDevices.length == 0) {
                return null;
            }

            // Get the desired camera by panel.
            for (var i = 0; i < allVideoDevices.length; i++) {
                var device = allVideoDevices[i];

                if (device.enclosureLocation != null && device.enclosureLocation.panel == desiredPanel)
                {
                    return device;
                }
            }

            // If there is no device mounted on the desired panel, return the first device found.
            return allVideoDevices[0];
        });
    }

    /// <summary>
    /// Reads the current orientation of the app and calculates the VideoRotation necessary to ensure the preview is rendered in the correct orientation.
    /// </summary>
    function calculatePreviewRotation() {

        var rotationDegrees = 0;
        var sourceRotation;

        // Note that in some cases, the rotation direction needs to be inverted if the preview is being mirrored.
        switch (displayInformation.currentOrientation) {
            case DisplayOrientations.portrait:
                if (mirroringPreview) {
                    rotationDegrees = 270;
                    sourceRotation = Capture.VideoRotation.clockwise270Degrees;
                }
                else {
                    rotationDegrees = 90;
                    sourceRotation = Capture.VideoRotation.clockwise90Degrees;
                }
                break;

            case DisplayOrientations.landscapeFlipped:
                // No need to invert this rotation, as rotating 180 degrees is the same either way.
                rotationDegrees = 180;
                sourceRotation = Capture.VideoRotation.clockwise180Degrees;
                break;

            case DisplayOrientations.portraitFlipped:
                if (mirroringPreview) {
                    rotationDegrees = 90;
                    sourceRotation = Capture.VideoRotation.clockwise90Degrees;
                }
                else {
                    rotationDegrees = 270;
                    sourceRotation = Capture.VideoRotation.clockwise270Degrees;
                }
                break;

            case DisplayOrientations.landscape:
            default:
                rotationDegrees = 0;
                sourceRotation = Capture.VideoRotation.none;
                break;
        }

        return {
            rotationDegrees: rotationDegrees,
            sourceRotation: sourceRotation
        };
    }

    /// <summary>
    /// Gets the current orientation of the UI in relation to the device and applies a corrective rotation to the preview.
    /// </summary>
    function setPreviewRotationAsync() {

        // Only need to update the orientation if the camera is mounted on the device.
        if (externalCamera) {
            return WinJS.Promise.as();
        }

        // Calculate which way and how far to rotate the preview.
        var previewRotation = calculatePreviewRotation();

        // Set preview rotation in the preview source.
        mediaCapture.setPreviewRotation(previewRotation.sourceRotation);

        // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames
        var props = mediaCapture.videoDeviceController.getMediaStreamProperties(Capture.MediaStreamType.videoPreview);
        props.properties.insert(RotationKey, previewRotation.rotationDegrees);
        return mediaCapture.setEncodingPropertiesAsync(Capture.MediaStreamType.videoPreview, props, null);
    }

    /// <summary>
    /// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on and unlocks the UI.
    /// </summary>
    function startPreviewAsync() {
        // Prevent the device from sleeping while the preview is running.
        displayRequest.requestActive();

        // Start the preview.
        var preview = document.getElementById("previewControl");
        if (mirroringPreview) {
            preview.style.transform = "scale(-1, 1)";
        }
        preview.src = URL.createObjectURL(mediaCapture);
        preview.play();
        isPreviewing = true;

        preview.addEventListener("playing", function () {
            isPreviewing = true;
            // Initialize the preview to the current orientation.
            setPreviewRotationAsync();
        });
    }

    /// <summary>
    /// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, and starts preview.
    /// </summary>
    function initializeCameraAsync() {

        if (mediaCapture != null) {
            return WinJS.Promise.as();
        }

        // Attempt to get the back camera if one is available, but use any camera device if not.
        return findCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.back).then(function (cameraDevice) {
            if (cameraDevice == null) {
                throw "No camera device.";
            }

            // Figure out where the camera is located.
            if (cameraDevice.enclosureLocation == null || cameraDevice.enclosureLocation.panel == DeviceEnumeration.Panel.unknown) {
                // No information on the location of the camera, assume it's an external camera, not integrated on the device.
                externalCamera = true;
            }
            else {
                // Camera is fixed on the device.
                externalCamera = false;

                // Only mirror the preview if the camera is on the front panel.
                mirroringPreview = (cameraDevice.enclosureLocation.panel == DeviceEnumeration.Panel.front);
            }

            // Create MediaCapture and its settings.
            mediaCapture = new Capture.MediaCapture();

            var settings = new Capture.MediaCaptureInitializationSettings();
            settings.videoDeviceId = cameraDevice.id;

            // Register for a notification when something goes wrong.
            mediaCapture.addEventListener("failed", mediaCapture_failed);

            // Initialize MediaCapture.
            return mediaCapture.initializeAsync(settings);
        }).then(function () {
            isInitialized = true;
            // If initialization succeeded, start the preview.
            return startPreviewAsync();
        }, function (error) {
            WinJS.log && WinJS.log(error, "sample", "error");
        });
    }

    function mediaCapture_failed(e) {
        // Something went very wrong. The user might have unplugged the camera while using it.
        cleanUpCamera();
        WinJS.log && WinJS.log(e.message, "sample", "error");

        document.getElementById("textOverlay").innerHTML = "";
        document.getElementById("buttonCamera").disabled = false;
        document.getElementById("buttonExtract").disabled = true;
    }

    /// <summary>
    /// Starts the camera. Initializes resources and starts preview.
    /// </summary>
    function startCameraAsync() {
        var startCameraPromise = WinJS.Promise.as();

        if (!isInitialized)
        {
            startCameraPromise = startCameraPromise.then(function (){
                return initializeCameraAsync();
            });
        }

        startCameraPromise.then(function () {
            if (isInitialized) {
                document.getElementById("textOverlay").innerHTML = "";
                document.getElementById("textOverlay").hidden = true;
                document.getElementById("buttonCamera").disabled = true;
                document.getElementById("imagePreview").hidden = true;
                document.getElementById("imagePreview").style.display = "none";
                document.getElementById("previewControl").hidden = false;
                document.getElementById("buttonExtract").disabled = false;

                WinJS.log && WinJS.log("Camera started.", "sample", "status");
            }
        });

        return startCameraPromise;
    }

    function extractHandler() {
        // Disable another extract text request.
        document.getElementById("buttonExtract").disabled = true;

        //Get information about the preview.
        var previewProperties = mediaCapture.videoDeviceController.getMediaStreamProperties(Capture.MediaStreamType.videoPreview);
        var videoFrameWidth = previewProperties.width;
        var videoFrameHeight = previewProperties.height;

        // In portrait modes, the width and height must be swapped for the VideoFrame to have the correct aspect ratio and avoid letterboxing / black bars.
        if (!externalCamera &&
            (displayInformation.currentOrientation == DisplayOrientations.portrait
            || displayInformation.currentOrientation == DisplayOrientations.portraitFlipped)) {
                videoFrameWidth = previewProperties.height;
                videoFrameHeight = previewProperties.width;
        }

        // Create the video frame to request a SoftwareBitmap preview frame.
        var videoFrame = new Windows.Media.VideoFrame(Windows.Graphics.Imaging.BitmapPixelFormat.bgra8, videoFrameWidth, videoFrameHeight);

        // Create OCR engine.
        var ocrEngine = Windows.Media.Ocr.OcrEngine.tryCreateFromLanguage(new Windows.Globalization.Language("en"));

        var bitmap = null;
        var stream = Windows.Storage.Streams.InMemoryRandomAccessStream();
        mediaCapture.getPreviewFrameAsync(videoFrame).then(function (currentFrame) {
            bitmap = currentFrame.softwareBitmap;
            imageRatio = videoFrameWidth / videoFrameHeight;
            return Windows.Graphics.Imaging.BitmapEncoder.createAsync(Windows.Graphics.Imaging.BitmapEncoder.bmpEncoderId, stream);
        }).then(function (encoder) {
            encoder.setSoftwareBitmap(bitmap);
            return encoder.flushAsync();
        }).then(function () {
            var blob = MSApp.createBlobFromRandomAccessStream("image/bmp", stream.cloneStream());
            document.getElementById("imagePreview").src = URL.createObjectURL(blob, { oneTimeOnly: true });
            // Now, recognize text from image.
            return ocrEngine.recognizeAsync(bitmap);
        }).then(function (result) {
            var ocrText = document.getElementById("textOverlay");

            // Set font size to height of full control, so later it will be easier to control child controls font by percentage.
            ocrText.style.fontSize = ocrText.clientHeight + "px";

            if (result.lines != null) {
                for (var l = 0; l < result.lines.length; l++) {
                    var line = result.lines[l];
                    for (var w = 0; w < line.words.length; w++) {
                        var word = line.words[w];

                        // Define "div" tag to overlay recognized word.
                        var wordbox = document.createElement("div");

                        wordbox.className = "result";
                        wordbox.style.width = 100 * word.boundingRect.width / bitmap.pixelWidth + "%";
                        wordbox.style.height = 100 * word.boundingRect.height / bitmap.pixelHeight + "%";
                        wordbox.style.top = 100 * word.boundingRect.y / bitmap.pixelHeight + "%";
                        wordbox.style.left = 100 * word.boundingRect.x / bitmap.pixelWidth + "%";
                        wordbox.textContent = word.text;
                        wordbox.style.fontSize = Math.round(75 * word.boundingRect.height / bitmap.pixelHeight) + "%";

                        if (result.textAngle) {
                            // Rotate word for detected text angle.
                            wordbox.style.transform = "rotate(" + result.textAngle + "deg)";

                            // Calculate image/rotation center relative to word bounding box.
                            var centerX = -100 * (word.boundingRect.x - bitmap.pixelWidth / 2) / word.boundingRect.width;
                            var centerY = -100 * (word.boundingRect.y - bitmap.pixelHeight / 2) / word.boundingRect.height;
                            wordbox.style.transformOrigin = "" + centerX + "% " + centerY + "%";
                        }

                        // Put the filled textblock in the results grid.
                        ocrText.appendChild(wordbox);
                    }
                }
            }

            document.getElementById("buttonCamera").disabled = false;
            document.getElementById("imagePreview").hidden = false;
            document.getElementById("imagePreview").style.display = "block";
            document.getElementById("previewControl").hidden = true;
            document.getElementById("textOverlay").hidden = false;
            document.getElementById("textOverlay").style.display = "block";

            WinJS.log && WinJS.log("Image processed using " + ocrEngine.recognizerLanguage.displayName + " language.", "sample", "status");

            // Update preview elements size and position.
            updatePreview();
        }).then(null, function (e) {
            WinJS.log && WinJS.log(e.message, "sample", "error");
        });
    }

    /// <summary>
    /// This is event handler for 'Camera' button.
    /// Clears previous OCR results and starts camera.
    /// </summary>
    function cameraHandler() {
        startCameraAsync().then(function () {
            // Update preview elements size and position.
            updatePreview();
        }).then(null, function (e) {
            WinJS.log && WinJS.log(e.message, "sample", "error");
        })
    }

    /// <summary>
    /// Updates preview image size and position.
    /// </summary>
    function updatePreview() {
        // Get element to calculate size of camera preview and image.
        var parent = document.getElementById("scenarioView").parentElement;
        var header = document.getElementById("scenarioHeader");
        var footer = document.getElementById("scenarioFooter");

        // Calculate max width and height.
        var maxWidth = header.clientWidth;
        var maxHeight = parent.clientHeight - header.clientHeight - footer.clientHeight - 30;

        // Update preview element.
        var preview = document.getElementById("previewControl");
        preview.style.maxHeight = maxHeight + "px";

        // Update image element.
        var image = document.getElementById("imagePreview");
        if (!image.hidden) {
            if (maxWidth / maxHeight < imageRatio) {
                // Use max width.
                image.style.height = "auto";
                image.style.width = "100%";
            }
            else {
                // Use max height.
                image.style.height = maxHeight + "px";
                image.style.width = "auto";
            }
        }

        // If image loading is not done, skip updating text overlay.
        if (image.complete) {
            updateTextOverlay();
        }
    }

    /// <summary>
    /// Updates text overlay size and position.
    /// </summary>
    function updateTextOverlay () {
        var image = document.getElementById("imagePreview");
        var overlay = document.getElementById("textOverlay");

        // Set textOverlay element to match size and position of an imagePreview element.
        overlay.style.height = image.clientHeight + "px";
        overlay.style.width = image.clientWidth + "px";
        overlay.style.left = image.offsetLeft + "px";

        // Set fontSize to full element height, so it will be easier to set relative font size on word boxes.
        overlay.style.fontSize = image.clientHeight + "px";
    }
})();
