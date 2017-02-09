#include "pch.h"
#include "CameraRotationHelper.h"

CameraStarterKit::CameraRotationHelper::CameraRotationHelper(EnclosureLocation^ cameraEnclosureLocation)
{
    _displayInformation = DisplayInformation::GetForCurrentView();
    _orientationSensor = SimpleOrientationSensor::GetDefault();
    _cameraEnclosureLocation = cameraEnclosureLocation;
    if (!IsEnclosureLocationExternal(_cameraEnclosureLocation) && _orientationSensor != nullptr)
    {
        _orientationSensor->OrientationChanged += ref new TypedEventHandler<SimpleOrientationSensor^, SimpleOrientationSensorOrientationChangedEventArgs^>(this, &CameraRotationHelper::SimpleOrientationSensor_OrientationChanged);
    }
    _displayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &CameraRotationHelper::DisplayInformation_OrientationChanged);
}

/// <summary>
/// Detects whether or not the camera is external to the device
/// </summary>
bool CameraStarterKit::CameraRotationHelper::IsEnclosureLocationExternal(EnclosureLocation^ enclosureLocation)
{
    return (enclosureLocation == nullptr || enclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Unknown);
}
/// <summary>
/// Gets the rotation to rotate UI elements
/// </summary>
SimpleOrientation CameraStarterKit::CameraRotationHelper::GetUIOrientation()
{
    if (IsEnclosureLocationExternal(_cameraEnclosureLocation))
    {
        // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
        return SimpleOrientation::NotRotated;
    }

    // Return the difference between the orientation of the device and the orientation of the app display
    SimpleOrientation deviceOrientation = _orientationSensor != nullptr ? _orientationSensor->GetCurrentOrientation() : SimpleOrientation::NotRotated;
    SimpleOrientation displayOrientation = ConvertDisplayOrientationToSimpleOrientation(_displayInformation->CurrentOrientation);
    return SubtractOrientations(displayOrientation, deviceOrientation);
}

/// <summary>
/// Gets the rotation of the camera to rotate pictures/videos when saving to file
/// </summary>
SimpleOrientation CameraStarterKit::CameraRotationHelper::GetCameraCaptureOrientation()
{
    if (IsEnclosureLocationExternal(_cameraEnclosureLocation))
    {
        // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
        return SimpleOrientation::NotRotated;
    }

    // Get the device orientation offset by the camera hardware offset
    SimpleOrientation deviceOrientation = _orientationSensor != nullptr ? _orientationSensor->GetCurrentOrientation() : SimpleOrientation::NotRotated;
    SimpleOrientation result = SubtractOrientations(deviceOrientation, GetCameraOrientationRelativeToNativeOrientation());

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
SimpleOrientation CameraStarterKit::CameraRotationHelper::GetCameraPreviewOrientation()
{
    if (IsEnclosureLocationExternal(_cameraEnclosureLocation))
    {
        // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
        return SimpleOrientation::NotRotated;
    }

    // Get the app display rotation offset by the camera hardware offset
    SimpleOrientation result = ConvertDisplayOrientationToSimpleOrientation(_displayInformation->CurrentOrientation);
    result = SubtractOrientations(result, GetCameraOrientationRelativeToNativeOrientation());

    // If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
    if (ShouldMirrorPreview())
    {
        result = MirrorOrientation(result);
    }
    return result;
}

PhotoOrientation CameraStarterKit::CameraRotationHelper::ConvertSimpleOrientationToPhotoOrientation(SimpleOrientation orientation)
{
    switch (orientation)
    {
    case SimpleOrientation::Rotated90DegreesCounterclockwise:
        return PhotoOrientation::Rotate90;
    case SimpleOrientation::Rotated180DegreesCounterclockwise:
        return PhotoOrientation::Rotate180;
    case SimpleOrientation::Rotated270DegreesCounterclockwise:
        return PhotoOrientation::Rotate270;
    case SimpleOrientation::NotRotated:
    default:
        return PhotoOrientation::Normal;
    }
}

int CameraStarterKit::CameraRotationHelper::ConvertSimpleOrientationToClockwiseDegrees(SimpleOrientation orientation)
{
    switch (orientation)
    {
    case SimpleOrientation::Rotated90DegreesCounterclockwise:
        return 270;
    case SimpleOrientation::Rotated180DegreesCounterclockwise:
        return 180;
    case SimpleOrientation::Rotated270DegreesCounterclockwise:
        return 90;
    case SimpleOrientation::NotRotated:
    default:
        return 0;
    }
}

SimpleOrientation CameraStarterKit::CameraRotationHelper::ConvertDisplayOrientationToSimpleOrientation(DisplayOrientations orientation)
{
    SimpleOrientation result;
    switch (orientation)
    {
    case DisplayOrientations::Landscape:
        result = SimpleOrientation::NotRotated;
        break;
    case DisplayOrientations::PortraitFlipped:
        result = SimpleOrientation::Rotated90DegreesCounterclockwise;
        break;
    case DisplayOrientations::LandscapeFlipped:
        result = SimpleOrientation::Rotated180DegreesCounterclockwise;
        break;
    case DisplayOrientations::Portrait:
    default:
        result = SimpleOrientation::Rotated270DegreesCounterclockwise;
        break;
    }

    // Above assumes landscape; offset is needed if native orientation is portrait
    if (_displayInformation->NativeOrientation == DisplayOrientations::Portrait)
    {
        result = AddOrientations(result, SimpleOrientation::Rotated90DegreesCounterclockwise);
    }

    return result;
}

SimpleOrientation CameraStarterKit::CameraRotationHelper::MirrorOrientation(SimpleOrientation orientation)
{
    // This only affects the 90 and 270 degree cases, because rotating 0 and 180 degrees is the same clockwise and counter-clockwise
    switch (orientation)
    {
    case SimpleOrientation::Rotated90DegreesCounterclockwise:
        return SimpleOrientation::Rotated270DegreesCounterclockwise;
    case SimpleOrientation::Rotated270DegreesCounterclockwise:
        return SimpleOrientation::Rotated90DegreesCounterclockwise;
    }
    return orientation;
}

SimpleOrientation CameraStarterKit::CameraRotationHelper::AddOrientations(SimpleOrientation a, SimpleOrientation b)
{
    int aRot = ConvertSimpleOrientationToClockwiseDegrees(a);
    int bRot = ConvertSimpleOrientationToClockwiseDegrees(b);
    int result = (aRot + bRot) % 360;
    return ConvertClockwiseDegreesToSimpleOrientation(result);
}

SimpleOrientation CameraStarterKit::CameraRotationHelper::SubtractOrientations(SimpleOrientation a, SimpleOrientation b)
{
    int aRot = ConvertSimpleOrientationToClockwiseDegrees(a);
    int bRot = ConvertSimpleOrientationToClockwiseDegrees(b);
    // Add 360 to ensure the modulus operator does not operate on a negative
    int result = (360 + (aRot - bRot)) % 360;
    return ConvertClockwiseDegreesToSimpleOrientation(result);
}

VideoRotation CameraStarterKit::CameraRotationHelper::ConvertSimpleOrientationToVideoRotation(SimpleOrientation orientation)
{
    switch (orientation)
    {
    case SimpleOrientation::Rotated90DegreesCounterclockwise:
        return VideoRotation::Clockwise270Degrees;
    case SimpleOrientation::Rotated180DegreesCounterclockwise:
        return VideoRotation::Clockwise180Degrees;
    case SimpleOrientation::Rotated270DegreesCounterclockwise:
        return VideoRotation::Clockwise90Degrees;
    case SimpleOrientation::NotRotated:
    default:
        return VideoRotation::None;
    }
}

SimpleOrientation CameraStarterKit::CameraRotationHelper::ConvertClockwiseDegreesToSimpleOrientation(int orientation)
{
    switch (orientation)
    {
    case 270:
        return SimpleOrientation::Rotated90DegreesCounterclockwise;
    case 180:
        return SimpleOrientation::Rotated180DegreesCounterclockwise;
    case 90:
        return SimpleOrientation::Rotated270DegreesCounterclockwise;
    case 0:
    default:
        return SimpleOrientation::NotRotated;
    }
}

void CameraStarterKit::CameraRotationHelper::SimpleOrientationSensor_OrientationChanged(SimpleOrientationSensor^ sender, SimpleOrientationSensorOrientationChangedEventArgs^ args)
{
    if (args->Orientation != SimpleOrientation::Faceup && args->Orientation != SimpleOrientation::Facedown)
    {
        // Only raise the OrientationChanged event if the device is not parallel to the ground. This allows users to take pictures of documents (FaceUp)
        // or the ceiling (FaceDown) in portrait or landscape, by first holding the device in the desired orientation, and then pointing the camera
        // either up or down, at the desired subject.
        //Note: This assumes that the camera is either facing the same way as the screen, or the opposite way. For devices with cameras mounted
        //      on other panels, this logic should be adjusted.
        OrientationChanged(this, false);
    }
}

void CameraStarterKit::CameraRotationHelper::DisplayInformation_OrientationChanged(DisplayInformation^ sender, Object^ ref)
{
    OrientationChanged(this, true);
}

bool CameraStarterKit::CameraRotationHelper::ShouldMirrorPreview()
{
    // It is recommended that applications mirror the preview for front-facing cameras, as it gives users a more natural experience, since it behaves more like a mirror
    return (_cameraEnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Front);
}

SimpleOrientation CameraStarterKit::CameraRotationHelper::GetCameraOrientationRelativeToNativeOrientation()
{
    // Get the rotation angle of the camera enclosure as it is mounted in the device hardware
    SimpleOrientation enclosureAngle = ConvertClockwiseDegreesToSimpleOrientation((int)_cameraEnclosureLocation->RotationAngleInDegreesClockwise);

    // Account for the fact that, on portrait-first devices, the built in camera sensor is read at a 90 degree offset to the native orientation
    if (_displayInformation->NativeOrientation == DisplayOrientations::Portrait && !IsEnclosureLocationExternal(_cameraEnclosureLocation))
    {
        enclosureAngle = AddOrientations(SimpleOrientation::Rotated90DegreesCounterclockwise, enclosureAngle);
    }

    return enclosureAngle;
}
