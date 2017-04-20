#pragma once
using namespace Windows::Foundation;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Graphics::Display;
using namespace Windows::Media::Capture;
using namespace Windows::Storage::FileProperties;


namespace CameraStarterKit
{
    ref class CameraRotationHelper sealed
    {
    public:
        event EventHandler<bool>^ OrientationChanged;

        CameraRotationHelper(EnclosureLocation^ cameraEnclosureLocation);

        static bool IsEnclosureLocationExternal(EnclosureLocation^ enclosureLocation);
        SimpleOrientation GetUIOrientation();
        SimpleOrientation GetCameraCaptureOrientation();
        SimpleOrientation GetCameraPreviewOrientation();
        static PhotoOrientation ConvertSimpleOrientationToPhotoOrientation(SimpleOrientation orientation);
        static int ConvertSimpleOrientationToClockwiseDegrees(SimpleOrientation orientation);
    private:
        EnclosureLocation^ _cameraEnclosureLocation;
        DisplayInformation^ _displayInformation;
        SimpleOrientationSensor^ _orientationSensor;

        SimpleOrientation ConvertDisplayOrientationToSimpleOrientation(DisplayOrientations orientation);
        static SimpleOrientation MirrorOrientation(SimpleOrientation orientation);
        static SimpleOrientation AddOrientations(SimpleOrientation a, SimpleOrientation b);
        static SimpleOrientation SubtractOrientations(SimpleOrientation a, SimpleOrientation b);
        static VideoRotation ConvertSimpleOrientationToVideoRotation(SimpleOrientation orientation);
        static SimpleOrientation ConvertClockwiseDegreesToSimpleOrientation(int orientation);
        void SimpleOrientationSensor_OrientationChanged(SimpleOrientationSensor^ sender, SimpleOrientationSensorOrientationChangedEventArgs^ args);
        void DisplayInformation_OrientationChanged(DisplayInformation^ sender, Object^ ref);
        bool ShouldMirrorPreview();
        SimpleOrientation GetCameraOrientationRelativeToNativeOrientation();
    };
}