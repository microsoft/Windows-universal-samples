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
#include "Scenario1_LowLatency.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

enum Action
{
    previewButtonClicked,
    localClientButtonClicked,
    previewSetupCompleted,
    recordingStarted,
    latencyModeToggled,
};

Scenario1_LowLatency::Scenario1_LowLatency() 
    : _currentState(ScenarioOneStates::uninitialized)
    , _previousState(ScenarioOneStates::uninitialized)
    , _latencyMode(LatencyModes::uninitialized)
    , _initialized(false)
    , _navigationDisabled(FALSE) 
{
    InitializeComponent();

    _sdkSample.Initialize();
    _dispatcher = Window::Current->Dispatcher;

    _localHostVideo[LatencyModes::highLatency] = PlaybackVideo;
    _localHostVideo[LatencyModes::lowLatency] = RealTimePlaybackVideo;

    _streamFilteringCriteria.aspectRatio = "1.333333333333333";
    _streamFilteringCriteria.horizontalResolution = 640;
    _streamFilteringCriteria.subType = "YUY2";

    actionTransitionFunctions[ScenarioOneStates::initializing][Action::previewSetupCompleted] = ref new ActionTransition(this, &Scenario1_LowLatency::initializing_previewSetupCompleted);
    actionTransitionFunctions[ScenarioOneStates::waiting][Action::previewButtonClicked] = ref new ActionTransition(this, &Scenario1_LowLatency::waiting_previewButtonClicked);
    actionTransitionFunctions[ScenarioOneStates::previewing][Action::previewButtonClicked] = ref new ActionTransition(this, &Scenario1_LowLatency::previewing_previewButtonClicked);
    actionTransitionFunctions[ScenarioOneStates::previewing][Action::localClientButtonClicked] = ref new ActionTransition(this, &Scenario1_LowLatency::previewing_localClientButtonClicked);
    actionTransitionFunctions[ScenarioOneStates::previewing][Action::recordingStarted] = ref new ActionTransition(this, &Scenario1_LowLatency::previewing_recordingStarted);
    actionTransitionFunctions[ScenarioOneStates::streaming][Action::previewButtonClicked] = ref new ActionTransition(this, &Scenario1_LowLatency::streaming_previewButtonClicked);
    actionTransitionFunctions[ScenarioOneStates::streaming][Action::latencyModeToggled] = ref new ActionTransition(this, &Scenario1_LowLatency::streaming_latencyModeToggled);
    actionTransitionFunctions[ScenarioOneStates::streaming][Action::recordingStarted] = ref new ActionTransition(this, &Scenario1_LowLatency::streaming_recordingStarted);

    MainPage::EnsureExtensionRegistration();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1_LowLatency::OnNavigatedTo(NavigationEventArgs^ e)
{
    if (_initialized)
    {
        InitializeUIControls();
        return;
    }
    else
        if (_currentState == ScenarioOneStates::initializing || _device)
        {
            // We are already in intializing state.
            return;
        }

    LockScenarioChange();

    _sdkSample.DisplayStatus("Loading...Please wait.");

    // Check for camera and init scenario if one is found
    task<DeviceInformationCollection^>(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture)).then([this](DeviceInformationCollection^ devices)
    {
        return devices->Size > 0;
    }).then([this](bool success)
    {
        if (success)
        {
            if (_currentState == ScenarioOneStates::initializing || _device)
            {
                // We are already in intializing state.
                cancel_current_task();
            }
            else
            {
                return Initialize().then([this](task<void> result)
                {
                    try
                    {
                        result.get();
                        return create_task(_dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
                        {
                            InitializeUIControls();
                            UnlockScenarioChange();
                            _initialized = true;

                            m_visibilityEventRegistrationToken = Window::Current->VisibilityChanged += ref new Windows::UI::Xaml::WindowVisibilityChangedEventHandler(this, &Scenario1_LowLatency::VisibilityChanged);
                        })));
                    }
                    catch (Platform::Exception^ e)
                    {
                        Windows::Foundation::HResult hr;
                        hr.Value = e->HResult;
                        _sdkSample.FormatError(hr, "Initialization error: ", "");
                        UnlockScenarioChange();
                        return create_task([] {});
                    }
                });
            }
        }
        else
        {
            _sdkSample.DisplayError("A machine with a camera is required to run this sample.");
            UnlockScenarioChange();
            cancel_current_task();
        }
    });
}

void Scenario1_LowLatency::InitializeUIControls()
{
    PreviewVideo->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;

    PlaybackVideo->AutoPlay = true;
    PlaybackVideo->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;
    PlaybackVideo->MediaFailed += ref new Windows::UI::Xaml::ExceptionRoutedEventHandler(this, &Scenario1_LowLatency::OnMediaFailed);

    RealTimePlaybackVideo->AutoPlay = true;
    RealTimePlaybackVideo->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;
    RealTimePlaybackVideo->MediaFailed += ref new Windows::UI::Xaml::ExceptionRoutedEventHandler(this, &Scenario1_LowLatency::OnMediaFailed);

    _latencyMode = LatencyModes::highLatency;
    LatencyModeToggle->IsOn = (_latencyMode == LatencyModes::lowLatency);
};

void Scenario1_LowLatency::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
    if (InterlockedOr(&_navigationDisabled, 0))
    {
        e->Cancel = true;
    }

    Window::Current->VisibilityChanged -= m_visibilityEventRegistrationToken;
}

void Scenario1_LowLatency::OnNavigatedFrom(NavigationEventArgs^ e)
{
    _sdkSample.Deinitialize();
    this->Cleanup();
    for (auto iter = _localHostVideo.begin(); iter != _localHostVideo.end(); iter++)
    {
        auto video = iter->second;
        if (video)
        {
            video->Source = nullptr;
        }
    }
    Reset();
}

/// <summary>Handler for the camera 'failed' event</summary>
void Scenario1_LowLatency::OnMediaCaptureFailed(CaptureDevice^ sender, CaptureFailedEventArgs^ errorEventArgs)
{
    _sdkSample.FormatError(errorEventArgs, "Capture engine error code: ");
}

void Scenario1_LowLatency::OnIncomingConnection(Microsoft::Samples::SimpleCommunication::StspMediaSinkProxy^ sender, Microsoft::Samples::SimpleCommunication::IncomingConnectionEventArgs^ args)
{
    args->Accept();
}

void Scenario1_LowLatency::VisibilityChanged(Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e)
{
    if (e->Visible == true)
    {
        Initialize();
    }
    else
    {
        Cleanup();
    }
}


/// <summary>Initializes the scenario</summary>
task<void> Scenario1_LowLatency::Initialize()
{
    _ASSERT(_currentState != ScenarioOneStates::initializing);

    _currentState = ScenarioOneStates::initializing;
    CaptureDevice ^device = ref new CaptureDevice();


    return device->InitializeAsync().then([this, device](task<void> asyncInfo)
    {
        try
        {
            asyncInfo.get();
        }
        catch (Platform::Exception^ e)
        {
            HandleError(e, "Initialization error: ", "CaptureManager::InitializeAsync");
            if (_device)
            {
                _device->CleanupAsync().then([this]() {});
                _device = nullptr;
            }
            return create_task([] {});
        }

        _device = device;
        if (!_videoSettingsFilter)
        {
            _videoSettingsFilter = ref new VideoSettingsFilter([this](Windows::Media::MediaProperties::IMediaEncodingProperties^ encodingProp, unsigned int index)
            {
                if (encodingProp->Type != L"Video")
                {
                    return false;
                }

                auto videoProp = static_cast<Windows::Media::MediaProperties::IVideoEncodingProperties^>(encodingProp);
                if (videoProp->Height != 0 && videoProp->Width != 0)
                {
                    return (videoProp->Width == _streamFilteringCriteria.horizontalResolution &&
                        videoProp->Subtype == _streamFilteringCriteria.subType);
                }
                return false;
            });
        }

        return device->SelectPreferredCameraStreamSettingAsync(Windows::Media::Capture::MediaStreamType::VideoPreview, _videoSettingsFilter).then([this](task<void> asyncInfo)
        {
            try
            {
                asyncInfo.get();
            }
            catch (Platform::Exception^ e)
            {
                HandleError(e, "Initialization error: ", "CaptureDevice.SelectPreferredCameraStreamSettingAsync(): ");
                if (_device)
                {
                    _device->CleanupAsync().then([this]() {});
                    _device = nullptr;
                }
                return create_task([] {});
            }

            return create_task(actionTransitionFunctions[ScenarioOneStates::initializing][Action::previewSetupCompleted]());
        });
    });
}

task<void> Scenario1_LowLatency::StartRecordingToCustomSink()
{
    //// Use the MP4 preset to an obtain H.264 video encoding profile
    auto mediaEncodingProfile = Windows::Media::MediaProperties::MediaEncodingProfile::CreateMp4(Windows::Media::MediaProperties::VideoEncodingQuality::Vga);
    mediaEncodingProfile->Audio = nullptr;
    mediaEncodingProfile->Container = nullptr;

    return _device->StartRecordingAsync(mediaEncodingProfile).then([this](task<void> asyncInfo)
    {
        try
        {
            asyncInfo.get();
        }
        catch (Platform::Exception^ e)
        {
            HandleError(e, ": Local streaming initialization error: ", _currentState.ToString());
            return create_task([] {});
        }

        return create_task(actionTransitionFunctions[_currentState][Action::recordingStarted]());
    });
}

/// <summary>Cleans up.</summary>
task<void> Scenario1_LowLatency::Cleanup()
{
    _previousState = _currentState;

    if (!_device)
    {
        return create_task([]()
        {
        });
    }

    Reset();
    PreviewVideo->Source = nullptr;

    _device->Failed -= _mediaCaptureFailedEventRegistrationToken;
    _device->IncomingConnectionEvent -= _incomingConnectionEventRegistrationToken;
    return _device->CleanupAsync().then([this]()
    {
        _device = nullptr;
        _currentState = ScenarioOneStates::end;
    });
}

void Scenario1_LowLatency::PreviewButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LockScenarioChange(); // Lock

    _sdkSample.DisplayStatus("");
    create_task(actionTransitionFunctions[_currentState][Action::previewButtonClicked]()).then([this](task<void>& transitionTask)
    {
        try
        {
            transitionTask.get();
            UnlockScenarioChange(); // Unlock
        }
        catch (Exception^ e)
        {
            HandleError(e, "Transition error", "Scenario1_LowLatency::PreviewButtonClicked");
        }
    });
}

void Scenario1_LowLatency::LocalClientButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LockScenarioChange(); // Lock

    _sdkSample.DisplayStatus("");
    PreviewButton->IsEnabled = false;

    create_task(actionTransitionFunctions[ScenarioOneStates::previewing][Action::localClientButtonClicked]()).then([this]()
    {
        _dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            UpdateLocalClientWindow();
            UnlockScenarioChange(); // Unlock
        }
        ));
    }).then([this](task<void>& transitionTask)
    {
        try
        {
            transitionTask.get();
        }
        catch (Exception^ e)
        {
            HandleError(e, "Transition error", "Scenario1_LowLatency::LocalClientButtonClicked");
        }
    });
}

void Scenario1_LowLatency::LatencyModeToggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (!LatencyModeToggle->IsEnabled)
    {
        // This is not a user operation.
        return;
    }

    LockScenarioChange(); // Lock

    _sdkSample.DisplayStatus("");
    PreviewButton->IsEnabled = false;
    LatencyModeToggle->IsEnabled = false;

    create_task(actionTransitionFunctions[ScenarioOneStates::streaming][Action::latencyModeToggled]()).then([this]()
    {
        _dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            UpdateLocalClientWindow();
            UnlockScenarioChange(); // Unlock
        }));
    }).then([this](task<void>& transitionTask)
    {
        try
        {
            transitionTask.get();
        }
        catch (Exception^ e)
        {
            HandleError(e, "Transition error", "Scenario1_LowLatency::LatencyModeToggled");
        }
    });
}

void Scenario1_LowLatency::UpdateLocalClientWindow()
{
    PreviewButton->IsEnabled = true;
    ClientButton->IsEnabled = false;
    LatencyModeToggle->IsEnabled = true;

    if (_latencyMode == LatencyModes::highLatency)
    {
        RealTimePlaybackVideo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        PlaybackVideo->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
    else
    {
        PlaybackVideo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        RealTimePlaybackVideo->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}

void Scenario1_LowLatency::Reset()
{
    _sdkSample.ClearLastStatus();
    PreviewVideo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    WebcamPreviewPoster->Visibility = Windows::UI::Xaml::Visibility::Visible;
    PlaybackVideo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    RealTimePlaybackVideo->Visibility = Windows::UI::Xaml::Visibility::Visible;

    PreviewButton->Content = "Start Preview";
    ClientButton->IsEnabled = false;
    _latencyMode = LatencyModes::highLatency;
    LatencyModeToggle->IsEnabled = false;
    LatencyModeToggle->IsOn = (_latencyMode == LatencyModes::lowLatency);

    if (_currentState == ScenarioOneStates::end)
    {
        PreviewButton->IsEnabled = false;
    }
}

//initializing
IAsyncAction^ Scenario1_LowLatency::initializing_previewSetupCompleted()
{
    return create_async([this]() -> task<void>
    {
        _mediaCaptureFailedEventRegistrationToken = _device->Failed += ref new CaptureFailedHandler(this, &Scenario1_LowLatency::OnMediaCaptureFailed);
        _incomingConnectionEventRegistrationToken = _device->IncomingConnectionEvent += ref new IncomingConnectionEventHandler(this, &Scenario1_LowLatency::OnIncomingConnection);
        _previousState = _currentState;
        _currentState = ScenarioOneStates::waiting;

        return create_task(_dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            PreviewButton->IsEnabled = true;
            _sdkSample.DisplayStatus("Done. Tap or click 'Start Preview' button to start webcam preview");
        })));
    });
}

//waiting
/// <summary>
/// Logic when preview button is clicked and the scenario is in "waiting" state.
/// </summary>
IAsyncAction^ Scenario1_LowLatency::waiting_previewButtonClicked()
{
    return create_async([this]() -> task<void>
    {
        return create_task(_dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            if (_previousState == ScenarioOneStates::initializing)
            {
                PreviewVideo->Source = _device->MediaCapture.Get();
            }
        }
            ))).then([this]()
        {
            return create_task(_device->MediaCapture.Get()->StartPreviewAsync()).then([this]()
            {
                _previousState = _currentState;
                _currentState = ScenarioOneStates::previewing;
                return _dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
                {
                    WebcamPreviewPoster->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
                    PreviewVideo->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    PreviewButton->Content = "Stop Preview";
                    ClientButton->IsEnabled = true;
                }));
            });
        });
    });
}

//previewing
/// <summary>
/// The logic when the "stop" preview button is clicked while the scenario is in "previewing" state.
/// </summary>
IAsyncAction^ Scenario1_LowLatency::previewing_previewButtonClicked()
{
    return create_async([this]() -> task<void>
    {
        return create_task(_device->MediaCapture->StopPreviewAsync()).then([this]()
        {
            if (ScenarioOneStates::previewing == _currentState)
            {
                _previousState = _currentState;
                _currentState = ScenarioOneStates::waiting;
            }
            return _dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
            {
                Reset();
            }));
        });
    });
}

IAsyncAction^ Scenario1_LowLatency::previewing_localClientButtonClicked()
{
    return create_async([this]() -> task<void>
    {
        return StartRecordingToCustomSink();
    });
}

IAsyncAction^ Scenario1_LowLatency::previewing_recordingStarted()
{
    return create_async([this]() -> task<void>
    {
        return create_task(_dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            _localHostVideo[_latencyMode]->Source = ref new Uri("stsp://localhost");
            _previousState = _currentState;
            _currentState = ScenarioOneStates::streaming;
        })));
    });
}

//streaming
/// <summary>
/// The logic when the "stop" preview button is clicked while the scenario is in "streaming" state.
/// </summary>
IAsyncAction^ Scenario1_LowLatency::streaming_previewButtonClicked()
{
    return create_async([this]() -> task<void>
    {
        return create_task(_dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            //// stop localhost client video
            auto currLocalHostVideo = _localHostVideo[_latencyMode];
            currLocalHostVideo->Pause();
        }
            ))).then([this]()
        {
            LatencyModes latencyMode = _latencyMode;

            //// stop streaming to network and close network sink
            return _device->StopRecordingAsync().then([this, latencyMode](task<void> asyncInfo)
            {
                return create_task(_dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, asyncInfo, latencyMode]()
                {
                    _localHostVideo[latencyMode]->Source = nullptr;
                    _latencyMode = LatencyModes::highLatency;
                }
                    ))).then([this]()
                {
                    return create_task(actionTransitionFunctions[ScenarioOneStates::previewing][Action::previewButtonClicked]()).then([this]()
                    {
                        _previousState = _currentState;
                        _currentState = ScenarioOneStates::waiting;
                    });
                });
            });
        });
    });
}

IAsyncAction^ Scenario1_LowLatency::streaming_latencyModeToggled()
{
    return create_async([this]() -> task<void>
    {
        if (!_device)
        {
            throw ref new Platform::Exception(E_NOINTERFACE);
        }

        return _device->StopRecordingAsync().then([this](task<void> asyncInfo)
        {
            try
            {
                asyncInfo.get();
            }
            catch (Platform::Exception^ e)
            {
                Windows::Foundation::HResult hr;
                hr.Value = e->HResult;
                _sdkSample.FormatError(hr, "Latency toggle error: ", "StopRecordingAsync: ");
                _currentState = ScenarioOneStates::end;
                throw e;
            }

            return create_task(_dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
            {
                _localHostVideo[_latencyMode]->Source = nullptr;
            }
            )));
        }
            ).then([this]()
        {
            _latencyMode = (_latencyMode == LatencyModes::highLatency) ?
                LatencyModes::lowLatency : LatencyModes::highLatency;

            return StartRecordingToCustomSink();
        });
    });
}

/// <summary>
/// Logic when the scenario is in "streaming" state and "recordingStarted"
/// event is triggered after toggling latency mode.
/// </summary>
IAsyncAction^ Scenario1_LowLatency::streaming_recordingStarted()
{
    return create_async([this]() -> task<void>
    {
        return create_task(_dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            _localHostVideo[_latencyMode]->Source = ref new Uri("stsp://localhost");
            _localHostVideo[_latencyMode]->Play();
        })));
    });
}

/// <summary>
/// Error handling
/// </summary>
void Scenario1_LowLatency::HandleError(Platform::Exception^ error, Platform::String^ msg, Platform::String^ prefix)
{
    UnlockScenarioChange();
    Windows::Foundation::HResult hr;
    hr.Value = error->HResult;
    _sdkSample.FormatError(hr, msg, prefix);
    _currentState = ScenarioOneStates::end;
}


void Scenario1_LowLatency::OnMediaFailed(Platform::Object ^sender, Windows::UI::Xaml::ExceptionRoutedEventArgs ^e)
{
    _sdkSample.DisplayError(e->ErrorMessage);
}

void Scenario1_LowLatency::LockScenarioChange()
{
    InterlockedCompareExchange(&_navigationDisabled, TRUE, FALSE);
}

void Scenario1_LowLatency::UnlockScenarioChange()
{
    InterlockedCompareExchange(&_navigationDisabled, FALSE, TRUE);
}
