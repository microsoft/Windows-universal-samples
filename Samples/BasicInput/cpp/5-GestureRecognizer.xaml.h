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

#include "5-GestureRecognizer.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    ref class ManipulationInputProcessor sealed
    {
    public:
        ManipulationInputProcessor(Windows::UI::Input::GestureRecognizer^ gestureRecognizer, Windows::UI::Xaml::UIElement^ target, Windows::UI::Xaml::UIElement^ referenceFrame);
        void LockToXAxis();
        void LockToYAxis();
        void MoveOnXAndYAxes();
        void UseInertia(bool inertia);
        void Reset();

    private:
        void InitializeTransforms();
        Windows::UI::Input::GestureSettings GenerateDefaultSettings();
        void OnPointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
        void OnPointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
        void OnPointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
        void OnPointerCanceled(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
        void OnManipulationStarted(Windows::UI::Input::GestureRecognizer^, Windows::UI::Input::ManipulationStartedEventArgs^ e);
        void OnManipulationUpdated(Windows::UI::Input::GestureRecognizer^, Windows::UI::Input::ManipulationUpdatedEventArgs^ e);
        void OnManipulationInertiaStarting(Windows::UI::Input::GestureRecognizer^r, Windows::UI::Input::ManipulationInertiaStartingEventArgs^ e);
        void OnManipulationCompleted(Windows::UI::Input::GestureRecognizer^, Windows::UI::Input::ManipulationCompletedEventArgs^ e);

        Windows::UI::Input::GestureRecognizer^ recognizer;
        Windows::UI::Xaml::UIElement^ element;
        Windows::UI::Xaml::UIElement^ reference;
        Windows::UI::Xaml::Media::TransformGroup^ cumulativeTransform;
        Windows::UI::Xaml::Media::MatrixTransform^ previousTransform;
        Windows::UI::Xaml::Media::CompositeTransform^ deltaTransform;
    };

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario5 sealed
    {
    public:
        Scenario5();

    private:
        void InitOptions();
        void movementAxis_Changed(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void InertiaSwitch_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void resetButton_Pressed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        MainPage^ rootPage = MainPage::Current;
        Windows::UI::Input::GestureRecognizer^ recognizer;
        ManipulationInputProcessor^ manipulationProcessor;
    };
}
