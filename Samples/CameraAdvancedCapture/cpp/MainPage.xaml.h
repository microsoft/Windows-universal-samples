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

#pragma once

#include "MainPage.g.h"

namespace CameraAdvancedCapture
{
    ref class ReencodeState sealed
    {
    internal:
        Windows::Graphics::Imaging::BitmapDecoder^ Decoder;
        Windows::Graphics::Imaging::BitmapEncoder^ Encoder;
        Windows::Storage::StorageFile^ File;
        Windows::Storage::FileProperties::PhotoOrientation Orientation;
    };

    /// <summary>
    /// Helper class to contain the information that describes an advanced capture
    /// </summary>
    ref class AdvancedCaptureContext
    {
    internal:
        Platform::String^ CaptureFileName;
        Windows::Storage::FileProperties::PhotoOrientation CaptureOrientation;
        Windows::Media::Capture::AdvancedCapturedPhoto^ Photo;
    };

    public ref class MainPage sealed
    {
    private:
        // Receive notifications about rotation of the device and UI and apply any necessary rotation to the preview stream and UI controls  
        Windows::Devices::Sensors::SimpleOrientationSensor^ _orientationSensor;
        Windows::Graphics::Display::DisplayInformation^ _displayInformation;
        Windows::Devices::Sensors::SimpleOrientation _deviceOrientation;
        Windows::Graphics::Display::DisplayOrientations _displayOrientation;

        // Rotation metadata to apply to the preview stream and recorded videos (MF_MT_VIDEO_ROTATION)
        // Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
        const GUID RotationKey;

        // Folder in which the captures will be stored (availability check performed in SetupUiAsync)
        Windows::Storage::StorageFolder^ _captureFolder;

        // Prevent the screen from sleeping while the camera is running
        Windows::System::Display::DisplayRequest^ _displayRequest;

        // For listening to media property changes
        Windows::Media::SystemMediaTransportControls^ _systemMediaControls;

        // MediaCapture and its state variables
        Platform::Agile<Windows::Media::Capture::MediaCapture^> _mediaCapture;
        bool _isInitialized;
        bool _isPreviewing;

        // Holds the index of the current advanced capture mode (-1 for no advanced capture active)
        int _advancedCaptureMode = -1;

        // Information about the camera device
        bool _externalCamera;
        bool _mirroringPreview;

        // The value at which the certainty from the HDR analyzer maxes out in the graphical representation
        const double CERTAINTY_CAP = 0.7;

        // Advanced Capture and Scene Analysis instances
        Windows::Media::Capture::AdvancedPhotoCapture^ _advancedCapture;
        Windows::Media::Core::SceneAnalysisEffect^ _sceneAnalysisEffect;

        // Event tokens
        Windows::Foundation::EventRegistrationToken _applicationSuspendingEventToken;
        Windows::Foundation::EventRegistrationToken _applicationResumingEventToken;
        Windows::Foundation::EventRegistrationToken _mediaControlPropChangedEventToken;
        Windows::Foundation::EventRegistrationToken _displayInformationEventToken;
        Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventToken;
        Windows::Foundation::EventRegistrationToken _orientationChangedEventToken;
        Windows::Foundation::EventRegistrationToken _hardwareCameraButtonEventToken;
        Windows::Foundation::EventRegistrationToken _sceneAnalyzedEventToken;
        Windows::Foundation::EventRegistrationToken _allPhotosCapturedEventToken;
        Windows::Foundation::EventRegistrationToken _optionalRefPhotoCapturedEventToken;

        // MediaCapture methods
        Concurrency::task<void> InitializeCameraAsync();
        Concurrency::task<void> CleanupCameraAsync();
        Concurrency::task<void> StartPreviewAsync();
        Concurrency::task<void> SetPreviewRotationAsync();
        Concurrency::task<void> StopPreviewAsync();
        Concurrency::task<void> CreateSceneAnalysisEffectAsync();
        Concurrency::task<void> CleanSceneAnalysisEffectAsync();
        Concurrency::task<void> EnableAdvancedCaptureAsync();
        void CycleAdvancedCaptureMode();
        Concurrency::task<void> DisableAdvancedCaptureAsync();
        Concurrency::task<void> TakeAdvancedCapturePhotoAsync();

        // Helpers
        Concurrency::task<Windows::Devices::Enumeration::DeviceInformation^> FindCameraDeviceByPanelAsync(Windows::Devices::Enumeration::Panel panel);
        Concurrency::task<void> ReencodeAndSavePhotoAsync(Windows::Storage::Streams::IRandomAccessStream^ stream, Windows::Storage::StorageFile^ file, Windows::Storage::FileProperties::PhotoOrientation photoOrientation);
        void UpdateUi();
        Concurrency::task<void> SetupUiAsync();
        Concurrency::task<void> CleanupUiAsync();
        void RegisterEventHandlers();
        void UnregisterEventHandlers();
        void WriteLine(Platform::String^ str);
        void WriteException(Platform::Exception^ ex);
        Concurrency::task<void> EmptyTask();
        static Platform::String^ GetTimeStr();
        static bool StringReplace(std::wstring& string, const std::wstring& removalStr, const std::wstring& replacementStr);

        // Rotation helpers
        Windows::Devices::Sensors::SimpleOrientation GetCameraOrientation();
        Windows::Storage::FileProperties::PhotoOrientation ConvertOrientationToPhotoOrientation(Windows::Devices::Sensors::SimpleOrientation orientation);
        int ConvertDeviceOrientationToDegrees(Windows::Devices::Sensors::SimpleOrientation orientation);
        int ConvertDisplayOrientationToDegrees(Windows::Graphics::Display::DisplayOrientations orientation);
        void UpdateControlOrientation();

        // UI event handlers
        void Application_Suspending(Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
        void Application_Resuming(Object^ sender, Object^ args);
        void DisplayInformation_OrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Object^ args);
        void SceneAnalysisEffect_SceneAnalyzed(Windows::Media::Core::SceneAnalysisEffect^ sender, Windows::Media::Core::SceneAnalyzedEventArgs^ args);
        void PhotoButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void CycleModeButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SystemMediaControls_PropertyChanged(Windows::Media::SystemMediaTransportControls^ sender, Windows::Media::SystemMediaTransportControlsPropertyChangedEventArgs^ args);
        void OrientationSensor_OrientationChanged(Windows::Devices::Sensors::SimpleOrientationSensor^, Windows::Devices::Sensors::SimpleOrientationSensorOrientationChangedEventArgs^);

        // Camera event handlers
        void AdvancedCapture_OptionalReferencePhotoCaptured(Windows::Media::Capture::AdvancedPhotoCapture^ sender, Windows::Media::Capture::OptionalReferencePhotoCapturedEventArgs^ args);
        void AdvancedCapture_AllPhotosCaptured(Windows::Media::Capture::AdvancedPhotoCapture^ sender, Object^ args);
        void HardwareButtons_CameraPress(Platform::Object^, Windows::Phone::UI::Input::CameraEventArgs^);
        void MediaCapture_Failed(Windows::Media::Capture::MediaCapture ^currentCaptureObject, Windows::Media::Capture::MediaCaptureFailedEventArgs^ errorEventArgs);

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

    public:
        MainPage();
        virtual ~MainPage();
    };
}
