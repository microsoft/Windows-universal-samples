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
#include "Scenario4.h"
#include "Scenario4.g.cpp"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    static constexpr float OSC_START_X = 100.0f;
    static constexpr float OSC_START_Y = 100.0f;
    static constexpr float OSC_X_LENGTH = 700.0f;
    static constexpr float OSC_TOTAL_HEIGHT = 200.0f;

    Scenario4::Scenario4()
    {
        InitializeComponent();
    }

    void Scenario4::OnNavigatedFrom(NavigationEventArgs const&)
    {
        ClearCapture();
    }

    void Scenario4::ClearCapture()
    {
        if (m_deviceStateChangeToken)
        {
            m_capture->DeviceStateChanged(std::exchange(m_deviceStateChangeToken, {}));
        }

        if (m_plotDataReadyToken)
        {
            m_capture->PlotDataReady(std::exchange(m_plotDataReadyToken, {}));
        }

        StopCapture();
        m_capture = nullptr;
    }

#pragma region UI Related Code
    // Updates transport controls based on current playstate
    void Scenario4::UpdateMediaControlUI(DeviceState deviceState)
    {
        switch (deviceState)
        {
        case DeviceState::Capturing:
            btnStartCapture().IsEnabled(false);
            btnStopCapture().IsEnabled(true);
            toggleMinimumLatency().IsEnabled(false);
            break;

        case DeviceState::Stopped:
            btnStartCapture().IsEnabled(true);
            btnStopCapture().IsEnabled(false);
            toggleMinimumLatency().IsEnabled(true);
            break;

        case DeviceState::Initialized:
        case DeviceState::Starting:
        case DeviceState::Stopping:
        case DeviceState::Flushing:
        case DeviceState::Error:
            btnStartCapture().IsEnabled(false);
            btnStopCapture().IsEnabled(false);
            break;
        }
    }
#pragma endregion

#pragma region UI Event Handlers
    void Scenario4::btnStartCapture_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);
        InitializeCapture();
    }

    void Scenario4::btnStopCapture_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StopCapture();
    }
#pragma endregion

    // Event callback from WASAPI capture for changes in device state
    fire_and_forget Scenario4::OnDeviceStateChange(IDeviceStateSource const&, SDKTemplate::DeviceStateChangedEventArgs e)
    {
        auto lifetime = get_strong();

        // Get the current time for messages
        std::time_t now = clock::to_time_t(clock::now());
        char buffer[26];
        ctime_s(buffer, ARRAYSIZE(buffer), &now);

        // Continue on UI thread.
        co_await resume_foreground(Dispatcher());

        // Update Control Buttons
        DeviceState state = e.DeviceState();
        UpdateMediaControlUI(state);

        // Handle state specific messages
        switch (state)
        {
        case DeviceState::Initialized:
            m_capture->AsyncStartCapture();
            break;

        case DeviceState::Capturing:
            if (m_isLowLatency)
            {
                rootPage.NotifyUser(L"Capture Started (minimum latency) at " + to_hstring(buffer), NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(L"Capture Started (normal latency) at " + to_hstring(buffer), NotifyType::StatusMessage);
            }
            break;

        case DeviceState::Discontinuity:
        {
            m_discontinuityCount++;

            if (m_discontinuityCount > 0)
            {
                rootPage.NotifyUser(L"DISCONTINUITY DETECTED: " + to_hstring(buffer) + L" (Count = " + to_hstring(m_discontinuityCount) + L")", NotifyType::StatusMessage);
            }
        }
        break;

        case DeviceState::Flushing:
            if (m_plotDataReadyToken)
            {
                m_capture->PlotDataReady(std::exchange(m_plotDataReadyToken, {}));
            }

            rootPage.NotifyUser(L"Finalizing WAV Header.  This may take a few minutes...", NotifyType::StatusMessage);

            Oscilloscope().Points().ReplaceAll({ { OSC_START_X, OSC_START_Y }, { OSC_X_LENGTH, OSC_START_Y } });
            break;

        case DeviceState::Stopped:
            // For the stopped state, completely tear down the audio device
            ClearCapture();

            rootPage.NotifyUser(L"Capture Stopped", NotifyType::StatusMessage);
            break;

        case DeviceState::Error:
            ClearCapture();

            // Specifically handle a couple of known errors
            switch (hresult error = e.ExtendedError(); error)
            {
            case __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
                rootPage.NotifyUser(L"ERROR: Check the Sound control panel for an active recording device.", NotifyType::ErrorMessage);
                break;

            case AUDCLNT_E_RESOURCES_INVALIDATED:
                rootPage.NotifyUser(L"ERROR: Endpoint Lost Access To Resources", NotifyType::ErrorMessage);
                break;

            case E_ACCESSDENIED:
                rootPage.NotifyUser(L"ERROR: Access Denied.  Check 'Settings->Permissions' for access to Microphone.", NotifyType::ErrorMessage);
                break;

            default:
                rootPage.NotifyUser(L"ERROR: " + hresult_error(error).message(), NotifyType::ErrorMessage);
                break;
            }
        }
    }

    // Event callback when visualization data is ready to be plotted
    fire_and_forget Scenario4::OnPlotDataReady(IPlotDataSource const&, SDKTemplate::PlotDataReadyEventArgs e)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(Dispatcher());

        IBuffer buffer = e.Points();
        size_t pointCount = buffer.Length() / sizeof(int16_t);
        int16_t* points = reinterpret_cast<int16_t*>(buffer.data());

        // Scale the incoming point list so that it fills our scope.
        float y_range = static_cast<float>((std::numeric_limits<int16_t>::max)()) - std::numeric_limits<int16_t>::lowest();
        float x_inc = pointCount > 0 ? OSC_X_LENGTH / static_cast<float>(pointCount - 1) : 0.0f;

        UINT32 existingPoints = Oscilloscope().Points().Size();

        for (UINT32 i = 0; i < pointCount; i++)
        {
            Point p;

            // Fixup the x and y coordinates and set it back into the collection
            p.X = OSC_START_X + i * x_inc;
            p.Y = OSC_START_Y - (points[i] * OSC_TOTAL_HEIGHT) / y_range;

            if (i < existingPoints)
            {
                Oscilloscope().Points().SetAt(i, p);
            }
            else
            {
                Oscilloscope().Points().Append(p);
            }
        }
    }

    //
    //  InitializeCapture()
    //
    void Scenario4::InitializeCapture()
    {
        m_capture = nullptr;

        // Create a new WASAPI capture instance
        m_capture = make_self<WASAPICapture>();

        // Register for events
        m_deviceStateChangeToken = m_capture->DeviceStateChanged({ get_weak(), &Scenario4::OnDeviceStateChange });
        m_plotDataReadyToken = m_capture->PlotDataReady({ get_weak(), &Scenario4::OnPlotDataReady });

        // There is an initial discontinuity when we start, so reset discontinuity counter
        // to -1 so that we ignore that initial discontinuity.
        m_discontinuityCount = -1;

        // Configure whether we are using low-latency capture
        m_isLowLatency = toggleMinimumLatency().IsOn();
        m_capture->SetLowLatencyCapture(m_isLowLatency);

        // Perform the initialization
        m_capture->AsyncInitializeAudioDevice();
    }

    //
    //  StopCapture()
    //
    void Scenario4::StopCapture()
    {
        if (m_capture)
        {
            // Set the event to stop playback
            m_capture->AsyncStopCapture();
        }
    }
}
