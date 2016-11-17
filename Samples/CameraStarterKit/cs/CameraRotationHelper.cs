using System;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Graphics.Display;
using Windows.Media.Capture;
using Windows.Storage.FileProperties;

namespace CameraStarterKit
{
    class CameraRotationHelper
    {
        private EnclosureLocation _cameraEnclosureLocation;
        private DisplayInformation _displayInformation = DisplayInformation.GetForCurrentView();
        private SimpleOrientationSensor _orientationSensor = SimpleOrientationSensor.GetDefault();

        /// <summary>
        /// Occurs each time the simple orientation sensor reports a new sensor reading or when the display's current or native orientation changes
        /// </summary>
        public event EventHandler<bool> OrientationChanged;

        public CameraRotationHelper(EnclosureLocation cameraEnclosureLocation)
        {
            _cameraEnclosureLocation = cameraEnclosureLocation;
            if (!IsEnclosureLocationExternal(_cameraEnclosureLocation) && _orientationSensor != null)
            {
                _orientationSensor.OrientationChanged += SimpleOrientationSensor_OrientationChanged;
            }
            _displayInformation.OrientationChanged += DisplayInformation_OrientationChanged;
        }

        /// <summary>
        /// Detects whether or not the camera is external to the device
        /// </summary>
        public static bool IsEnclosureLocationExternal(EnclosureLocation enclosureLocation)
        {
            return (enclosureLocation == null || enclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Unknown);
        }

        /// <summary>
        /// Gets the rotation to rotate ui elements
        /// </summary>
        public SimpleOrientation GetUIOrientation()
        {
            if (IsEnclosureLocationExternal(_cameraEnclosureLocation))
            {
                // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                return SimpleOrientation.NotRotated;
            }

            // Return the difference between the orientation of the device and the orientation of the app display
            var deviceOrientation = _orientationSensor?.GetCurrentOrientation() ?? SimpleOrientation.NotRotated;
            var displayOrientation = ConvertDisplayOrientationToSimpleOrientation(_displayInformation.CurrentOrientation);
            return SubtractOrientations(displayOrientation, deviceOrientation);
        }

        /// <summary>
        /// Gets the rotation of the camera to rotate pictures/videos when saving to file
        /// </summary>
        public SimpleOrientation GetCameraCaptureOrientation()
        {
            if (IsEnclosureLocationExternal(_cameraEnclosureLocation))
            {
                // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                return SimpleOrientation.NotRotated;
            }

            // Get the device orientation offset by the camera hardware offset
            var deviceOrientation = _orientationSensor?.GetCurrentOrientation() ?? SimpleOrientation.NotRotated;
            var result = SubtractOrientations(deviceOrientation, GetCameraOrientationRelativeToNativeOrientation());

            // If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
            if (ShouldMirrorPreview())
            {
                result = MirrorOrientation(result);
            }
            return result;
        }

        /// <summary>
        /// Gets the rotation of the camera to display the camera preview
        /// </summary>
        public SimpleOrientation GetCameraPreviewOrientation()
        {
            if (IsEnclosureLocationExternal(_cameraEnclosureLocation))
            {
                // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                return SimpleOrientation.NotRotated;
            }

            // Get the app display rotation offset by the camera hardware offset
            var result = ConvertDisplayOrientationToSimpleOrientation(_displayInformation.CurrentOrientation);
            result = SubtractOrientations(result, GetCameraOrientationRelativeToNativeOrientation());

            // If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
            if (ShouldMirrorPreview())
            {
                result = MirrorOrientation(result);
            }
            return result;
        }

        public static PhotoOrientation ConvertSimpleOrientationToPhotoOrientation(SimpleOrientation orientation)
        {
            switch (orientation)
            {
                case SimpleOrientation.Rotated90DegreesCounterclockwise:
                    return PhotoOrientation.Rotate90;
                case SimpleOrientation.Rotated180DegreesCounterclockwise:
                    return PhotoOrientation.Rotate180;
                case SimpleOrientation.Rotated270DegreesCounterclockwise:
                    return PhotoOrientation.Rotate270;
                case SimpleOrientation.NotRotated:
                default:
                    return PhotoOrientation.Normal;
            }
        }

        public static int ConvertSimpleOrientationToClockwiseDegrees(SimpleOrientation orientation)
        {
            switch (orientation)
            {
                case SimpleOrientation.Rotated90DegreesCounterclockwise:
                    return 270;
                case SimpleOrientation.Rotated180DegreesCounterclockwise:
                    return 180;
                case SimpleOrientation.Rotated270DegreesCounterclockwise:
                    return 90;
                case SimpleOrientation.NotRotated:
                default:
                    return 0;
            }
        }

        private SimpleOrientation ConvertDisplayOrientationToSimpleOrientation(DisplayOrientations orientation)
        {
            SimpleOrientation result;
            switch (orientation)
            {
                case DisplayOrientations.Landscape:
                    result = SimpleOrientation.NotRotated;
                    break;
                case DisplayOrientations.PortraitFlipped:
                    result = SimpleOrientation.Rotated90DegreesCounterclockwise;
                    break;
                case DisplayOrientations.LandscapeFlipped:
                    result = SimpleOrientation.Rotated180DegreesCounterclockwise;
                    break;
                case DisplayOrientations.Portrait:
                default:
                    result = SimpleOrientation.Rotated270DegreesCounterclockwise;
                    break;
            }

            // Above assumes landscape; offset is needed if native orientation is portrait
            if (_displayInformation.NativeOrientation == DisplayOrientations.Portrait)
            {
                result = AddOrientations(result, SimpleOrientation.Rotated90DegreesCounterclockwise);
            }

            return result;
        }

        private static SimpleOrientation MirrorOrientation(SimpleOrientation orientation)
        {
            // This only affects the 90 and 270 degree cases, because rotating 0 and 180 degrees is the same clockwise and counter-clockwise
            switch (orientation)
            {
                case SimpleOrientation.Rotated90DegreesCounterclockwise:
                    return SimpleOrientation.Rotated270DegreesCounterclockwise;
                case SimpleOrientation.Rotated270DegreesCounterclockwise:
                    return SimpleOrientation.Rotated90DegreesCounterclockwise;
            }
            return orientation;
        }

        private static SimpleOrientation AddOrientations(SimpleOrientation a, SimpleOrientation b)
        {
            var aRot = ConvertSimpleOrientationToClockwiseDegrees(a);
            var bRot = ConvertSimpleOrientationToClockwiseDegrees(b);
            var result = (aRot + bRot) % 360;
            return ConvertClockwiseDegreesToSimpleOrientation(result);
        }

        private static SimpleOrientation SubtractOrientations(SimpleOrientation a, SimpleOrientation b)
        {
            var aRot = ConvertSimpleOrientationToClockwiseDegrees(a);
            var bRot = ConvertSimpleOrientationToClockwiseDegrees(b);
            // Add 360 to ensure the modulus operator does not operate on a negative
            var result = (360 + (aRot - bRot)) % 360;
            return ConvertClockwiseDegreesToSimpleOrientation(result);
        }

        private static VideoRotation ConvertSimpleOrientationToVideoRotation(SimpleOrientation orientation)
        {
            switch (orientation)
            {
                case SimpleOrientation.Rotated90DegreesCounterclockwise:
                    return VideoRotation.Clockwise270Degrees;
                case SimpleOrientation.Rotated180DegreesCounterclockwise:
                    return VideoRotation.Clockwise180Degrees;
                case SimpleOrientation.Rotated270DegreesCounterclockwise:
                    return VideoRotation.Clockwise90Degrees;
                case SimpleOrientation.NotRotated:
                default:
                    return VideoRotation.None;
            }
        }

        private static SimpleOrientation ConvertClockwiseDegreesToSimpleOrientation(int orientation)
        {
            switch (orientation)
            {
                case 270:
                    return SimpleOrientation.Rotated90DegreesCounterclockwise;
                case 180:
                    return SimpleOrientation.Rotated180DegreesCounterclockwise;
                case 90:
                    return SimpleOrientation.Rotated270DegreesCounterclockwise;
                case 0:
                default:
                    return SimpleOrientation.NotRotated;
            }
        }

        private void SimpleOrientationSensor_OrientationChanged(SimpleOrientationSensor sender, SimpleOrientationSensorOrientationChangedEventArgs args)
        {
            if (args.Orientation != SimpleOrientation.Faceup && args.Orientation != SimpleOrientation.Facedown)
            {
                // Only raise the OrientationChanged event if the device is not parallel to the ground. This allows users to take pictures of documents (FaceUp)
                // or the ceiling (FaceDown) in portrait or landscape, by first holding the device in the desired orientation, and then pointing the camera
                // either up or down, at the desired subject.
                //Note: This assumes that the camera is either facing the same way as the screen, or the opposite way. For devices with cameras mounted
                //      on other panels, this logic should be adjusted.
                OrientationChanged?.Invoke(this, false);
            }
        }

        private void DisplayInformation_OrientationChanged(DisplayInformation sender, object args)
        {
            OrientationChanged?.Invoke(this, true);
        }

        private bool ShouldMirrorPreview()
        {
            // We recommend that applications mirror the preview for front-facing cameras, as it gives users a more natural experience, since it behaves more like a mirror
            return (_cameraEnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Front);
        }

        private SimpleOrientation GetCameraOrientationRelativeToNativeOrientation()
        {
            // Get the rotation angle of the camera enclosure as it is mounted in the device hardware
            var enclosureAngle = ConvertClockwiseDegreesToSimpleOrientation((int)_cameraEnclosureLocation.RotationAngleInDegreesClockwise);

            // Account for the fact that, on portrait-first devices, the built in camera sensor is read at a 90 degree offset to the native orientation
            if (_displayInformation.NativeOrientation == DisplayOrientations.Portrait && !IsEnclosureLocationExternal(_cameraEnclosureLocation))
            {
                enclosureAngle = AddOrientations(SimpleOrientation.Rotated90DegreesCounterclockwise, enclosureAngle);
            }

            return enclosureAngle;
        }
    }
}
