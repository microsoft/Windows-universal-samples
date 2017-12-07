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

#include "Scenario1_CustomDry.g.h"
#include "MainPage.xaml.h"
#include "Renderers\SceneComposer.h"
#include "Controls\ContextMenuFlyout.xaml.h"
#include "Common\InkEventLogger.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_CustomDry sealed
    {
    public:
        Scenario1_CustomDry();
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:

        enum PointerHitTestStates
        {
            None,
            InsideSelectionRect,
            InsideInkingArea,
        };

        enum InkingAreaSubStates
        {
            AreaNone,
            Selecting,
            SelectingOrPasting,
            Erasing
        };

        enum InputModes
        {
            InkingMode,
            SelectingMode,
            ErasingMode,
            InkToolbarMode
        };

    private:

        SDKTemplate::MainPage^ rootPage;
        std::shared_ptr<SceneComposer> _sceneComposer;
        Windows::UI::Xaml::Media::Imaging::VirtualSurfaceImageSource^ _imageSource;

        // Inking members
        Windows::UI::Input::Inking::Core::CoreInkIndependentInputSource^ _independentInput;
        // Using this object to store the configuration from InkPresenter and use it when 
        // changing modes in the Independent Input Handler
        Windows::UI::Input::Inking::InkInputProcessingConfiguration^ _config;

        // Vector of points to build the stroke that will enclose the selected strokes
        Windows::Foundation::Collections::IVector<Windows::Foundation::Point>^ _selectionPoints;

        // Inking State Management
        InkingAreaSubStates _currentHitTestSubState;
        PointerHitTestStates _currentHitTestState;
        InputModes   _inputMode;

        // Context Menu members
        ContextMenuFlyout^ _contextMenuFlyout;

        int _pointerId;

        // Lasso selection members
        Windows::UI::Input::PointerPoint^ _initialPointerPoint;

        // Erasing 
        Windows::Foundation::Point _prevErasingPoint;

        // Gesture Recognizer to get gestures for cut/copy/paste scenarios
        Windows::UI::Input::GestureRecognizer^ _gestureRecognizer;

        // Logging of input messages
        InkEventLogger* _eventLogger;
        bool _isLoggingEnabled;

        // concurrency lock to have thread-safe data access across multiple threads
        Concurrency::critical_section _criticalSection;

        // Event Handler Tokens
        Windows::Foundation::EventRegistrationToken _strokesCollectedToken;

        Windows::Foundation::EventRegistrationToken _independentInputMovingToken;
        Windows::Foundation::EventRegistrationToken _independentInputPressingToken;
        Windows::Foundation::EventRegistrationToken _independentInputReleasingToken;
        Windows::Foundation::EventRegistrationToken _independentInputExitingToken;
        Windows::Foundation::EventRegistrationToken _independentInputEnteringToken;
        Windows::Foundation::EventRegistrationToken _independentInputHoveringToken;
        Windows::Foundation::EventRegistrationToken _independentInputLostToken;

        Windows::Foundation::EventRegistrationToken _unprocessedInputMovedToken;
        Windows::Foundation::EventRegistrationToken _unprocessedInputPressedToken;
        Windows::Foundation::EventRegistrationToken _unprocessedInputReleasedToken;
        Windows::Foundation::EventRegistrationToken _unprocessedInputExitedToken;
        Windows::Foundation::EventRegistrationToken _unprocessedInputEnteredToken;
        Windows::Foundation::EventRegistrationToken _unprocessedInputHoveredToken;
        Windows::Foundation::EventRegistrationToken _unprocessedInputLostToken;

        Windows::Foundation::EventRegistrationToken _strokeStartedToken;
        Windows::Foundation::EventRegistrationToken _stokeContinuedToken;
        Windows::Foundation::EventRegistrationToken _strokeEndedToken;
        Windows::Foundation::EventRegistrationToken _strokeCanceledToken;

    private:

        // Page and controls event handlers
        void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
        void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^);
        void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
        void OnClear(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnInsertShape(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void TouchInking_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void TouchInking_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InkingModeSelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

        // Inking input handlers
        void OnStrokesCollected(Windows::UI::Input::Inking::InkPresenter^ sender, Windows::UI::Input::Inking::InkStrokesCollectedEventArgs^ args);

        // Independent input handlers
        void IndependentInputOnPointerMoving(Windows::UI::Input::Inking::Core::CoreInkIndependentInputSource^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void IndependentInputOnPointerPressing(Windows::UI::Input::Inking::Core::CoreInkIndependentInputSource^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void IndependentInputOnPointerReleasing(Windows::UI::Input::Inking::Core::CoreInkIndependentInputSource^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void IndependentInputOnPointerExiting(Windows::UI::Input::Inking::Core::CoreInkIndependentInputSource^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void IndependentInputOnPointerEntering(Windows::UI::Input::Inking::Core::CoreInkIndependentInputSource^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void IndependentInputOnPointerHovering(Windows::UI::Input::Inking::Core::CoreInkIndependentInputSource^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void IndependentInputOnPointerLost(Windows::UI::Input::Inking::Core::CoreInkIndependentInputSource^ sender, Windows::UI::Core::PointerEventArgs^ args);

        // Unprocessed input handlers
        void UnprocessedInputOnPointerEntered(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void UnprocessedInputOnPointerHovered(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void UnprocessedInputOnPointerPressed(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void UnprocessedInputOnPointerMoved(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void UnprocessedInputOnPointerReleased(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void UnprocessedInputOnPointerExited(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void UnprocessedInputOnPointerLost(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);

        // InkStroke handlers
        void OnStrokeStarted(Windows::UI::Input::Inking::InkStrokeInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void OnStrokeContinued(Windows::UI::Input::Inking::InkStrokeInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void OnStrokeEnded(Windows::UI::Input::Inking::InkStrokeInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void OnStrokeCanceled(Windows::UI::Input::Inking::InkStrokeInput^ sender, Windows::UI::Core::PointerEventArgs^ args);

        // Gesture Recognizer event handlers
        void OnRightTapped(Windows::UI::Input::GestureRecognizer^ sender, Windows::UI::Input::RightTappedEventArgs^ args);
        void OnHolding(Windows::UI::Input::GestureRecognizer^ sender, Windows::UI::Input::HoldingEventArgs^ args);

        // Logging event handlers
        void InkingEvent_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void IndependentEvent_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UnprocessedEvent_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StrokeEvent_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        // View event handlers
        void OnViewChanged(Platform::Object^, Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs^ e);

        // Helper methods
        void EnableInkingEventHandlers();
        void EnableIndependentInputEventHandlers();
        void EnableLoggingIndependentInputEventHandlers(bool enable);
        void EnableUnprocessedInputEventHandlers();
        void EnableLoggingUnprocessedInputEventHandlers(bool enable);
        void EnableLoggingStrokeEventHandlers(bool enable);

        InkingAreaSubStates GetInkingSubstate(Windows::UI::Core::PointerEventArgs^ args);
        PointerHitTestStates GetHitTestState(Windows::Foundation::Point position);
        void StartStrokeSelection(Windows::Foundation::Point position);
        void LaunchContextMenu(Windows::Foundation::Point contextMenuPosition);

        void Log(LogEventAction action);
        void UpdateMessageLog(LogEventAction action);
        void LogToggleSwitch_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}