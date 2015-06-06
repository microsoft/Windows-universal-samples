// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_Metrics.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Display animation metrics.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Metrics sealed
    {
    public:
        Scenario1_Metrics();
    private:
        MainPage^ rootPage;
        void DisplayMetrics(Windows::UI::Core::AnimationMetrics::AnimationEffect effect, Windows::UI::Core::AnimationMetrics::AnimationEffectTarget target);
        void Animations_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
    };
}
