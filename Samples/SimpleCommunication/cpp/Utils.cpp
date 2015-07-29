//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Utils.h"

using namespace SDKTemplate;

using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Media::Capture;

using namespace Concurrency;

void SdkSample::Initialize()
{
    _rootPage = MainPage::Current;
    _dispatcher = _rootPage->Dispatcher;
}

void SdkSample::Deinitialize()
{
    _rootPage = nullptr;
    _dispatcher = nullptr;
}

void SdkSample::DisplayStatus(Platform::String^ msg)
{
    Log(msg);
}

void SdkSample::DisplayError(Platform::String^ msg)
{
    Log(msg, nullptr, "error");
}

void SdkSample::FormatError(HResult err, Platform::String^ msg, Platform::String^ prefix)
{
    if (err.Value)
    {
        WCHAR dst[33];
        _ultow_s(err.Value, dst, ARRAYSIZE(dst), 16);
        Log(
            prefix +
            msg +
            ", Error code: " +
            ref new Platform::String(dst),
            nullptr,
            "error"
            );
    }
    else
    {
        Log(
            prefix +
            msg,
            nullptr,
            "error"
            );
    }
}

void SdkSample::FormatError(CaptureFailedEventArgs^ e, Platform::String^ prefix)
{
    HResult hr;
    hr.Value = e->Code;
    FormatError(hr, e->Message, prefix);
}

void SdkSample::ClearLastStatus()
{
    Log("");
}

void SdkSample::Log(Platform::String^ message)
{
    Log(message, nullptr, "status");
}

void SdkSample::Log(Platform::String^ message, Platform::String^ tag, Platform::String^ type)
{
    auto isError = (type == "error");
    auto isStatus = (type == "status");

    if (isError || isStatus) 
    {
        if (_rootPage != nullptr && _dispatcher != nullptr)
        {
            _dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([this, message, isError]()
            {
                _rootPage->NotifyUser(message, (isError) ? NotifyType::ErrorMessage : NotifyType::StatusMessage);
            }));
        }
    }
}

CaptureDevice::CaptureDevice()
    : _mediaCapture(nullptr)
{

}

Platform::Agile<MediaCapture>
    CaptureDevice::MediaCapture::get()
{
    return _mediaCapture;
}

/// <summary>
/// Initialization of the device.
/// </summary>
task<void> CaptureDevice::InitializeAsync()
{
    try
    {
        auto mediaCapture = ref new Windows::Media::Capture::MediaCapture();
        _mediaCapture = mediaCapture;
        _mediaCaptureFailedEventRegistrationToken = mediaCapture->Failed += ref new MediaCaptureFailedEventHandler(this, &CaptureDevice::OnCaptureFailed);

        return create_task(mediaCapture->InitializeAsync());
    }
    catch (Platform::Exception^ e)
    {
        DoCleanup();
        throw e;
    }

    return create_task([](){});
}

/// <summary>
/// Cleaning up the sink.
/// </summary>
void CaptureDevice::CleanupSink()
{
    if (_mediaSink)
    {
        _mediaSink->IncomingConnectionEvent -= _mediaSinkIncomingConnectionEventRegistrationToken;
        delete _mediaSink;
        _mediaSink = nullptr;
        _recordingStarted = false;
    }
}

/// <summary>
/// Cleans up the resources.
/// </summary>
void CaptureDevice::DoCleanup()
{
    Windows::Media::Capture::MediaCapture ^mediaCapture = _mediaCapture.Get();
    if (mediaCapture != nullptr)
    {
        mediaCapture->Failed -= _mediaCaptureFailedEventRegistrationToken;
    }

    CleanupSink();
}

/// <summary>
/// Asynchronous method cleaning up resources and stopping recording if necessary.
/// </summary>
task<void> CaptureDevice::CleanupAsync()
{
    Windows::Media::Capture::MediaCapture ^mediaCapture = _mediaCapture.Get();
    if (mediaCapture == nullptr && !_mediaSink)
    {
        return create_task([](){});
    }

    if (mediaCapture != nullptr)
    {
        mediaCapture->Failed -= _mediaCaptureFailedEventRegistrationToken;
    }

    if (mediaCapture != nullptr && _recordingStarted)
    {
        return create_task(mediaCapture->StopRecordAsync()).then([this](task<void>&)
        {
            DoCleanup();
        });
    }
    else
    {
        DoCleanup();
    }
    return create_task([](){});
}

/// <summary>
/// Function allows to select camera settings.
/// </summary>
/// <param name="mediaStreamType" type="Windows.Media.Capture.MediaStreamType">
/// Type of a the media stream.
/// </param>
/// <param name="mediaStreamType">
/// Function which will be called to filter the correct settings.
/// </param>
task<void> CaptureDevice::SelectPreferredCameraStreamSettingAsync(MediaStreamType mediaStreamType, VideoSettingsFilter^ settingsFilterFunc)
{
    auto prefferedSettings = _mediaCapture.Get()->VideoDeviceController->GetAvailableMediaStreamProperties(mediaStreamType);

    std::vector<Windows::Media::MediaProperties::IVideoEncodingProperties^> vector;
    for (unsigned int i = 0; i < prefferedSettings->Size; i ++)
    {
        auto prop = prefferedSettings->GetAt(i);
        if (settingsFilterFunc(prop, i))
        {
            vector.push_back(static_cast<Windows::Media::MediaProperties::IVideoEncodingProperties^>(prop));
        }
    }

    std::sort(vector.begin(), vector.end(), [](Windows::Media::MediaProperties::IVideoEncodingProperties^ prop1, Windows::Media::MediaProperties::IVideoEncodingProperties^ prop2)
    {
        return (prop2->Width - prop1->Width);                            
    });

    if (vector.size() > 0)
    {
        return create_task(_mediaCapture.Get()->VideoDeviceController->SetMediaStreamPropertiesAsync(mediaStreamType, vector.front()));
    }

    return create_task([]()
    {
        // Nothing to do
    });
}

/// <summary>
/// Begins recording
/// </summary>
/// <param name="mediaEncodingProfile">
/// Encoding profile which should be used for the recording session.
/// </param>
task<void> CaptureDevice::StartRecordingAsync(Windows::Media::MediaProperties::MediaEncodingProfile^ mediaEncodingProfile)
{
    // We cannot start recording twice.
    if (_mediaSink && _recordingStarted)
    {
        throw ref new Platform::Exception(__HRESULT_FROM_WIN32(ERROR_INVALID_STATE));
    }

    // Release sink if there is one already.
    CleanupSink();

    // Create new sink
    _mediaSink = ref new Microsoft::Samples::SimpleCommunication::StspMediaSinkProxy();
    _mediaSinkIncomingConnectionEventRegistrationToken = _mediaSink->IncomingConnectionEvent += ref new Windows::Foundation::EventHandler<Object^>(this, &CaptureDevice::OnIncomingConnection);

    return create_task(_mediaSink->InitializeAsync(mediaEncodingProfile->Audio, mediaEncodingProfile->Video)).then([this, mediaEncodingProfile](Windows::Media::IMediaExtension^ mediaExtension)
    {
        return create_task(_mediaCapture->StartRecordToCustomSinkAsync(mediaEncodingProfile, mediaExtension)).then([this](task<void>& asyncInfo)
        {
            try
            {
                asyncInfo.get();
                _recordingStarted = true;
            }
            catch (Platform::Exception^)
            {
                CleanupSink();
                throw;
            }
        });
    });
}

/// <summary>
/// Stops recording.
/// </summary>
task<void> CaptureDevice::StopRecordingAsync()
{
    if (_recordingStarted)
    {
        return create_task(_mediaCapture.Get()->StopRecordAsync()).then([this]()
        {
            CleanupSink();
        });
    }
    else
    {
        // If recording not started just do nothing
    }
    return create_task([](){});
}
