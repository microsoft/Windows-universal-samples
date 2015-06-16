//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario3.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include "Scenario3.xaml.h"
#include <ppl.h>
using namespace concurrency;

using namespace SDKSample;
using namespace SDKSample::WASAPIAudio;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Media;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;


Scenario3::Scenario3() :
    m_IsMFLoaded(false),
    m_ContentStream(nullptr),
    m_StateChangedEvent(nullptr),
    m_spRenderer(nullptr),
    m_ContentType(ContentType::ContentTypeTone),
    m_IsMinimumLatency(false)
{
    InitializeComponent();

    // Initialize MF
    HRESULT hr = MFStartup(MF_VERSION, MFSTARTUP_LITE);
    if (SUCCEEDED(hr))
    {
        m_IsMFLoaded = true;
    }
    else
    {
        ThrowIfFailed(hr);
    }

    m_CoreDispatcher = CoreWindow::GetForCurrentThread()->Dispatcher;

    // Get a string representing the Default Audio Render Device
    String^ deviceId = Windows::Media::Devices::MediaDevice::GetDefaultAudioRenderId(Windows::Media::Devices::AudioDeviceRole::Default);

    auto PropertiesToRetrieve = ref new Platform::Collections::Vector<String^>();
    PropertiesToRetrieve->Append("System.Devices.AudioDevice.RawProcessingSupported");
    
    // read property store to see if the device supports a RAW processing mode
    create_task(Windows::Devices::Enumeration::DeviceInformation::CreateFromIdAsync(deviceId, PropertiesToRetrieve)).then([this] (Windows::Devices::Enumeration::DeviceInformation^ deviceInformation)
    {
        auto obj = deviceInformation->Properties->Lookup("System.Devices.AudioDevice.RawProcessingSupported");
        this->m_deviceSupportsRawMode = false;
        if (obj)
        {
            this->m_deviceSupportsRawMode = obj->Equals(true);
        }

        if (this->m_deviceSupportsRawMode)
        {
            this->toggleRawAudio->IsEnabled = true;
            ShowStatusMessage("Raw Supported", NotifyType::StatusMessage);
        }
        else
        {
            this->toggleRawAudio->IsEnabled = false;
            ShowStatusMessage("Raw Not Supported", NotifyType::StatusMessage);
        }
    });

    // Register for Media Transport controls.  This is required to support background
    // audio scenarios.
    m_SystemMediaControls = SystemMediaTransportControls::GetForCurrentView();
    m_SystemMediaControlsButtonToken = m_SystemMediaControls->ButtonPressed += ref new TypedEventHandler<SystemMediaTransportControls^, SystemMediaTransportControlsButtonPressedEventArgs^>(this, &Scenario3::MediaButtonPressed);
    m_SystemMediaControls->IsPlayEnabled = true;
    m_SystemMediaControls->IsPauseEnabled = true;
    m_SystemMediaControls->IsStopEnabled = true;
}

Scenario3::~Scenario3()
{
    if (m_deviceStateChangeToken.Value != 0)
    {
        m_StateChangedEvent->StateChangedEvent -= m_deviceStateChangeToken;
        m_StateChangedEvent = nullptr;
        m_deviceStateChangeToken.Value = 0;
    }

    if (m_SystemMediaControls)
    {
        m_SystemMediaControls->ButtonPressed -= m_SystemMediaControlsButtonToken;
        m_SystemMediaControls->IsPlayEnabled = false;
        m_SystemMediaControls->IsPauseEnabled = false;
        m_SystemMediaControls->IsStopEnabled = false;
        m_SystemMediaControls->PlaybackStatus = MediaPlaybackStatus::Closed;
    }

    // Uninitialize MF
    if (m_IsMFLoaded)
    {
        MFShutdown();
        m_IsMFLoaded = false;
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
    UpdateContentUI(false);
}

/// <summary>
/// Invoked when about to leave this page.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (nullptr != m_StateChangedEvent)
    {
        DeviceState deviceState = m_StateChangedEvent->GetState();

        if (deviceState == DeviceState::DeviceStatePlaying)
        {
            StopDevice();
        }
    }
}

#pragma region UI Related Code
void Scenario3::ShowStatusMessage(Platform::String^ str, NotifyType messageType)
{
    m_CoreDispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(
        [this, str, messageType]()
    {
        rootPage->NotifyUser(str, messageType);
    }));
}

// Updates content controls based on selected option
void Scenario3::UpdateContentUI(Platform::Boolean disableAll)
{
    if ((nullptr != btnFilePicker) &&
        (nullptr != sliderFrequency))
    {
        if (disableAll)
        {
            btnFilePicker->IsEnabled = false;
            sliderFrequency->IsEnabled = false;
            radioFile->IsEnabled = false;
            radioTone->IsEnabled = false;
        }
        else
        {
            radioFile->IsEnabled = true;
            radioTone->IsEnabled = true;

            switch (m_ContentType)
            {
            case ContentType::ContentTypeTone:
                btnFilePicker->IsEnabled = false;
                sliderFrequency->IsEnabled = true;
                UpdateContentProps(sliderFrequency->Value.ToString() + " Hz");
                break;

            case ContentType::ContentTypeFile:
                btnFilePicker->IsEnabled = true;
                sliderFrequency->IsEnabled = false;
                break;

            default:
                break;
            }
        }
    }
}

// Updates transport controls based on current playstate
void Scenario3::UpdateMediaControlUI(DeviceState deviceState)
{
    switch (deviceState)
    {
    case DeviceState::DeviceStatePlaying:
        btnPlay->IsEnabled = false;
        btnStop->IsEnabled = true;
        btnPlayPause->IsEnabled = true;
        btnPause->IsEnabled = true;
        toggleMinimumLatency->IsEnabled = false;
        break;

    case DeviceState::DeviceStateStopped:
    case DeviceState::DeviceStateInError:
        btnPlay->IsEnabled = true;
        btnStop->IsEnabled = false;
        btnPlayPause->IsEnabled = true;
        btnPause->IsEnabled = false;
        toggleMinimumLatency->IsEnabled = true;

        UpdateContentUI(false);
        break;

    case DeviceState::DeviceStatePaused:
        btnPlay->IsEnabled = true;
        btnStop->IsEnabled = true;
        btnPlayPause->IsEnabled = true;
        btnPause->IsEnabled = false;
        break;

    case DeviceState::DeviceStateStarting:
    case DeviceState::DeviceStateStopping:
        btnPlay->IsEnabled = false;
        btnStop->IsEnabled = false;
        btnPlayPause->IsEnabled = false;
        btnPause->IsEnabled = false;

        UpdateContentUI(true);
        break;
    }
}

// Updates textbox on UI thread
void Scenario3::UpdateContentProps(String^ str)
{
    String^ text = str;

    if (nullptr != txtContentProps)
    {
        // The event handler may be invoked on a background thread, so use the Dispatcher to invoke the UI-related code on the UI thread.
        txtContentProps->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(
            [this, text]()
        {
            Windows::UI::Xaml::Media::SolidColorBrush ^brush;
            txtContentProps->Text = text;

            if (("" == text) && (m_ContentType == ContentType::ContentTypeFile))
            {
                brush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::ColorHelper::FromArgb(0xCC, 0xFF, 0x00, 0x00));
            }
            else
            {
                brush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::ColorHelper::FromArgb(0xFF, 0xFF, 0xFF, 0xFF));
            }

            txtContentProps->Background = brush;
        }));
    }
}

#pragma endregion

#pragma region UI Event Handlers
void Scenario3::sliderFrequency_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    Slider^ s = safe_cast<Slider^>(sender) ;
    if (s != nullptr)
    {
        UpdateContentProps(s->Value.ToString() + " Hz");
    }
}

void Scenario3::sliderVolume_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    Slider^ s = safe_cast<Slider^>(sender) ;
    if (s != nullptr)
    {
        OnSetVolume(static_cast<UINT32>(s->Value));
    }
}

void Scenario3::radioTone_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    m_ContentType = ContentType::ContentTypeTone;
    UpdateContentProps("");
    m_ContentStream = nullptr;
    UpdateContentUI(false);
}

void Scenario3::radioFile_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    m_ContentType = ContentType::ContentTypeFile;
    UpdateContentProps("");
    m_ContentStream = nullptr;
    UpdateContentUI(false);
}

void Scenario3::btnPlay_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartDevice();
}

void Scenario3::btnPause_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    PauseDevice();
}

void Scenario3::btnPlayPause_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    PlayPauseToggleDevice();
}

void Scenario3::btnStop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopDevice();
}

void Scenario3::btnFilePicker_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    OnPickFileAsync();
}

#pragma endregion

// Event callback from WASAPI renderer for changes in device state
void Scenario3::OnDeviceStateChange(Object^ sender, DeviceStateChangedEventArgs^ e)
{
    // Update Control Buttons
    m_CoreDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(
        [this, e]()
    {
        UpdateMediaControlUI(e->State);
    }));

    // Handle state specific messages
    switch (e->State)
    {
    case DeviceState::DeviceStateInitialized:
        StartDevice();
        m_SystemMediaControls->PlaybackStatus = MediaPlaybackStatus::Closed;
        break;

    case DeviceState::DeviceStatePlaying:
        if (m_IsMinimumLatency == true)
        {
            ShowStatusMessage("Playback Started (minimum latency)", NotifyType::StatusMessage);
        }
        else
        {
            ShowStatusMessage("Playback Started (normal latency)", NotifyType::StatusMessage);
        }
        m_SystemMediaControls->PlaybackStatus = MediaPlaybackStatus::Playing;
        break;

    case DeviceState::DeviceStatePaused:
        ShowStatusMessage("Playback Paused", NotifyType::StatusMessage);
        m_SystemMediaControls->PlaybackStatus = MediaPlaybackStatus::Paused;
        break;

    case DeviceState::DeviceStateStopped:
        m_spRenderer = nullptr;

        if (m_deviceStateChangeToken.Value != 0)
        {
            m_StateChangedEvent->StateChangedEvent -= m_deviceStateChangeToken;
            m_StateChangedEvent = nullptr;
            m_deviceStateChangeToken.Value = 0;
        }

        ShowStatusMessage("Playback Stopped", NotifyType::StatusMessage);
        m_SystemMediaControls->PlaybackStatus = MediaPlaybackStatus::Stopped;
        break;

    case DeviceState::DeviceStateInError:
        HRESULT hr = e->hr;

        if (m_deviceStateChangeToken.Value != 0)
        {
            m_StateChangedEvent->StateChangedEvent -= m_deviceStateChangeToken;
            m_StateChangedEvent = nullptr;
            m_deviceStateChangeToken.Value = 0;
        }

        m_spRenderer = nullptr;

        m_SystemMediaControls->PlaybackStatus = MediaPlaybackStatus::Closed;

        wchar_t hrVal[11];
        swprintf_s(hrVal, 11, L"0x%08x\0", hr);
        String^ strHRVal = ref new String(hrVal);

        String^ strMessage = "";

        // Specifically handle a couple of known errors
        switch (hr)
        {
        case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE:
            strMessage = "ERROR: Endpoint Does Not Support HW Offload (" + strHRVal + ")";
            ShowStatusMessage(strMessage, NotifyType::ErrorMessage);
            break;

        case AUDCLNT_E_RESOURCES_INVALIDATED:
            strMessage = "ERROR: Endpoint Lost Access To Resources (" + strHRVal + ")";
            ShowStatusMessage(strMessage, NotifyType::ErrorMessage);
            break;

        default:
            strMessage = "ERROR: " + strHRVal + " has occurred.";
            ShowStatusMessage(strMessage, NotifyType::ErrorMessage);
        }
    }
}

//
//  OnPickFileAsync()
//
//  File chooser for MF Source playback.  Retrieves a pointer to IRandomAccessStream
//
void Scenario3::OnPickFileAsync()
{
    Pickers::FileOpenPicker^ filePicker = ref new Pickers::FileOpenPicker();
    filePicker->ViewMode = Pickers::PickerViewMode::List;
    filePicker->SuggestedStartLocation = Pickers::PickerLocationId::MusicLibrary;
    filePicker->FileTypeFilter->Append(".wav");
    filePicker->FileTypeFilter->Append(".mp3");
    filePicker->FileTypeFilter->Append(".wma");

    concurrency::create_task(filePicker->PickSingleFileAsync()).then(
        [this](Windows::Storage::StorageFile^ file)
    {
        if (nullptr != file)
        {
            // Open the stream
            concurrency::create_task(file->OpenAsync(FileAccessMode::Read)).then(
                [this, file](IRandomAccessStream^ stream)
            {
                if (stream != nullptr)
                {
                    m_ContentStream = stream;
                    UpdateContentProps(file->Path);
                }
            });
        }
    });
}

//
//  OnSetVolume()
//
//  Updates the session volume
//
void Scenario3::OnSetVolume(UINT32 volume)
{
    if (m_spRenderer)
    {
        // Updates the Session volume on the AudioClient
        m_spRenderer->SetVolumeOnSession(volume);
    }
}

//
//  InitializeDevice()
//
//  Sets up a new instance of the WASAPI renderer
//
void Scenario3::InitializeDevice()
{
    HRESULT hr = S_OK;

    if (!m_spRenderer)
    {
        // Create a new WASAPI instance
        m_spRenderer = Make<WASAPIRenderer>();

        if (nullptr == m_spRenderer)
        {
            OnDeviceStateChange(this, ref new DeviceStateChangedEventArgs(DeviceState::DeviceStateInError, E_OUTOFMEMORY));
            return;
        }

        // Get a pointer to the device event interface
        m_StateChangedEvent = m_spRenderer->GetDeviceStateEvent();

        if (nullptr == m_StateChangedEvent)
        {
            OnDeviceStateChange(this, ref new DeviceStateChangedEventArgs(DeviceState::DeviceStateInError, E_FAIL));
            return;
        }

        // Register for events
        m_deviceStateChangeToken = m_StateChangedEvent->StateChangedEvent += ref new DeviceStateChangedHandler(this, &Scenario3::OnDeviceStateChange);

        // Configure user based properties
        DEVICEPROPS props;

        switch (m_ContentType)
        {
        case ContentType::ContentTypeTone:
            props.IsTonePlayback = true;
            props.Frequency = static_cast<DWORD>(sliderFrequency->Value);
            break;

        case ContentType::ContentTypeFile:
            props.IsTonePlayback = false;
            props.ContentStream = m_ContentStream;
            break;
        }

        m_IsMinimumLatency = static_cast<Platform::Boolean>(toggleMinimumLatency->IsOn);
        
        props.IsLowLatency = m_IsMinimumLatency;
        props.IsHWOffload = false;
        props.IsBackground = false;
        props.IsRawChosen = static_cast<Platform::Boolean>(toggleRawAudio->IsOn);
        props.IsRawSupported = m_deviceSupportsRawMode;
        
        m_spRenderer->SetProperties(props);

        // Selects the Default Audio Device
        m_spRenderer->InitializeAudioDeviceAsync();
    }
}

//
//  StartDevice()
//
void Scenario3::StartDevice()
{
    if (nullptr == m_spRenderer)
    {
        // Call from main UI thread
        InitializeDevice();
    }
    else
    {
        // Starts a work item to begin playback, likely in the paused state
        m_spRenderer->StartPlaybackAsync();
    }
}

//
//  StopDevice()
//
void Scenario3::StopDevice()
{
    if (m_spRenderer)
    {
        // Set the event to stop playback
        m_spRenderer->StopPlaybackAsync();
    }
}

//
//  PauseDevice()
//
void Scenario3::PauseDevice()
{
    if (m_spRenderer)
    {
        DeviceState deviceState = m_StateChangedEvent->GetState();

        if (deviceState == DeviceState::DeviceStatePlaying)
        {
            // Starts a work item to pause playback
            m_spRenderer->PausePlaybackAsync();
        }
    }
}

//
//  PlayPauseToggleDevice()
//
void Scenario3::PlayPauseToggleDevice()
{
    if (m_spRenderer)
    {
        DeviceState deviceState = m_StateChangedEvent->GetState();

        if (deviceState == DeviceState::DeviceStatePlaying)
        {
            // Starts a work item to pause playback
            m_spRenderer->PausePlaybackAsync();
        }
        else if (deviceState == DeviceState::DeviceStatePaused)
        {
            // Starts a work item to pause playback
            m_spRenderer->StartPlaybackAsync();
        }
    }
    else
    {
        StartDevice();
    }
}

//
//   MediaButtonPressed
//
void Scenario3::MediaButtonPressed(SystemMediaTransportControls^ sender, SystemMediaTransportControlsButtonPressedEventArgs^ e)
{
    m_CoreDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(
        [this, e]()
    {
        switch (e->Button)
        {
        case SystemMediaTransportControlsButton::Play:
            StartDevice();
            break;

        case SystemMediaTransportControlsButton::Pause:
            PauseDevice();
            break;

        case SystemMediaTransportControlsButton::Stop:
            StopDevice();
            break;

        default:
            break;
        }
    }));
}
