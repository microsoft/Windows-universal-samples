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

#include "pch.h"
#include "Scenario2_VideoChat.xaml.h"
#include <wrl\wrappers\corewrappers.h>

using namespace SDKTemplate;

using namespace Microsoft::WRL::Wrappers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_VideoChat::Scenario2_VideoChat()
    : _active(false)
    , _initialization(false)
    , _isTerminator(false)
    , _role(Role::uninitialized)
    , _dispatcher(nullptr)
    , _navigationDisabled(FALSE)
{
    InitializeComponent();

    _sdkSample.Initialize();
    _dispatcher = Window::Current->Dispatcher;

    MainPage::EnsureExtensionRegistration();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2_VideoChat::OnNavigatedTo(NavigationEventArgs^ e)
{
    LockScenarioChange(); // Lock

    Initialize().then([this](task<void> asyncInfo)
    {
        Windows::Foundation::HResult hr;
        try
        {
            asyncInfo.get();
            hr.Value = S_OK;
        }
        catch (Platform::Exception^ e)
        {
            hr.Value = e->HResult;
        }

        return OnInitializeCompleted(hr).then([this]()
        {
            UnlockScenarioChange(); // Unlock
        });
    });
}

void Scenario2_VideoChat::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
    if (InterlockedOr(&_navigationDisabled, 0))
    {
        e->Cancel = true;
    }
    else
    {
        RemoteVideo->MediaFailed -= _mediaElementFailedEventRegistrationToken;
        _mediaElementFailedEventRegistrationToken.Value = 0;
        Deactivate();
    }
}

void Scenario2_VideoChat::OnNavigatedFrom(NavigationEventArgs^ e)
{
    _sdkSample.Deinitialize();
}

void Scenario2_VideoChat::CallButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto address = IpAddressTextbox->Text;
    if (address)
    {
        _role = Role::active;
        RemoteVideo->Source = ref new Windows::Foundation::Uri("stsp://" + address);
        RemoteVideo->Play();
        IpAddressTextbox->IsEnabled = false;
        CallButton->IsEnabled = false;
        _sdkSample.DisplayStatus("Initiating connection... Please wait!");
    }
}

void Scenario2_VideoChat::EndCallButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    bool callEndCall = false;

    if (!_isTerminator && _active)
    {
        _isTerminator = true;
        callEndCall = true;
        LockScenarioChange(); // Lock
    }

    if (callEndCall)
    {
        EndCall().then([this](task<void>)
        {
            _isTerminator = false;
        });
    }
}

task<void> Scenario2_VideoChat::Initialize()
{
    if (_initialization || _device)
    {
        return create_task([]() {});
    }
    _initialization = true;
    _active = true;

    return task<DeviceInformationCollection^>(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture)).then([this](DeviceInformationCollection^ devices)
    {
        auto videoCaptures = devices->Size;
        return task<DeviceInformationCollection^>(DeviceInformation::FindAllAsync(DeviceClass::AudioCapture)).then([this, videoCaptures](DeviceInformationCollection^ devices)
        {
            auto audioCaptures = devices->Size;
            return ((videoCaptures > 0) && (audioCaptures) > 0);
        });
    }).then([this](task<bool> asyncInfo)
    {
        bool succeed;
        try
        {
            succeed = asyncInfo.get();
        }
        catch (Platform::Exception^ e)
        {
            Windows::Foundation::HResult hr;
            hr.Value = e->HResult;
            _sdkSample.FormatError(hr, "SDKTemplate.NoCamera", "A machine with a camera and a microphone is required to run this sample.");
            throw;
        }

        if (succeed)
        {
            CaptureDevice ^device = ref new CaptureDevice();

            return device->InitializeAsync().then([this, device]()
            {
                _device = device;
                _mediaCaptureFailedEventRegistrationToken = _device->Failed += ref new CaptureFailedHandler(this, &Scenario2_VideoChat::OnMediaCaptureFailed);
                _incomingConnectionEventRegistrationToken = _device->IncomingConnectionEvent += ref new IncomingConnectionEventHandler(this, &Scenario2_VideoChat::OnIncomingConnection);
                return StartRecordingToCustomSinkAsync();
            });
        }
        else
        {
            _sdkSample.DisplayError("A machine with a camera and a microphone is required to run this sample.");
            UnlockScenarioChange();
            cancel_current_task();
        }
    });
}

task<void> Scenario2_VideoChat::OnInitializeCompleted(Windows::Foundation::HResult hr)
{
    return create_task([this, hr]()
    {
        if (FAILED(hr.Value))
        {
            UnlockScenarioChange();
            _initialization = false;

            Deactivate();

            _sdkSample.FormatError(hr, "", ref new Platform::String(L"Initialization") + " error: ");
            throw ref new Platform::Exception(hr.Value);
        }

        return task<void>(_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            CallButton->IsEnabled = true;
            IpAddressTextbox->IsEnabled = true;
            EndCallButton->IsEnabled = false;

            CallButton->IsEnabled = true;
            EndCallButton->IsEnabled = true;
            if (_mediaElementFailedEventRegistrationToken.Value == 0)
            {
                _mediaElementFailedEventRegistrationToken = RemoteVideo->MediaFailed += ref new Windows::UI::Xaml::ExceptionRoutedEventHandler(this, &Scenario2_VideoChat::VideoPlaybackErrorHandler);
            }

            // Both side start out as passive
            _role = Role::passive;
            _isTerminator = false;
            _initialization = false;

            _sdkSample.DisplayStatus("Tap 'Call' button to start call");
        }
        )));
    });
}

task<void> Scenario2_VideoChat::EndCall()
{
    return CleanUp().then([this]()
    {

        if (_active)
        {
            return Initialize().then([this](task<void> asyncInfo)
            {
                Windows::Foundation::HResult hr;
                try
                {
                    asyncInfo.get();
                    hr.Value = S_OK;
                }
                catch (Platform::Exception^ e)
                {
                    hr.Value = e->HResult;
                }

                return OnInitializeCompleted(hr).then([this](task<void> asyncInfo) {});
            });
        }
        return create_task([]() {});
    });
}

task<void> Scenario2_VideoChat::Deactivate()
{
    _active = false;

    return CleanUp();
}

task<void> Scenario2_VideoChat::CleanUp()
{
    if (_device != nullptr)
    {
        _device->Failed -= _mediaCaptureFailedEventRegistrationToken;
        _device->IncomingConnectionEvent -= _incomingConnectionEventRegistrationToken;
    }
    _mediaCaptureFailedEventRegistrationToken.Value = 0;
    _incomingConnectionEventRegistrationToken.Value = 0;

    return task<void>(_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        CallButton->IsEnabled = false;
        EndCallButton->IsEnabled = false;
    }
        ))).then([this]()
    {
        if (_device != nullptr)
        {
            auto device = _device;
            _device = nullptr;

            return device->CleanupAsync();
        }

        return create_task([]() {});
    }).then([this](task<void>)
    {
        if (!_active)
        {
            return;
        }
        _dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            RemoteVideo->Source = nullptr;
        }));
    });
}

task<void> Scenario2_VideoChat::StartRecordingToCustomSinkAsync()
{
    auto mediaEncodingProfile = Windows::Media::MediaProperties::MediaEncodingProfile::CreateMp4(Windows::Media::MediaProperties::VideoEncodingQuality::Qvga);

    mediaEncodingProfile->Video->FrameRate->Numerator = 15;
    mediaEncodingProfile->Video->FrameRate->Denominator = 1;
    mediaEncodingProfile->Container = nullptr;

    return _device->StartRecordingAsync(mediaEncodingProfile);
}

void Scenario2_VideoChat::OnIncomingConnection(Microsoft::Samples::SimpleCommunication::StspMediaSinkProxy^ sender, Microsoft::Samples::SimpleCommunication::IncomingConnectionEventArgs^ args)
{
    args->Accept();

    _dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, args]()
    {
        EndCallButton->IsEnabled = true;
        _isTerminator = false;

        HString url_, header_;
        header_.Set(L"stsp://");
        WindowsTrimStringStart(reinterpret_cast<HSTRING>(args->RemoteUrl), header_.Get(), url_.GetAddressOf());
        auto url = reinterpret_cast<Platform::String^>(url_.Get());

        switch (_role)
        {
        case Role::active:
            _sdkSample.DisplayStatus("Connected. Remote machine address: " + url);
            break;
        case Role::passive:
        {
            auto address = args->RemoteUrl;
            RemoteVideo->Source = ref new Windows::Foundation::Uri(address);
            RemoteVideo->Play();
            _sdkSample.DisplayStatus("Connected. Remote machine address: " + url);
        }
        IpAddressTextbox->IsEnabled = false;
        CallButton->IsEnabled = false;
        break;
        default:
            break;
        }
    }
    ));
}

void Scenario2_VideoChat::OnMediaCaptureFailed(CaptureDevice^ sender, CaptureFailedEventArgs^ errorEventArgs)
{
    _sdkSample.FormatError(errorEventArgs, "Capture error code: ");

    _dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        if (!_isTerminator && _active)
        {
            _isTerminator = true;

            EndCall().then([this](task<void> previousTask)
            {
                _isTerminator = false;
            });
        }
    }));
}

void Scenario2_VideoChat::VideoPlaybackErrorHandler(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e)
{
    // See MF_MEDIA_ENGINE_ERR
    if (e->ErrorMessage != "MF_MEDIA_ENGINE_ERR_NOERROR")
    {
        _sdkSample.DisplayError(e->ErrorMessage);

        _dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            if (!_isTerminator && _active)
            {
                _isTerminator = true;

                EndCall().then([this](task<void> previousTask)
                {
                    _isTerminator = false;
                });
            }
        }));
    }
}

void Scenario2_VideoChat::LockScenarioChange()
{
    InterlockedCompareExchange(&_navigationDisabled, TRUE, FALSE);
}

void Scenario2_VideoChat::UnlockScenarioChange()
{
    InterlockedCompareExchange(&_navigationDisabled, FALSE, TRUE);
}
