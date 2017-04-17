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
#include "Scenario1_OmnidirectionalSound.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_OmnidirectionalSound::Scenario1_OmnidirectionalSound() : _rootPage(MainPage::Current)
{
    InitializeComponent();
    auto hr = _omnidirectionalSound.Initialize(L"assets//MonoSound.wav");
    if (SUCCEEDED(hr))
    {
        _timer = ref new DispatcherTimer();
        _timerEventToken = _timer->Tick += ref new EventHandler<Platform::Object^>(this, &Scenario1_OmnidirectionalSound::OnTimerTick);
        TimeSpan timespan;
        timespan.Duration = 10000 / 30;
        _timer->Interval = timespan;
        EnvironmentComboBox->SelectedIndex = static_cast<int>(_omnidirectionalSound.GetEnvironment());
        _rootPage->NotifyUser("Stopped", NotifyType::StatusMessage);
    }
    else
    {
        if (hr == E_NOTIMPL)
        {
            _rootPage->NotifyUser("HRTF API is not supported on this platform. Use X3DAudio API instead - https://code.msdn.microsoft.com/XAudio2-Win32-Samples-024b3933", NotifyType::ErrorMessage);
        }
        else
        {
            throw ref new COMException(hr);
        }
    }

    _initialized = SUCCEEDED(hr);
}

Scenario1_OmnidirectionalSound::~Scenario1_OmnidirectionalSound()
{
    if (_timerEventToken.Value != 0)
    {
        _timer->Tick -= _timerEventToken;
    }
}

void SDKTemplate::Scenario1_OmnidirectionalSound::PlayButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_initialized)
    {
        _omnidirectionalSound.Start();
        _timer->Start();
        _rootPage->NotifyUser("Playing", NotifyType::StatusMessage);
    }
}

void SDKTemplate::Scenario1_OmnidirectionalSound::StopButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_initialized)
    {
        _omnidirectionalSound.Stop();
        _timer->Stop();
        _rootPage->NotifyUser("Stopped", NotifyType::StatusMessage);
    }
}

void SDKTemplate::Scenario1_OmnidirectionalSound::OnTimerTick(Object^ sender, Object^ e)
{
    // Update the sound position on every dispatcher timer tick.
    _omnidirectionalSound.OnUpdate(_angularVelocity, _height, _radius);
}

void SDKTemplate::Scenario1_OmnidirectionalSound::RadiusSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _radius = static_cast<float>(e->NewValue);
}

void SDKTemplate::Scenario1_OmnidirectionalSound::EnvironmentComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    if (_initialized)
    {
        _omnidirectionalSound.SetEnvironment(static_cast<HrtfEnvironment>(EnvironmentComboBox->SelectedIndex));
    }
}

void SDKTemplate::Scenario1_OmnidirectionalSound::RotationSpeedSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    // Update angular velocity based on specified orbit time in seconds.
    if (_initialized)
    {
        auto milliseconds = static_cast<float>(e->NewValue) * 1000;
        if (milliseconds > 0)
        {
            _angularVelocity = HRTF_2PI / milliseconds;
        }
        else
        {
            _angularVelocity = 0;
        }
    }
}

void SDKTemplate::Scenario1_OmnidirectionalSound::HeightSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _height = static_cast<float>(e->NewValue);
}
