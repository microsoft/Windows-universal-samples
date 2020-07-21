#pragma once

#include "pch.h"
#include "MainPage.h"
#include "WordOverlay.h"
#include "OcrCapturedImage.g.h"

using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    struct OcrCapturedImage : OcrCapturedImageT<OcrCapturedImage>
    {
        OcrCapturedImage();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        fire_and_forget OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs const& e);
        fire_and_forget Application_Suspending(Windows::Foundation::IInspectable const& sender, Windows::ApplicationModel::SuspendingEventArgs const& e);
        fire_and_forget Application_Resuming(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
        void DisplayInformation_OrientationChanged(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);

        fire_and_forget CameraButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget ExtractButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

        void PreviewImage_SizeChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::SizeChangedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        // Language for OCR.
        Windows::Globalization::Language ocrLanguage{ Windows::Globalization::Language(L"en") };

        // Recognized words overlay boxes.
        std::vector<com_ptr<WordOverlay>> wordBoxes;

        // Receive notifications about rotation of the UI and apply any necessary rotation to the preview stream.
        Windows::Graphics::Display::DisplayInformation displayInformation{ Windows::Graphics::Display::DisplayInformation::GetForCurrentView() };

        // Rotation metadata to apply to the preview stream (MF_MT_VIDEO_ROTATION).
        // Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
        static constexpr GUID RotationKey{ 0xC380465D, 0x2271, 0x428C,{ 0x9B, 0x83, 0xEC, 0xEA, 0x3B, 0x4A, 0x85, 0xC1} };

        // Prevent the screen from sleeping while the camera is running.
        Windows::System::Display::DisplayRequest displayRequest;

        // MediaCapture and its state variables.
        Windows::Media::Capture::MediaCapture mediaCapture{ nullptr };
        bool isInitialized = false;
        bool isPreviewing = false;

        // Information about the camera device.
        bool mirroringPreview = false;
        bool externalCamera = false;

        event_token orientationChangedEventToken;
        event_token mediaCaptureFailedToken;
        event_token applicationSuspendingToken;
        event_token applicationResumingToken;

    private:
        Windows::Foundation::IAsyncAction StartCameraAsync();
        Windows::Foundation::IAsyncAction InitializeCameraAsync();
        Windows::Foundation::IAsyncAction StartPreviewAsync();
        Windows::Foundation::IAsyncAction SetPreviewRotationAsync();
        Windows::Foundation::IAsyncAction StopPreviewAsync();
        Windows::Foundation::IAsyncAction CleanupCameraAsync();

        void UpdateWordBoxTransform();
        Windows::Foundation::IAsyncOperation<DeviceInformation> FindCameraDeviceByPanelAsync(Windows::Devices::Enumeration::Panel desiredPanel);
        std::pair<Windows::Media::Capture::VideoRotation, int> CalculatePreviewRotation();
        fire_and_forget MediaCapture_Failed(Windows::Media::Capture::MediaCapture const& sender, Windows::Media::Capture::MediaCaptureFailedEventArgs const& errorEventArgs);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct OcrCapturedImage : OcrCapturedImageT<OcrCapturedImage, implementation::OcrCapturedImage>
    {
    };
}
