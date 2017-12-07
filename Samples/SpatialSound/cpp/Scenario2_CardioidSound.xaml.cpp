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
#include "Scenario2_CardioidSound.xaml.h"

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


Scenario2_CardioidSound::Scenario2_CardioidSound() : _rootPage(MainPage::Current)
{
    InitializeComponent();
    auto hr = _cardioidSound.Initialize(L"assets//MonoSound.wav");
    if (SUCCEEDED(hr))
    {
        _timer = ref new DispatcherTimer();
        _timerEventToken = _timer->Tick += ref new EventHandler<Platform::Object^>(this, &Scenario2_CardioidSound::OnTimerTick);
        TimeSpan timespan;
        timespan.Duration = 10000 / 30;
        _timer->Interval = timespan;
        EnvironmentComboBox->SelectedIndex = static_cast<int>(_cardioidSound.GetEnvironment());
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

Scenario2_CardioidSound::~Scenario2_CardioidSound()
{
    if (_timerEventToken.Value != 0)
    {
        _timer->Tick -= _timerEventToken;
    }
}

void SDKTemplate::Scenario2_CardioidSound::EnvironmentComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    if (_initialized)
    {
        _cardioidSound.SetEnvironment(static_cast<HrtfEnvironment>(EnvironmentComboBox->SelectedIndex));
    }
}

void SDKTemplate::Scenario2_CardioidSound::ScalingSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    UpdateScalingAndOrder();
}

void SDKTemplate::Scenario2_CardioidSound::OrderSlider_ValudChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    UpdateScalingAndOrder();
}

void SDKTemplate::Scenario2_CardioidSound::YawSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _yaw = static_cast<float>(e->NewValue);
}

void SDKTemplate::Scenario2_CardioidSound::PitchSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _pitch = static_cast<float>(e->NewValue);
}


void SDKTemplate::Scenario2_CardioidSound::RollSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _roll = static_cast<float>(e->NewValue);
}

void SDKTemplate::Scenario2_CardioidSound::PlayButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_initialized)
    {
        _cardioidSound.Start();
        _state = PlayState::Playing;
        _rootPage->NotifyUser("Playing", NotifyType::StatusMessage);
    }
}

void SDKTemplate::Scenario2_CardioidSound::StopButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_initialized)
    {
        _cardioidSound.Stop();
        _state = PlayState::Stopped;
        _rootPage->NotifyUser("Stopped", NotifyType::StatusMessage);
    }
}

void SDKTemplate::Scenario2_CardioidSound::OnTimerTick(Object^ sender, Object^ e)
{
    // Update the sound position and orientation on every dispatcher timer tick.
    _cardioidSound.OnUpdate(_x, _y, _z, _pitch, _yaw, _roll);
}

void SDKTemplate::Scenario2_CardioidSound::UpdateScalingAndOrder()
{
    if (_initialized)
    {
        _timer->Stop();
        _cardioidSound.ConfigureApo(static_cast<float>(ScalingSlider->Value), static_cast<float>(OrderSlider->Value));
        _timer->Start();

        if (_state == PlayState::Playing)
        {
            _cardioidSound.Start();
        }
    }
}

void SDKTemplate::Scenario2_CardioidSound::SourcePositionX_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _x = static_cast<float>(e->NewValue);
}

void SDKTemplate::Scenario2_CardioidSound::SourcePositionY_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _y = static_cast<float>(e->NewValue);
}

void SDKTemplate::Scenario2_CardioidSound::SourcePositionZ_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _z = static_cast<float>(e->NewValue);
}
