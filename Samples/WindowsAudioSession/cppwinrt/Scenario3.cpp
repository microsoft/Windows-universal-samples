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
#include "Scenario3.h"
#include "Scenario3.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Devices;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml::Controls::Primitives;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3::Scenario3()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario3::OnNavigatedTo(NavigationEventArgs const&)
    {
        auto lifetime = get_strong();

        // Register for Media Transport controls.  This is required to support background
        // audio scenarios.
        m_systemMediaControls = SystemMediaTransportControls::GetForCurrentView();
        m_systemMediaControlsButtonToken = m_systemMediaControls.ButtonPressed({ get_weak(), &Scenario3::MediaButtonPressed });
        m_systemMediaControls.IsPlayEnabled(true);
        m_systemMediaControls.IsPauseEnabled(true);
        m_systemMediaControls.IsStopEnabled(true);

        UpdateContentUI();

        // Get a string representing the Default Audio Render Device
        hstring deviceId = MediaDevice::GetDefaultAudioRenderId(AudioDeviceRole::Default);

        // The string is empty if there is no such device.
        if (deviceId.empty())
        {
            rootPage.NotifyUser(L"No audio devices available", NotifyType::StatusMessage);
            co_return;
        }

        // read property store to see if the device supports a RAW processing mode
        static constexpr wchar_t PKEY_AudioDevice_RawProcessingSupported[] = L"System.Devices.AudioDevice.RawProcessingSupported";
        DeviceInformation deviceInformation = co_await DeviceInformation::CreateFromIdAsync(deviceId, { PKEY_AudioDevice_RawProcessingSupported });

        std::optional<bool> obj = deviceInformation.Properties().TryLookup(PKEY_AudioDevice_RawProcessingSupported).try_as<bool>();
        m_deviceSupportsRawMode = (obj == true);
        toggleRawAudio().IsEnabled(m_deviceSupportsRawMode);

        if (m_deviceSupportsRawMode)
        {
            rootPage.NotifyUser(L"Raw Supported", NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Raw Not Supported", NotifyType::StatusMessage);
        }
    }

    void Scenario3::OnNavigatedFrom(NavigationEventArgs const&)
    {
        StopDevice();

        if (m_deviceStateChangeToken)
        {
            m_renderer->DeviceStateChanged(std::exchange(m_deviceStateChangeToken, {}));
        }

        if (m_systemMediaControlsButtonToken)
        {
            m_systemMediaControls.ButtonPressed(m_systemMediaControlsButtonToken);
            m_systemMediaControls.IsPlayEnabled(false);
            m_systemMediaControls.IsPauseEnabled(false);
            m_systemMediaControls.IsStopEnabled(false);
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Closed);
        }
    }

#pragma region UI Related Code

    void Scenario3::DisableContentUI()
    {
        btnFilePicker().IsEnabled(false);
        sliderFrequency().IsEnabled(false);
        radioFile().IsEnabled(false);
        radioTone().IsEnabled(false);
    }

    // Updates content controls based on selected option
    void Scenario3::UpdateContentUI()
    {
        // Ignore calls triggered by binding before construction is complete.
        if (radioFile() == nullptr)
        {
            return;
        }

        radioFile().IsEnabled(true);
        radioTone().IsEnabled(true);

        switch (m_contentType)
        {
        case ContentType::Tone:
            btnFilePicker().IsEnabled(false);
            sliderFrequency().IsEnabled(true);
            UpdateContentProps(to_hstring(sliderFrequency().Value()) + L" Hz");
            break;

        case ContentType::File:
            btnFilePicker().IsEnabled(true);
            sliderFrequency().IsEnabled(false);
            break;

        default:
            break;
        }
    }

    // Updates transport controls based on current playstate
    void Scenario3::UpdateMediaControlUI(DeviceState deviceState)
    {
        switch (deviceState)
        {
        case DeviceState::Playing:
            btnPlay().IsEnabled(false);
            btnStop().IsEnabled(true);
            btnPlayPause().IsEnabled(true);
            btnPause().IsEnabled(true);
            toggleMinimumLatency().IsEnabled(false);
            break;

        case DeviceState::Stopped:
        case DeviceState::Error:
            btnPlay().IsEnabled(true);
            btnStop().IsEnabled(false);
            btnPlayPause().IsEnabled(true);
            btnPause().IsEnabled(false);
            toggleMinimumLatency().IsEnabled(true);

            UpdateContentUI();
            break;

        case DeviceState::Paused:
            btnPlay().IsEnabled(true);
            btnStop().IsEnabled(true);
            btnPlayPause().IsEnabled(true);
            btnPause().IsEnabled(false);
            break;

        case DeviceState::Starting:
        case DeviceState::Stopping:
            btnPlay().IsEnabled(false);
            btnStop().IsEnabled(false);
            btnPlayPause().IsEnabled(false);
            btnPause().IsEnabled(false);

            DisableContentUI();
            break;
        }
    }

    // Updates textbox
    void Scenario3::UpdateContentProps(hstring const& text)
    {
        // Ignore calls triggered by binding before construction is complete.
        if (txtContentProps() == nullptr)
        {
            return;
        }

        txtContentProps().Text(text);

        if (text.empty() && (m_contentType == ContentType::File))
        {
            txtContentProps().Background(SolidColorBrush({ 0xCC, 0xFF, 0x00, 0x00 }));
        }
        else
        {
            txtContentProps().Background(nullptr);
        }
    }

#pragma endregion

#pragma region UI Event Handlers
    void Scenario3::sliderFrequency_ValueChanged(IInspectable const&, RangeBaseValueChangedEventArgs const& e)
    {
        UpdateContentProps(to_hstring(e.NewValue()) + L" Hz");
    }

    void Scenario3::sliderVolume_ValueChanged(IInspectable const&, RangeBaseValueChangedEventArgs const& e)
    {
        OnSetVolume(e.NewValue());
    }

    void Scenario3::radioTone_Checked(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        m_contentType = ContentType::Tone;
        UpdateContentProps(L"");
        m_contentStream = nullptr;
        UpdateContentUI();
    }

    void Scenario3::radioFile_Checked(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        m_contentType = ContentType::File;
        UpdateContentProps(L"");
        m_contentStream = nullptr;
        UpdateContentUI();
    }

    void Scenario3::btnPlay_Click(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        StartDevice();
    }

    void Scenario3::btnPause_Click(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        PauseDevice();
    }

    void Scenario3::btnPlayPause_Click(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        PlayPauseToggleDevice();
    }

    void Scenario3::btnStop_Click(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        StopDevice();
    }

    void Scenario3::btnFilePicker_Click(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        OnPickFileAsync();
    }

#pragma endregion

    // Event callback from WASAPI renderer for changes in device state
    fire_and_forget Scenario3::OnDeviceStateChange(IDeviceStateSource const&, SDKTemplate::DeviceStateChangedEventArgs e)
    {
        auto lifetime = get_strong();

        // Handle state specific messages
        switch (e.DeviceState())
        {
        case DeviceState::Initialized:
            StartDevice();
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Closed);
            break;

        case DeviceState::Playing:
            if (m_isMinimumLatency == true)
            {
                rootPage.NotifyUser(L"Playback Started (minimum latency)", NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(L"Playback Started (normal latency)", NotifyType::StatusMessage);
            }
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Playing);
            break;

        case DeviceState::Paused:
            rootPage.NotifyUser(L"Playback Paused", NotifyType::StatusMessage);
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Paused);
            break;

        case DeviceState::Stopped:
            if (m_deviceStateChangeToken)
            {
                m_renderer->DeviceStateChanged(std::exchange(m_deviceStateChangeToken, {}));
            }
            m_renderer = nullptr;

            rootPage.NotifyUser(L"Playback Stopped", NotifyType::StatusMessage);
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Stopped);
            break;

        case DeviceState::Error:
            if (m_deviceStateChangeToken)
            {
                m_renderer->DeviceStateChanged(std::exchange(m_deviceStateChangeToken, {}));
            }
            m_renderer = nullptr;

            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Closed);

            // Specifically handle a couple of known errors
            auto error = e.ExtendedError();
            switch (error)
            {
            case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE:
                rootPage.NotifyUser(L"ERROR: Endpoint Does Not Support HW Offload", NotifyType::ErrorMessage);
                break;

            case AUDCLNT_E_RESOURCES_INVALIDATED:
                rootPage.NotifyUser(L"ERROR: Endpoint Lost Access To Resources", NotifyType::ErrorMessage);
                break;

            default:
                rootPage.NotifyUser(L"ERROR: " + hresult_error(error).message(), NotifyType::ErrorMessage);
                break;
            }
        }

        // Update Control Buttons
        co_await resume_foreground(Dispatcher());
        UpdateMediaControlUI(e.DeviceState());
    }

    //
    //  OnPickFileAsync()
    //
    //  File chooser for MF Source playback.  Retrieves a pointer to IRandomAccessStream
    //
    fire_and_forget Scenario3::OnPickFileAsync()
    {
        auto lifetime = get_strong();
        FileOpenPicker filePicker;
        filePicker.ViewMode(PickerViewMode::List);
        filePicker.SuggestedStartLocation(PickerLocationId::MusicLibrary);
        filePicker.FileTypeFilter().ReplaceAll({ L".wav", L".mp3", L".wma" });

        StorageFile file = co_await filePicker.PickSingleFileAsync();
        if (file != nullptr)
        {
            // Open the stream
            IRandomAccessStream stream = co_await file.OpenAsync(FileAccessMode::Read);
            if (stream != nullptr)
            {
                m_contentStream = stream;
                UpdateContentProps(file.Path());
            }
        }
    }

    //
    //  OnSetVolume()
    //
    //  Updates the session volume
    //
    void Scenario3::OnSetVolume(double volume)
    {
        if (m_renderer)
        {
            // Updates the Session volume on the AudioClient
            m_renderer->SetVolumeOnSession(volume);
        }
    }

    //
    //  InitializeDevice()
    //
    //  Sets up a new instance of the WASAPI renderer
    //
    void Scenario3::InitializeDevice()
    {
        if (m_renderer)
        {
            // Already initialized.
            return;
        }

        // Create a new WASAPI instance
        m_renderer = make_self<WASAPIRenderer>();

        // Register for events
        m_deviceStateChangeToken = m_renderer->DeviceStateChanged({ get_weak(), &Scenario3::OnDeviceStateChange });

        // Configure user based properties
        m_renderer->SetContentType(m_contentType);
        m_renderer->SetFrequency(sliderFrequency().Value());
        m_renderer->SetContentStream(m_contentStream);

        m_isMinimumLatency = toggleMinimumLatency().IsOn();
        m_renderer->SetLowLatency(m_isMinimumLatency);
        m_renderer->SetHWOffload(false);
        m_renderer->SetBackgroundAudio(false);
        m_renderer->SetRawAudio(m_deviceSupportsRawMode && toggleRawAudio().IsOn());

        // Selects the Default Audio Device
        m_renderer->AsyncInitializeAudioDevice();

        // Set the initial volume.
        OnSetVolume(sliderVolume().Value());
    }

    void Scenario3::StartDevice()
    {
        if (m_renderer == nullptr)
        {
            // Call from main UI thread
            InitializeDevice();
        }
        else
        {
            // Starts a work item to begin playback, likely in the paused state
            m_renderer->StartPlayback();
        }
    }

    void Scenario3::StopDevice()
    {
        if (m_renderer)
        {
            // Set the event to stop playback
            m_renderer->StopPlaybackAsync();
        }
    }

    void Scenario3::PauseDevice()
    {
        if (m_renderer)
        {
            if (m_renderer->DeviceState() == DeviceState::Playing)
            {
                // Starts a work item to pause playback
                m_renderer->PausePlaybackAsync();
            }
        }
    }    

    //
    //  PlayPauseToggleDevice()
    //
    void Scenario3::PlayPauseToggleDevice()
    {
        if (m_renderer)
        {
            DeviceState deviceState = m_renderer->DeviceState();

            if (deviceState == DeviceState::Playing)
            {
                // Starts a work item to pause playback
                m_renderer->PausePlaybackAsync();
            }
            else if (deviceState == DeviceState::Paused)
            {
                // Starts a work item to pause playback
                m_renderer->StartPlayback();
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
    fire_and_forget Scenario3::MediaButtonPressed(SystemMediaTransportControls const&, SystemMediaTransportControlsButtonPressedEventArgs e)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        switch (e.Button())
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
    }
}
