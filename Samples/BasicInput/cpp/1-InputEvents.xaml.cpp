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
#include "1-InputEvents.xaml.h"

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

Scenario1::Scenario1()
{
    InitializeComponent();

    // pointer press/release handlers
    pressedTarget->PointerPressed += ref new PointerEventHandler(this, &Scenario1::target_PointerPressed);
    pressedTarget->PointerReleased += ref new PointerEventHandler(this, &Scenario1::target_PointerReleased);

    // pointer enter/exit handlers
    enterExitTarget->PointerEntered += ref new PointerEventHandler(this, &Scenario1::target_PointerEntered);
    enterExitTarget->PointerExited += ref new PointerEventHandler(this, &Scenario1::target_PointerExited);

    // gesture handlers
    tapTarget->Tapped += ref new TappedEventHandler(this, &Scenario1::target_Tapped);
    tapTarget->DoubleTapped += ref new DoubleTappedEventHandler(this, &Scenario1::target_DoubleTapped);

    holdTarget->Holding += ref new HoldingEventHandler(this, &Scenario1::target_Holding);
    holdTarget->RightTapped += ref new RightTappedEventHandler(this, &Scenario1::target_RightTapped);
}

// A PointerPressed event is sent whenever a mouse button, finger, or pen is pressed to make
// contact with an object
void Scenario1::target_PointerPressed(Object^ sender, PointerRoutedEventArgs^ e)
{
    pressedTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::RoyalBlue);
    pressedTargetText->Text = "Pointer Pressed";
}

// A PointerReleased event is sent whenever a mouse button, finger, or pen is released to remove
// contact from an object
void Scenario1::target_PointerReleased(Object^ sender, PointerRoutedEventArgs^ e)
{
    pressedTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::LightGray);
    pressedTargetText->Text = "Pointer Released";
}

// A PointerEntered event is sent whenever a mouse cursor is moved on top of an object
// or when a pen or finger is dragged on top of an object
void Scenario1::target_PointerEntered(Object^ sender, PointerRoutedEventArgs^ e)
{
    enterExitTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::RoyalBlue);
    enterExitTargetText->Text = "Pointer Entered";
}

// A PointerExited event is sent whenever a mouse cursor is moved off of an object
// or when a pen or finger is dragged off of an object
void Scenario1::target_PointerExited(Object^ sender, PointerRoutedEventArgs^ e)
{
    enterExitTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::LightGray);
    enterExitTargetText->Text = "Pointer Exited";
}

// A Tapped event is sent whenever a mouse is clicked or a finger or pen taps
// the object
void Scenario1::target_Tapped(Object^ sender, TappedRoutedEventArgs^ e)
{
    tapTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::DeepSkyBlue);
    tapTargetText->Text = "Tapped";
}

// A DoubleTapped event is sent whenever a mouse is double-clicked or a finger or pen taps
// the object twice in quick succession
void Scenario1::target_DoubleTapped(Object^ sender, DoubleTappedRoutedEventArgs^ e)
{
    tapTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::RoyalBlue);
    tapTargetText->Text = "Double-Tapped";
}

// A RightTapped event is sent whenever a mouse is right-clicked or a finger or pen
// completes a Holding event.  This is intended to be used to handle secondary actions
// on an object.
void Scenario1::target_RightTapped(Object^ sender, RightTappedRoutedEventArgs^ e)
{
    holdTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::RoyalBlue);
    holdTargetText->Text = "Right Tapped";
}

// A Holding event is sent whenever a finger or pen is pressed and held on top of
// an object.
// Once a small amount of time has elapsed, the event is sent with a HoldingState
// of the type HoldingState.Started, indicating that the held threshold has just
// been passed.
// When a finger has been lifted after a successful hold, a Holding event is sent
// with a HoldingState of Completed.
// If the user cancels the hold after it has been started, but before it completes,
// a Holding event is sent with a HoldingState of Canceled.
void Scenario1::target_Holding(Object^ sender, HoldingRoutedEventArgs^ e)
{
    if (e->HoldingState == Windows::UI::Input::HoldingState::Started)
    {
        holdTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::DeepSkyBlue);
        holdTargetText->Text = "Holding";
    }
    else if (e->HoldingState == Windows::UI::Input::HoldingState::Completed)
    {
        holdTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::LightGray);
        holdTargetText->Text = "Held";
    }
    else
    {
        holdTarget->Background = ref new SolidColorBrush(Windows::UI::Colors::LightGray);
        holdTargetText->Text = "Hold Canceled";
    }
}

