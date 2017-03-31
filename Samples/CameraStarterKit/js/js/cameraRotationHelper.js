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

    var Display = Windows.Graphics.Display;
    var DisplayOrientations = Windows.Graphics.Display.DisplayOrientations;
    var PhotoOrientation = Windows.Storage.FileProperties.PhotoOrientation;
    var Sensors = Windows.Devices.Sensors;
    var SimpleOrientation = Windows.Devices.Sensors.SimpleOrientation;

    var CameraRotationHelper = WinJS.Class.define(function (cameraEnclosureLocation) {
        this._cameraEnclosureLocation = cameraEnclosureLocation;
        this._displayInformation = Display.DisplayInformation.getForCurrentView();
        this._orientationSensor = Sensors.SimpleOrientationSensor.getDefault();

        if (!CameraRotationHelper.isEnclosureLocationExternal(this._cameraEnclosureLocation) && this._orientationSensor != null) {
            this._orientationSensor.addEventListener("orientationchanged", this._simpleOrientationSensor_orientationChanged.bind(this));
        }
        this._displayInformation.addEventListener("orientationchanged", this._displayInformation_orientationChanged.bind(this));
    },
    {
        _cameraEnclosureLocation: null,
        _displayInformation: null,
        _orientationSensor: null,
        /// <summary>
        /// Gets the rotation to rotate ui elements
        /// </summary>
        getUIOrientation: function () {
            if (CameraRotationHelper.isEnclosureLocationExternal(this._cameraEnclosureLocation)) {
                // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                return SimpleOrientation.notRotated;
            }

            // Return the difference between the orientation of the device and the orientation of the app display
            var deviceOrientation = this._orientationSensor != null ? this._orientationSensor.getCurrentOrientation() : SimpleOrientation.notRotated;
            var displayOrientation = this.convertDisplayOrientationToSimpleOrientation(this._displayInformation.currentOrientation);
            return CameraRotationHelper.subtractOrientations(displayOrientation, deviceOrientation);
        },
        /// <summary>
        /// Gets the rotation of the camera to rotate pictures/videos when saving to file
        /// </summary>
        getCameraCaptureOrientation: function () {
            if (CameraRotationHelper.isEnclosureLocationExternal(this._cameraEnclosureLocation)) {
                // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                return SimpleOrientation.notRotated;
            }

            // Get the device orientation offset by the camera hardware offset
            var deviceOrientation = this._orientationSensor != null ? this._orientationSensor.getCurrentOrientation() : SimpleOrientation.notRotated;
            var result = CameraRotationHelper.subtractOrientations(deviceOrientation, this.getCameraOrientationRelativeToNativeOrientation());

            // If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
            if (this.shouldMirrorPreview()) {
                result = CameraRotationHelper.mirrorOrientation(result);
            }
            return result;
        },
        /// <summary>
        /// Gets the rotation of the camera to display the camera preview
        /// </summary>
        getCameraPreviewOrientation: function () {
            if (CameraRotationHelper.isEnclosureLocationExternal(this._cameraEnclosureLocation)) {
                // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                return SimpleOrientation.NotRotated;
            }

            // Get the app display rotation offset by the camera hardware offset
            var result = this.convertDisplayOrientationToSimpleOrientation(this._displayInformation.currentOrientation);
            result = CameraRotationHelper.subtractOrientations(result, this.getCameraOrientationRelativeToNativeOrientation());
            // If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
            if (this.shouldMirrorPreview()) {
                result = this.mirrorOrientation(result);
            }
            return result;
        },
        convertDisplayOrientationToSimpleOrientation: function (orientation) {
            var result;
            switch (orientation) {
                case DisplayOrientations.landscape:
                    result = SimpleOrientation.notRotated;
                    break;
                case DisplayOrientations.portraitFlipped:
                    result = SimpleOrientation.rotated90DegreesCounterclockwise;
                    break;
                case DisplayOrientations.landscapeFlipped:
                    result = SimpleOrientation.rotated180DegreesCounterclockwise;
                    break;
                case DisplayOrientations.portrait:
                default:
                    result = SimpleOrientation.rotated270DegreesCounterclockwise;
                    break;
            }

            // Above assumes landscape; offset is needed if native orientation is portrait
            if (this._displayInformation.nativeOrientation === DisplayOrientations.portrait) {
                result = CameraRotationHelper.addOrientations(result, SimpleOrientation.rotated90DegreesCounterclockwise);
            }

            return result;
        },
        _simpleOrientationSensor_orientationChanged: function (args) {

            if (args.orientation != SimpleOrientation.faceup && args.orientation != SimpleOrientation.facedown) {
                // Only raise the OrientationChanged event if the device is not parallel to the ground. This allows users to take pictures of documents (FaceUp)
                // or the ceiling (FaceDown) in portrait or landscape, by first holding the device in the desired orientation, and then pointing the camera
                // either up or down, at the desired subject.
                //Note: This assumes that the camera is either facing the same way as the screen, or the opposite way. For devices with cameras mounted
                //      on other panels, this logic should be adjusted.
                this.dispatchEvent("orientationchanged", { updatePreview: false });
            }
        },
        _displayInformation_orientationChanged: function (args) {
            this.dispatchEvent("orientationchanged", { updatePreview: true });
        },
        shouldMirrorPreview: function () {
            // It is recommended that applications mirror the preview for front-facing cameras, as it gives users a more natural experience, since it behaves more like a mirror
            return (this._cameraEnclosureLocation.panel === Windows.Devices.Enumeration.Panel.front);
        },
        getCameraOrientationRelativeToNativeOrientation: function () {
            // Get the rotation angle of the camera enclosure as it is mounted in the device hardware
            var enclosureAngle = this.convertDisplayOrientationToSimpleOrientation(this._cameraEnclosureLocation.RotationAngleInDegreesClockwise);

            // Account for the fact that, on portrait-first devices, the built in camera sensor is read at a 90 degree offset to the native orientation
            if (this._displayInformation.nativeOrientation === DisplayOrientations.portrait && !CameraRotationHelper.isEnclosureLocationExternal(this._cameraEnclosureLocation)) {
                enclosureAngle = CameraRotationHelper.addOrientations(SimpleOrientation.rotated90DegreesCounterclockwise, enclosureAngle);
            }

            return enclosureAngle;
        }
    },
    {
        /// <summary>
        /// Detects whether or not the camera is external to the device
        /// </summary>
        isEnclosureLocationExternal: function (enclosureLocation) {
            return (enclosureLocation === null || enclosureLocation.Panel === Windows.Devices.Enumeration.Panel.unknown);
        },
        convertSimpleOrientationToPhotoOrientation: function (orientation) {
            switch (orientation) {
                case SimpleOrientation.rotated90DegreesCounterclockwise:
                    return PhotoOrientation.rotate90;
                case SimpleOrientation.rotated180DegreesCounterclockwise:
                    return PhotoOrientation.rotate180;
                case SimpleOrientation.rotated270DegreesCounterclockwise:
                    return PhotoOrientation.rotate270;
                case SimpleOrientation.notRotated:
                default:
                    return PhotoOrientation.normal;
            }
        },
        convertSimpleOrientationToClockwiseDegrees: function (orientation) {
            switch (orientation) {
                case SimpleOrientation.rotated90DegreesCounterclockwise:
                    return 270;
                case SimpleOrientation.rotated180DegreesCounterclockwise:
                    return 180;
                case SimpleOrientation.rotated270DegreesCounterclockwise:
                    return 90;
                case SimpleOrientation.rotRotated:
                default:
                    return 0;
            }
        },
        mirrorOrientation: function (orientation) {
            // This only affects the 90 and 270 degree cases, because rotating 0 and 180 degrees is the same clockwise and counter-clockwise
            switch (orientation) {
                case SimpleOrientation.rotated90DegreesCounterclockwise:
                    return SimpleOrientation.rotated270DegreesCounterclockwise;
                case SimpleOrientation.rotated270DegreesCounterclockwise:
                    return SimpleOrientation.rotated90DegreesCounterclockwise;
            }
            return orientation;
        },
        addOrientations: function (a, b) {
            var aRot = this.convertSimpleOrientationToClockwiseDegrees(a);
            var bRot = this.convertSimpleOrientationToClockwiseDegrees(b);
            var result = (aRot + bRot) % 360;
            return this.convertClockwiseDegreesToSimpleOrientation(result);
        },
        subtractOrientations: function (a, b) {
            var aRot = this.convertSimpleOrientationToClockwiseDegrees(a);
            var bRot = this.convertSimpleOrientationToClockwiseDegrees(b);
            // Add 360 to ensure the modulus operator does not operate on a negative
            var result = (360 + (aRot - bRot)) % 360;
            return this.convertClockwiseDegreesToSimpleOrientation(result);
        },
        convertSimpleOrientationToVideoRotation: function (orientation) {
            switch (orientation) {
                case SimpleOrientation.rotated90DegreesCounterclockwise:
                    return VideoRotation.Clockwise270Degrees;
                case SimpleOrientation.rotated180DegreesCounterclockwise:
                    return VideoRotation.Clockwise180Degrees;
                case SimpleOrientation.rotated270DegreesCounterclockwise:
                    return VideoRotation.Clockwise90Degrees;
                case SimpleOrientation.notRotated:
                default:
                    return VideoRotation.None;
            }
        },
        convertClockwiseDegreesToSimpleOrientation: function (orientation) {
            switch (orientation) {
                case 270:
                    return SimpleOrientation.rotated90DegreesCounterclockwise;
                case 180:
                    return SimpleOrientation.rotated180DegreesCounterclockwise;
                case 90:
                    return SimpleOrientation.rotated270DegreesCounterclockwise;
                case 0:
                default:
                    return SimpleOrientation.notRotated;
            }
        }
    });

    WinJS.Class.mix(CameraRotationHelper, WinJS.Utilities.eventMixin);

    WinJS.Namespace.define("SDKSample", {
        CameraRotationHelper: CameraRotationHelper
    });
})();