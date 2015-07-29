//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "MainPage.xaml.h"

using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;

using namespace Concurrency;

namespace SDKTemplate
{
    ref class CaptureFailedEventArgs
    {
    internal:
        CaptureFailedEventArgs(HRESULT code, Platform::String^ message)
            : _code(code)
            , _message(message)
        {
        }

        property HRESULT Code
        {
            HRESULT get() { return _code; }
        }

        property Platform::String^ Message
        {
            Platform::String^ get() { return _message; }
        }

    private:
        HRESULT _code;
        Platform::String^ _message;
    };

    ref class SdkSample
    {
    internal:
        void Initialize();
        void Deinitialize();

        void DisplayStatus(Platform::String^ msg);
        void DisplayError(Platform::String^ msg);
        void FormatError(HResult err, Platform::String^ msg, Platform::String^ prefix);
        void FormatError(CaptureFailedEventArgs^ e, Platform::String^ prefix);
        void ClearLastStatus();

    private:
        void Log(Platform::String^ message);
        void Log(Platform::String^ message, Platform::String^ tag, Platform::String^ type);

        SDKTemplate::MainPage^ _rootPage;
        Windows::UI::Core::CoreDispatcher^ _dispatcher;
    };

    ref class CaptureDevice;

    delegate void CaptureFailedHandler(CaptureDevice^ sender, CaptureFailedEventArgs^ errorEventArgs);
    delegate void IncomingConnectionEventHandler(Microsoft::Samples::SimpleCommunication::StspMediaSinkProxy^ sender, Microsoft::Samples::SimpleCommunication::IncomingConnectionEventArgs^ args);
    delegate bool VideoSettingsFilter (Windows::Media::MediaProperties::IMediaEncodingProperties^, unsigned int);

    // Capture device represents a device used in one capture session between
    // calling CaptureManager.lockAsync and CaptureManager.unlockAsync.
    ref class CaptureDevice sealed
    {
    private:
        // Media capture object
        Platform::Agile<Windows::Media::Capture::MediaCapture> _mediaCapture;
        // Custom media sink
        Microsoft::Samples::SimpleCommunication::StspMediaSinkProxy^ _mediaSink;
        // EventRegistrationToken for StspMediaSink::IncomingConnection
        Windows::Foundation::EventRegistrationToken _mediaSinkIncomingConnectionEventRegistrationToken;
        // EventRegistrationToken for MediaCapture::Failed
        Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventRegistrationToken;

        // Flag indicating if recording to custom sink has started
        bool _recordingStarted;

    internal:
        event CaptureFailedHandler^ Failed;
        event IncomingConnectionEventHandler^ IncomingConnectionEvent;

        CaptureDevice();

        /// <summary>
        /// Initialization of the device.
        /// </summary>
        task<void> InitializeAsync();

        /// <summary>
        /// Cleaning up the sink.
        /// </summary>
        void CleanupSink();

        /// <summary>
        /// Cleans up the resources.
        /// </summary>
        void DoCleanup();

        /// <summary>
        /// Asynchronous method cleaning up resources and stopping recording if necessary.
        /// </summary>
        task<void> CleanupAsync();

        /// <summary>
        /// Function allows to select camera settings.
        /// </summary>
        /// <param name="mediaStreamType" type="Windows.Media.Capture.MediaStreamType">
        /// Type of a the media stream.
        /// </param>
        /// <param name="mediaStreamType">
        /// Function which will be called to filter the correct settings.
        /// </param>
        task<void> SelectPreferredCameraStreamSettingAsync(Windows::Media::Capture::MediaStreamType mediaStreamType, VideoSettingsFilter^ settingsFilterFunc);

        /// <summary>
        /// Begins recording
        /// </summary>
        /// <param name="mediaEncodingProfile">
        /// Encoding profile which should be used for the recording session.
        /// </param>
        task<void> StartRecordingAsync(Windows::Media::MediaProperties::MediaEncodingProfile^ mediaEncodingProfile);

        /// <summary>
        /// Stops recording.
        /// </summary>
        task<void> StopRecordingAsync();

        /// <summary>
        /// Capture has failed.
        /// </summary>
        void OnCaptureFailed(Windows::Media::Capture::MediaCapture^ sender, Windows::Media::Capture::MediaCaptureFailedEventArgs^ errorEventArgs)
        {
            // Forward the error to listeners.
            Failed(this, ref new CaptureFailedEventArgs(errorEventArgs->Code, errorEventArgs->Message));
        }

        /// <summary>
        /// There is an incoming connection.
        /// </summary>
        void OnIncomingConnection(Object^ sender, Object^ args)
        {
            // Forward the event to listeners.
            IncomingConnectionEvent(safe_cast<Microsoft::Samples::SimpleCommunication::StspMediaSinkProxy^>(sender), safe_cast<Microsoft::Samples::SimpleCommunication::IncomingConnectionEventArgs^>(args));
        }

        property Platform::Agile<Windows::Media::Capture::MediaCapture> MediaCapture
        {
            Platform::Agile<Windows::Media::Capture::MediaCapture> get();
        }
    };
}
