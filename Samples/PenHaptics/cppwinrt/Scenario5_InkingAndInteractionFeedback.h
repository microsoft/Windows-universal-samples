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

#include "Scenario5_InkingAndInteractionFeedback.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_InkingAndInteractionFeedback : Scenario5_InkingAndInteractionFeedbackT<Scenario5_InkingAndInteractionFeedback>
    {
        Scenario5_InkingAndInteractionFeedback();

        void MovableRect_Entered(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void MovableRect_Exited(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void MovableRect_ManipulationStarted(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::ManipulationStartedRoutedEventArgs const&);
        void MovableRect_ManipulationDelta(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs const& e);
        void MovableRect_ManipulationCompleted(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs const&);

    private:
        const int gridSize = 100;
        const double snapDistance = 20.0;
        Windows::Foundation::Point topLeft;
        Windows::Foundation::Point previewTopLeft{ -1, -1 };
        Windows::UI::Xaml::Media::TransformGroup transforms{ nullptr };
        Windows::UI::Xaml::Media::MatrixTransform previousTransform{ nullptr };
        Windows::UI::Xaml::Media::CompositeTransform deltaTransform{ nullptr };
        Windows::Devices::Input::PenDevice penDevice{ nullptr };
        Windows::Devices::Haptics::SimpleHapticsController hapticsController{ nullptr };

        void InitializeGridLines();
        void InitializeManipulationTransforms();
        void UpdatePreviewRect();
        void SnapMovableRectToGridLines();
        Windows::Foundation::Point GetSnappedPoint();
        bool IsInSnapRange();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_InkingAndInteractionFeedback : Scenario5_InkingAndInteractionFeedbackT<Scenario5_InkingAndInteractionFeedback, implementation::Scenario5_InkingAndInteractionFeedback>
    {
    };
}
