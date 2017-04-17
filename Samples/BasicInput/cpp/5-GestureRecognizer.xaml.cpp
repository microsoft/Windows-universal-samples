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
#include "5-GestureRecognizer.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario5::Scenario5() : rootPage(SDKTemplate::MainPage::Current)
{
    InitializeComponent();

    InitOptions();

    // Create a GestureRecognizer which will be used to process the manipulations
    // done on the rectangle
    recognizer = ref new GestureRecognizer();

    // Create a ManipulationInputProcessor which will listen for events on the
    // rectangle, process them, and update the rectangle's position, size, and rotation
    manipulationProcessor = ref new ManipulationInputProcessor(recognizer, manipulateMe, mainCanvas);
}

void Scenario5::InitOptions()
{
    movementAxis->SelectedIndex = 0;
    InertiaSwitch->IsOn = true;
}

void Scenario5::movementAxis_Changed(Platform::Object^ sender, SelectionChangedEventArgs^ e)
{
    if (manipulationProcessor == nullptr)
    {
        return;
    }

    ComboBox^ cb = dynamic_cast<ComboBox^>(sender);
    ComboBoxItem^ selectedItem = dynamic_cast<ComboBoxItem^>((cb)->SelectedItem);

    auto selection = selectedItem->Content->ToString();

    if (selection == L"X only")
    {
        manipulationProcessor->LockToXAxis();
    }
    else if (selection == L"Y only")
    {
        manipulationProcessor->LockToXAxis();
    }
    else
    {
        manipulationProcessor->MoveOnXAndYAxes();
    }
}

void Scenario5::InertiaSwitch_Toggled(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (manipulationProcessor == nullptr)
    {
        return;
    }

    manipulationProcessor->UseInertia(InertiaSwitch->IsOn);
}

void Scenario5::resetButton_Pressed(Platform::Object^ sender, RoutedEventArgs^ e)
{
    InitOptions();
    manipulationProcessor->Reset();
}

ManipulationInputProcessor::ManipulationInputProcessor(GestureRecognizer^ gestureRecognizer, UIElement^ target, UIElement^ referenceFrame)
    : recognizer(gestureRecognizer)
    , element(target)
    , reference(referenceFrame)
{
    // Initialize the transforms that will be used to manipulate the shape
    InitializeTransforms();

    InitializeTransforms();

    // The GestureSettings property dictates what manipulation events the
    // Gesture Recognizer will listen to.  This will set it to a limited
    // subset of these events.
    recognizer->GestureSettings = GenerateDefaultSettings();

    // Set up pointer event handlers. These receive input events that are used by the gesture recognizer->
    element->PointerPressed += ref new PointerEventHandler(this, &ManipulationInputProcessor::OnPointerPressed);
    element->PointerMoved += ref new PointerEventHandler(this, &ManipulationInputProcessor::OnPointerMoved);
    element->PointerReleased += ref new PointerEventHandler(this, &ManipulationInputProcessor::OnPointerReleased);
    element->PointerCanceled += ref new PointerEventHandler(this, &ManipulationInputProcessor::OnPointerCanceled);

    // Set up event handlers to respond to gesture recognizer output
    recognizer->ManipulationStarted += ref new TypedEventHandler<GestureRecognizer^, ManipulationStartedEventArgs^>(this, &ManipulationInputProcessor::OnManipulationStarted);
    recognizer->ManipulationUpdated += ref new TypedEventHandler<GestureRecognizer^, ManipulationUpdatedEventArgs^>(this, &ManipulationInputProcessor::OnManipulationUpdated);
    recognizer->ManipulationCompleted += ref new TypedEventHandler<GestureRecognizer^, ManipulationCompletedEventArgs^>(this, &ManipulationInputProcessor::OnManipulationCompleted);
    recognizer->ManipulationInertiaStarting += ref new TypedEventHandler<GestureRecognizer^, ManipulationInertiaStartingEventArgs^>(this, &ManipulationInputProcessor::OnManipulationInertiaStarting);
}

void ManipulationInputProcessor::InitializeTransforms()
{
    cumulativeTransform = ref new TransformGroup();
    deltaTransform = ref new CompositeTransform();
    previousTransform = ref new MatrixTransform();
    previousTransform->Matrix = Matrix::Identity;

    cumulativeTransform->Children->Append(previousTransform);
    cumulativeTransform->Children->Append(deltaTransform);

    element->RenderTransform = cumulativeTransform;
}

// Return the default GestureSettings for this sample
GestureSettings ManipulationInputProcessor::GenerateDefaultSettings()
{
    return GestureSettings::ManipulationTranslateX |
        GestureSettings::ManipulationTranslateY |
        GestureSettings::ManipulationRotate |
        GestureSettings::ManipulationTranslateInertia |
        GestureSettings::ManipulationRotateInertia;
}

// Route the pointer pressed event to the gesture recognizer->
// The points are in the reference frame of the canvas that contains the rectangle element->
void ManipulationInputProcessor::OnPointerPressed(Platform::Object^ sender, PointerRoutedEventArgs^ args)
{
    // Set the pointer capture to the element being interacted with so that only it
    // will fire pointer-related events
    element->CapturePointer(args->Pointer);

    // Feed the current point into the gesture recognizer as a down event
    recognizer->ProcessDownEvent(args->GetCurrentPoint(reference));
}

// Route the pointer moved event to the gesture recognizer->
// The points are in the reference frame of the canvas that contains the rectangle element->
void ManipulationInputProcessor::OnPointerMoved(Platform::Object^ sender, PointerRoutedEventArgs^ args)
{
    // Feed the set of points into the gesture recognizer as a move event
    recognizer->ProcessMoveEvents(args->GetIntermediatePoints(reference));
}

// Route the pointer released event to the gesture recognizer->
// The points are in the reference frame of the canvas that contains the rectangle element->
void ManipulationInputProcessor::OnPointerReleased(Platform::Object^ sender, PointerRoutedEventArgs^ args)
{
    // Feed the current point into the gesture recognizer as an up event
    recognizer->ProcessUpEvent(args->GetCurrentPoint(reference));

    // Release the pointer
    element->ReleasePointerCapture(args->Pointer);
}

// Route the pointer canceled event to the gesture recognizer->
// The points are in the reference frame of the canvas that contains the rectangle element->
void ManipulationInputProcessor::OnPointerCanceled(Platform::Object^ sender, PointerRoutedEventArgs^ args)
{
    recognizer->CompleteGesture();
    element->ReleasePointerCapture(args->Pointer);
}

// When a manipulation begins, change the color of the Platform::Object^ to reflect
// that a manipulation is in progress
void ManipulationInputProcessor::OnManipulationStarted(GestureRecognizer^ sender, ManipulationStartedEventArgs^ e)
{
    Border^ b = dynamic_cast<Border^>(element);
    b->Background = ref new SolidColorBrush(Windows::UI::Colors::DeepSkyBlue);
}

// Process the change resulting from a manipulation
void ManipulationInputProcessor::OnManipulationUpdated(GestureRecognizer^ sender, ManipulationUpdatedEventArgs^ e)
{
    previousTransform->Matrix = cumulativeTransform->Value;

    // Get the center point of the manipulation for rotation
    Point center = Point(e->Position.X, e->Position.Y);
    deltaTransform->CenterX = center.X;
    deltaTransform->CenterY = center.Y;

    // Look at the Delta property of the ManipulationDeltaRoutedEventArgs to retrieve
    // the rotation, X, and Y changes
    deltaTransform->Rotation = e->Delta.Rotation;
    deltaTransform->TranslateX = e->Delta.Translation.X;
    deltaTransform->TranslateY = e->Delta.Translation.Y;
}

// When a manipulation that's a result of inertia begins, change the color of the
// the Platform::Object^ to reflect that inertia has taken over
void ManipulationInputProcessor::OnManipulationInertiaStarting(GestureRecognizer^ sender, ManipulationInertiaStartingEventArgs^ e)
{
    Border^ b = dynamic_cast<Border^>(element);
    b->Background = ref new SolidColorBrush(Windows::UI::Colors::RoyalBlue);
}

// When a manipulation has finished, reset the color of the Platform::Object^
void ManipulationInputProcessor::OnManipulationCompleted(GestureRecognizer^ sender, ManipulationCompletedEventArgs^ e)
{
    Border^ b = dynamic_cast<Border^>(element);
    b->Background = ref new SolidColorBrush(Windows::UI::Colors::LightGray);
}

// Modify the GestureSettings property to only allow movement on the X axis
void ManipulationInputProcessor::LockToXAxis()
{
    recognizer->CompleteGesture();
    auto mode = recognizer->GestureSettings;
    auto update = GestureSettings::ManipulationTranslateY | GestureSettings::ManipulationTranslateX;
    recognizer->GestureSettings = mode | update;
    mode = recognizer->GestureSettings;
    update = GestureSettings::ManipulationTranslateY;
    recognizer->GestureSettings = mode ^ update;
}

// Modify the GestureSettings property to only allow movement on the Y axis
void ManipulationInputProcessor::LockToYAxis()
{
    recognizer->CompleteGesture();
    auto mode = recognizer->GestureSettings;
    auto update = GestureSettings::ManipulationTranslateY | GestureSettings::ManipulationTranslateX;
    recognizer->GestureSettings = mode | update;
    mode = recognizer->GestureSettings;
    update = GestureSettings::ManipulationTranslateX;
    recognizer->GestureSettings = mode ^ update;
}

// Modify the GestureSettings property to allow movement on both the the X and Y axes
void ManipulationInputProcessor::MoveOnXAndYAxes()
{
    recognizer->CompleteGesture();
    auto mode = recognizer->GestureSettings;
    auto update = GestureSettings::ManipulationTranslateX | GestureSettings::ManipulationTranslateY;
    recognizer->GestureSettings = mode | update;
}

// Modify the GestureSettings property to enable or disable inertia based on the passed-in value
void ManipulationInputProcessor::UseInertia(bool inertia)
{
    if (!inertia)
    {
        recognizer->CompleteGesture();
        auto mode = recognizer->GestureSettings;
        auto update = GestureSettings::ManipulationTranslateInertia | GestureSettings::ManipulationRotateInertia;
        recognizer->GestureSettings = mode ^ update;
    }
    else
    {
        recognizer->CompleteGesture();
        auto mode = recognizer->GestureSettings;
        auto update = GestureSettings::ManipulationTranslateInertia | GestureSettings::ManipulationRotateInertia;
        recognizer->GestureSettings = mode | update;
    }
}

void ManipulationInputProcessor::Reset()
{
    element->RenderTransform = nullptr;
    recognizer->CompleteGesture();
    InitializeTransforms();
    recognizer->GestureSettings = GenerateDefaultSettings();
}
