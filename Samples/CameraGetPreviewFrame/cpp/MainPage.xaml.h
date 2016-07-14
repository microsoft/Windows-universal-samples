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

namespace CameraGetPreviewFrame
{
    public ref class MainPage sealed
    {
    private:
        // Receive notifications about rotation of the device and UI and apply any necessary rotation to the preview stream and UI controls  
        Windows::Graphics::Display::DisplayInformation^ _displayInformation;
        Windows::Graphics::Display::DisplayOrientations _displayOrientation;

        // Prevent the screen from sleeping while the camera is running
        Windows::System::Display::DisplayRequest^ _displayRequest;

        // For listening to media property changes
        Windows::Media::SystemMediaTransportControls^ _systemMediaControls;

        // MediaCapture and its state variables
        Platform::Agile<Windows::Media::Capture::MediaCapture^> _mediaCapture;
        bool _isInitialized;
        bool _isPreviewing;

        // Information about the camera device
        bool _externalCamera;
        bool _mirroringPreview;
        
        // Rotation metadata to apply to the preview stream and recorded videos(MF_MT_VIDEO_ROTATION)
        // Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
        const GUID RotationKey;

        // Folder in which the captures will be stored (availability check performed in InitializeCameraAsync)
        Windows::Storage::StorageFolder^ _captureFolder;

        // Event tokens
        Windows::Foundation::EventRegistrationToken _applicationSuspendingEventToken;
        Windows::Foundation::EventRegistrationToken _applicationResumingEventToken;
        Windows::Foundation::EventRegistrationToken _mediaControlPropChangedEventToken;
        Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventToken;
        Windows::Foundation::EventRegistrationToken _displayInformationEventToken;

        // MediaCapture methods
        Concurrency::task<void> InitializeCameraAsync();
        Concurrency::task<void> CleanupCameraAsync();
        Concurrency::task<void> StartPreviewAsync();
        Concurrency::task<void> SetPreviewRotationAsync();
        Concurrency::task<void> StopPreviewAsync();
        Concurrency::task<void> GetPreviewFrameAsSoftwareBitmapAsync();
        Concurrency::task<void> GetPreviewFrameAsD3DSurfaceAsync();

        // Helpers
        Concurrency::task<void> SaveSoftwareBitmapAsync(Windows::Graphics::Imaging::SoftwareBitmap^ bitmap);
        Concurrency::task<Windows::Devices::Enumeration::DeviceInformation^> FindCameraDeviceByPanelAsync(Windows::Devices::Enumeration::Panel panel);
        void WriteLine(Platform::String^ str);
        void WriteException(Platform::Exception^ ex);
        int ConvertDisplayOrientationToDegrees(Windows::Graphics::Display::DisplayOrientations orientation);
        void ApplyGreenFilter(Windows::Graphics::Imaging::SoftwareBitmap^ bitmap);

        // Event handlers
        void Application_Suspending(Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
        void Application_Resuming(Object^ sender, Object^ args);
        void DisplayInformation_OrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Object^ args);
        void SystemMediaControls_PropertyChanged(Windows::Media::SystemMediaTransportControls^ sender, Windows::Media::SystemMediaTransportControlsPropertyChangedEventArgs^ args);
        void GetPreviewFrameButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void MediaCapture_Failed(Windows::Media::Capture::MediaCapture ^currentCaptureObject, Windows::Media::Capture::MediaCaptureFailedEventArgs^ errorEventArgs);

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

    public:
        MainPage();
        virtual ~MainPage();
    };
}
