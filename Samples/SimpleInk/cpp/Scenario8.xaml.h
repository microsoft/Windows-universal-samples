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

#include "Scenario8.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario8 sealed
    {
    public:
        Scenario8();
    private:

        void InkStackPanel_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs e);
        void CoreWetStrokeUpdateSource_WetStrokeStarting(Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateSource^ sender, Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateEventArgs^ args);
        void CoreWetStrokeUpdateSource_WetStrokeContinuing(Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateSource^ sender, Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateEventArgs^ args);
        void CoreWetStrokeUpdateSource_WetStrokeStopping(Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateSource^ sender, Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateEventArgs^ args);
        void CoreWetStrokeUpdateSource_WetStrokeCompleted(Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateSource^ sender, Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateEventArgs^ args);
        void CoreWetStrokeUpdateSource_WetStrokeCanceled(Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateSource^ sender, Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateEventArgs^ args);
        bool FollowCircleTest(Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateEventArgs^ args);
        void HandleFollowCircleInput(Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateEventArgs^ args);
        void HandleRegularInput(Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateEventArgs^ args);
        double DistanceFromCenter(Windows::Foundation::Point position);
        bool InCircleEdgeZone(double distanceFromCenter);

        MainPage^ rootPage = MainPage::Current;

        static const float radius;
        static const float circleStrokeThickness;
        static const float penSize;

        Windows::UI::Input::Inking::Core::CoreWetStrokeUpdateSource^ coreWetStrokeUpdateSource;

        Windows::Foundation::Point circleCenter;

        // Follow Circle is true if Ink input should be redirected to the circle edge
        bool followCircle = false;

        // discontinueStroke is true is the Ink input that follows for the wet stroke is to be ignored
        bool discontinueStroke = false;

        // disableFollowCircle is set to true to disable Follow Circle behavior for the current stroke
        bool disableFollowCircle = false;
    };
}
