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


#pragma once

#include "Scenario1_OmnidirectionalSound.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_OmnidirectionalSound sealed
    {
    public:
        Scenario1_OmnidirectionalSound();
        virtual ~Scenario1_OmnidirectionalSound();
    private:
        void StartButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StopButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnTimerTick(Object^ sender, Object^ e);
        void RadiusSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void EnvironmentComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void PlayButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AngularVelocitySlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void RotationSpeedSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void HeightSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);

    private:
        MainPage^                       _rootPage;
        bool                            _initialized = false;
        OmnidirectionalSound            _omnidirectionalSound;
        DispatcherTimer^                _timer;
        EventRegistrationToken          _timerEventToken;
        float                           _radius = 2.0f;            // Radius of the orbit
        float                           _height = 0;               // Height at which the sound is orbiting (0 for centered around listener's head, +ve for above and -ve for below)
        float                           _angularVelocity = 0;      // Speed of orbit, default is stationary
    };
}
