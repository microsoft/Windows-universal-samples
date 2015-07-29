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

#include "Scenario1_LowLatencyInput.g.h"
#include "MainPage.xaml.h"

#include "Common\DeviceResources.h"
#include "LowLatencyInputMain.h"
#include "Common\Constants.h"

namespace SDKTemplate
{
    // <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ManipulationProcessor sealed
    {
    public:
        ManipulationProcessor(Windows::UI::Input::GestureRecognizer^ gestureRecognizer, Windows::UI::Xaml::Controls::Border^ target, Windows::UI::Xaml::UIElement^ referenceFrame);
        void InitializeTransforms();
        void ResetRect();

    private:
        Windows::UI::Input::GestureRecognizer^ xamlGestureRecognizer;
        Windows::UI::Xaml::Controls::Border^ element;
        Windows::UI::Xaml::UIElement^ reference;
        Windows::UI::Xaml::Media::TransformGroup^ cumulativeTransform;
        Windows::UI::Xaml::Media::MatrixTransform^ previousTransform;
        Windows::UI::Xaml::Media::CompositeTransform^ deltaTransform;

        void OnPointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
        void OnPointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
        void OnPointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
        void OnPointerCanceled(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
        void OnManipulationStarted(Windows::UI::Input::GestureRecognizer^ sender, Windows::UI::Input::ManipulationStartedEventArgs^ args);
        void OnManipulationUpdated(Windows::UI::Input::GestureRecognizer^ sender, Windows::UI::Input::ManipulationUpdatedEventArgs^ args);
        void OnManipulationInertiaStarting(Windows::UI::Input::GestureRecognizer^ sender, Windows::UI::Input::ManipulationInertiaStartingEventArgs^ args);
        void OnManipulationCompleted(Windows::UI::Input::GestureRecognizer^ sender, Windows::UI::Input::ManipulationCompletedEventArgs^ args);
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_LowLatencyInput sealed
    {
        public:
            Scenario1_LowLatencyInput();
            virtual ~Scenario1_LowLatencyInput();

        private:
            MainPage^ rootPage;
            // XAML low-level rendering event handler.
            void OnRendering(Platform::Object^ sender, Platform::Object^ args);

            // Window event handlers.
            void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
            void OnSizeChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^args);

            // DisplayInformation event handlers.
            void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
            void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
            void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

            // Other event handlers.
            void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel^ sender, Object^ args);
            void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ args);
            void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);

            // Track our independent input on a background worker thread.
            Windows::Foundation::IAsyncAction^ inputLoopWorker;
            Windows::UI::Core::CoreIndependentInputSource^ coreInput;

            // Independent input handling functions.
            void OnPointerPressed(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ args);
            void OnPointerMoved(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ args);
            void OnPointerReleased(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ args);
            void OnPointerExited(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ args);

            // Gesture recognizer event handlers
            void OnManipulationStarted(
                 Windows::UI::Input::GestureRecognizer^ sender,  Windows::UI::Input::ManipulationStartedEventArgs^ args);
            void OnManipulationUpdated(
                 Windows::UI::Input::GestureRecognizer^ sender,  Windows::UI::Input::ManipulationUpdatedEventArgs^ args);
            void OnManipulationInertiaStarting(
                 Windows::UI::Input::GestureRecognizer^ sender,  Windows::UI::Input::ManipulationInertiaStartingEventArgs^ args);
            void OnManipulationCompleted(
                 Windows::UI::Input::GestureRecognizer^ sender,  Windows::UI::Input::ManipulationCompletedEventArgs^ args);

            // Resources used to render the DirectX content in the XAML page background.
            std::shared_ptr<DX::DeviceResources> deviceResources;
            std::unique_ptr<SDKTemplate::LowLatencyInputMain> main;
            bool windowVisible;

            Windows::UI::Xaml::DispatcherTimer^ dispatcherTimer;

            //Gesture recognizers for the XAML rect and DirectX rendered rect
            Windows::UI::Input::GestureRecognizer^ directXGestureRecognizer;
            Windows::UI::Input::GestureRecognizer^ xamlGestureRecognizer;
            SDKTemplate::ManipulationProcessor^ manipulationProcessor;

            // UI button event handlers
            void ResetButton_Pressed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void BlockUIThread_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void DispatcherTimer_Tick(Platform::Object^ sender, Platform::Object^ e);
    };
}

