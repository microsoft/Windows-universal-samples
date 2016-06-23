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
#include "Scenario3_CustomDecay.xaml.h"

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

Scenario3_CustomDecay::Scenario3_CustomDecay() : _rootPage(MainPage::Current)
{
    InitializeComponent();
    auto hr = _customDecaySound.Initialize(L"assets//MonoSound.wav");
    if (SUCCEEDED(hr))
    {
        _timer = ref new DispatcherTimer();
        _timerEventToken = _timer->Tick += ref new EventHandler<Platform::Object^>(this, &Scenario3_CustomDecay::OnTimerTick);
        TimeSpan timespan;
        timespan.Duration = 10000 / 30;
        _timer->Interval = timespan;
        EnvironmentComboBox->SelectedIndex = static_cast<int>(_customDecaySound.GetEnvironment());
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

Scenario3_CustomDecay::~Scenario3_CustomDecay()
{
    if (_timerEventToken.Value != 0)
    {
        _timer->Tick -= _timerEventToken;
    }
}

void SDKTemplate::Scenario3_CustomDecay::EnvironmentComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    if (_initialized)
    {
        _customDecaySound.SetEnvironment(static_cast<HrtfEnvironment>(EnvironmentComboBox->SelectedIndex));
    }
}

void SDKTemplate::Scenario3_CustomDecay::SourcePositionX_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _x = static_cast<float>(e->NewValue);
}

void SDKTemplate::Scenario3_CustomDecay::SourcePositionY_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _y = static_cast<float>(e->NewValue);
}

void SDKTemplate::Scenario3_CustomDecay::SourcePositionZ_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    _z = static_cast<float>(e->NewValue);
}

void SDKTemplate::Scenario3_CustomDecay::OnTimerTick(Object^ sender, Object^ e)
{
    // Update the source position to update the gain applied.
    _customDecaySound.OnUpdate(_x, _y, _z);
}

void SDKTemplate::Scenario3_CustomDecay::PlayButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_initialized)
    {
        _customDecaySound.Start();
        _timer->Start();
        _rootPage->NotifyUser("Playing", NotifyType::StatusMessage);
    }
}

void SDKTemplate::Scenario3_CustomDecay::StopButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_initialized)
    {
        _customDecaySound.Stop();
        _timer->Stop();
        _rootPage->NotifyUser("Stopped", NotifyType::StatusMessage);
    }
}
