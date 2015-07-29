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
#include "Scenario1_LowLatencyInput.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

Scenario1_LowLatencyInput::Scenario1_LowLatencyInput() : 
    rootPage(MainPage::Current),
    windowVisible(true),
    coreInput(nullptr)
{
    InitializeComponent();

    // Register event handlers for page lifecycle.
    CoreWindow^ window = Window::Current->CoreWindow;

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &Scenario1_LowLatencyInput::OnVisibilityChanged);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Scenario1_LowLatencyInput::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Scenario1_LowLatencyInput::OnOrientationChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Scenario1_LowLatencyInput::OnDisplayContentsInvalidated);

    swapChainPanel->CompositionScaleChanged +=
        ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &Scenario1_LowLatencyInput::OnCompositionScaleChanged);

    swapChainPanel->SizeChanged +=
        ref new SizeChangedEventHandler(this, &Scenario1_LowLatencyInput::OnSwapChainPanelSizeChanged);

    Application::Current->Suspending +=
        ref new SuspendingEventHandler(this, &Scenario1_LowLatencyInput::OnSuspending);

    // Disable all pointer visual feedback for better performance when touching.
    auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    pointerVisualizationSettings->IsContactFeedbackEnabled = false;
    pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;

    // At this point we have access to the device and can create the device-dependent resources
    // and set the swap chain panel to the XAML SwapChainPanel element
    deviceResources = std::make_shared<DX::DeviceResources>();
    deviceResources->SetSwapChainPanel(swapChainPanel);

    // Register our SwapChainPanel to get input pointer events on a dedicated input thread and off the UI thread
    auto inputWorkItemHandler = ref new WorkItemHandler([this](IAsyncAction ^)
    {
        // The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
        coreInput = swapChainPanel->CreateCoreIndependentInputSource(
            CoreInputDeviceTypes::Mouse |
            CoreInputDeviceTypes::Touch |
            CoreInputDeviceTypes::Pen
            );

        // Register for pointer events, which will be raised on the dedicated input thread.
        coreInput->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Scenario1_LowLatencyInput::OnPointerPressed);
        coreInput->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Scenario1_LowLatencyInput::OnPointerMoved);
        coreInput->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Scenario1_LowLatencyInput::OnPointerReleased);
        coreInput->PointerExited += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Scenario1_LowLatencyInput::OnPointerExited);

        //Create a gesture recognizer to handle manipulation events and configure it for translation and rotation
        directXGestureRecognizer = ref new Windows::UI::Input::GestureRecognizer();

        directXGestureRecognizer->GestureSettings =
            Windows::UI::Input::GestureSettings::ManipulationTranslateX |
            Windows::UI::Input::GestureSettings::ManipulationTranslateY |
            Windows::UI::Input::GestureSettings::ManipulationRotate |
            Windows::UI::Input::GestureSettings::ManipulationTranslateInertia |
            Windows::UI::Input::GestureSettings::ManipulationRotateInertia;

        directXGestureRecognizer->ManipulationStarted += 
            ref new TypedEventHandler<GestureRecognizer^, ManipulationStartedEventArgs^>(this, &Scenario1_LowLatencyInput::OnManipulationStarted);
        directXGestureRecognizer->ManipulationUpdated +=
            ref new TypedEventHandler<GestureRecognizer^, ManipulationUpdatedEventArgs^>(this, &Scenario1_LowLatencyInput::OnManipulationUpdated);
        directXGestureRecognizer->ManipulationInertiaStarting +=
            ref new TypedEventHandler<GestureRecognizer^, ManipulationInertiaStartingEventArgs^>(this, &Scenario1_LowLatencyInput::OnManipulationInertiaStarting);
        directXGestureRecognizer->ManipulationCompleted +=
            ref new TypedEventHandler<GestureRecognizer^, ManipulationCompletedEventArgs^>(this, &Scenario1_LowLatencyInput::OnManipulationCompleted);

        // Begin constantly processing input messages as they're delivered.
        coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
    });

    // Run input task on a dedicated high priority background thread.
    inputLoopWorker = ThreadPool::RunAsync(inputWorkItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);

    // Create a GestureRecognizer which will be used to process the manipulations on the XAML Border element
    xamlGestureRecognizer = ref new GestureRecognizer();

    // Create a ManipulationProcessor which will listen for events on the XAML Border element,
    // process them, and update the Border's position, size, and rotation
    manipulationProcessor = ref new ManipulationProcessor(xamlGestureRecognizer, manipulateMe, mainCanvas);

    //Set up a dispatchTimer to be used to artificially block the UI thread
    dispatcherTimer = ref new DispatcherTimer;
    dispatcherTimer->Tick += ref new Windows::Foundation::EventHandler<Object^>(this, &Scenario1_LowLatencyInput::DispatcherTimer_Tick);
    TimeSpan time;
    time.Duration = 1000;
    dispatcherTimer->Interval = time;

    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow ^, WindowSizeChangedEventArgs ^>(this, &Scenario1_LowLatencyInput::OnSizeChanged);

    //Check initial width of app window, and set the rect to the approriate size
    main = std::unique_ptr<LowLatencyInputMain>(new LowLatencyInputMain(deviceResources));

    if (window->Bounds.Width < minScreenWidth)
    {
        main->SetRectSize(smallRectSize);
    }
    else
    {
        main->SetRectSize(bigRectSize);
    }

    main->StartRenderLoop();
}

Scenario1_LowLatencyInput::~Scenario1_LowLatencyInput()
{
    // Stop rendering and processing events on destruction.
    main->StopRenderLoop();
    coreInput->Dispatcher->StopProcessEvents();
}

void Scenario1_LowLatencyInput::DispatcherTimer_Tick(Platform::Object^ sender, Platform::Object^ e)
{
    // Artificially block UI thread for 100ms
    Sleep(100);
}

void Scenario1_LowLatencyInput::OnSizeChanged(CoreWindow ^sender, WindowSizeChangedEventArgs ^args)
{
    //Update rect size based on window width. If it's below 720, reduce size of rect to 125px instead of 200px
    //Make consts and move into funtion to call 
    if (args->Size.Width < minScreenWidth)
        main->SetRectSize(smallRectSize);
    else
        main->SetRectSize(bigRectSize);
}

void Scenario1_LowLatencyInput::OnPointerPressed(Object^ sender, PointerEventArgs^ args)
{
    //1. Get the current point
    PointerPoint^ currentPoint = args->CurrentPoint;

    //2. Hit test the rect
    if (main->HitTest(currentPoint->Position))
    {
        //3. Process the current point in gesture recognizer
        directXGestureRecognizer->ProcessDownEvent(currentPoint);
    }
}

void Scenario1_LowLatencyInput::OnPointerMoved(Object^ sender, PointerEventArgs^ args)
{
    // Get the intermediate points and process them in your gestureRecognizer
    IVector<PointerPoint^>^ pointerPoints = PointerPoint::GetIntermediatePoints(args->CurrentPoint->PointerId);
    
    directXGestureRecognizer->ProcessMoveEvents(pointerPoints);
}

void Scenario1_LowLatencyInput::OnPointerReleased(Object^ sender, PointerEventArgs^ args)
{
    // Get the current point and process it in your gestureRecognizer
    PointerPoint^ pointerPoint = args->CurrentPoint;
    directXGestureRecognizer->ProcessUpEvent(pointerPoint);
}

void Scenario1_LowLatencyInput::OnPointerExited(Object^ sender, PointerEventArgs^ args)
{
    // Get the current point and process it in your gestureRecognizer
    PointerPoint^ pointerPoint = args->CurrentPoint;
    directXGestureRecognizer->ProcessUpEvent(pointerPoint);
}

void Scenario1_LowLatencyInput::OnManipulationStarted(GestureRecognizer^ sender, ManipulationStartedEventArgs^ args)
{
    //Handle logic you want to occur when the manipulation starts here
    main->SetBrushColor(D2D1::ColorF::Orange);
}

void Scenario1_LowLatencyInput::OnManipulationUpdated(GestureRecognizer^ sender,  ManipulationUpdatedEventArgs^ args)
{
    Point position = args->Position;
    Windows::UI::Input::ManipulationDelta delta = args->Delta;
    main->UpdateRectTransform(position, delta);
}

void Scenario1_LowLatencyInput::OnManipulationInertiaStarting(GestureRecognizer^,  ManipulationInertiaStartingEventArgs^)
{
    //Handle logic you want to occur when the inertia due to manipulation starts here 
    main->SetBrushColor(D2D1::ColorF::OrangeRed);
}

void Scenario1_LowLatencyInput::OnManipulationCompleted(GestureRecognizer^,  ManipulationCompletedEventArgs^)
{
    //Handle logic you want to occur when the manipulation completes here
    main->SetBrushColor(D2D1::ColorF::LightSlateGray);
}

ManipulationProcessor::ManipulationProcessor(Windows::UI::Input::GestureRecognizer^ gestureRecognizer, Windows::UI::Xaml::Controls::Border^ target, Windows::UI::Xaml::UIElement^ referenceFrame)
{
    xamlGestureRecognizer = gestureRecognizer;
    element = target;
    reference = referenceFrame;

    // Initialize the transforms that will be used to manipulate the shape
    InitializeTransforms();

    // The GestureSettings property dictates what manipulation events the
    // Gesture Recognizer will listen to.  This will set it to a limited
    // subset of these events.
    xamlGestureRecognizer->GestureSettings = Windows::UI::Input::GestureSettings::ManipulationTranslateX |
        Windows::UI::Input::GestureSettings::ManipulationTranslateY |
        Windows::UI::Input::GestureSettings::ManipulationRotate |
        Windows::UI::Input::GestureSettings::ManipulationTranslateInertia |
        Windows::UI::Input::GestureSettings::ManipulationRotateInertia;

    // Set up pointer event handlers.  These receive input events that are used by the gesture recognizer.
    element->PointerPressed += ref new PointerEventHandler(this, &ManipulationProcessor::OnPointerPressed);
    element->PointerMoved += ref new PointerEventHandler(this, &ManipulationProcessor::OnPointerMoved);
    element->PointerReleased += ref new PointerEventHandler(this, &ManipulationProcessor::OnPointerReleased);
    element->PointerCanceled += ref new PointerEventHandler(this, &ManipulationProcessor::OnPointerCanceled);

    // Set up event handlers to respond to gesture recognizer output
    xamlGestureRecognizer->ManipulationStarted +=
        ref new TypedEventHandler<GestureRecognizer^, ManipulationStartedEventArgs^>(this, &ManipulationProcessor::OnManipulationStarted);
    xamlGestureRecognizer->ManipulationUpdated +=
        ref new TypedEventHandler<GestureRecognizer^, ManipulationUpdatedEventArgs^>(this, &ManipulationProcessor::OnManipulationUpdated);
    xamlGestureRecognizer->ManipulationInertiaStarting +=
        ref new TypedEventHandler<GestureRecognizer^, ManipulationInertiaStartingEventArgs^>(this, &ManipulationProcessor::OnManipulationInertiaStarting);
    xamlGestureRecognizer->ManipulationCompleted +=
        ref new TypedEventHandler<GestureRecognizer^, ManipulationCompletedEventArgs^>(this, &ManipulationProcessor::OnManipulationCompleted);
}

// Route the pointer pressed event to the gesture recognizer.
// The points are in the reference frame of the canvas that contains the rectangle element.
void ManipulationProcessor::OnPointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args)
{
    // Set the pointer capture to the element being interacted with so that only it
    // will fire pointer-related events
    element->CapturePointer(args->Pointer);

    // Feed the current point into the gesture recognizer as a down event
    xamlGestureRecognizer->ProcessDownEvent(args->GetCurrentPoint(reference));
}

// Route the pointer moved event to the gesture recognizer.
// The points are in the reference frame of the canvas that contains the rectangle element.
void ManipulationProcessor::OnPointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args)
{
    // Feed the set of points into the gesture recognizer as a move event
    xamlGestureRecognizer->ProcessMoveEvents(args->GetIntermediatePoints(reference));
}

// Route the pointer released event to the gesture recognizer.
// The points are in the reference frame of the canvas that contains the rectangle element.
void ManipulationProcessor::OnPointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args)
{
    // Feed the current point into the gesture recognizer as an up event
    xamlGestureRecognizer->ProcessUpEvent(args->GetCurrentPoint(reference));

    // Release the pointer
    element->ReleasePointerCapture(args->Pointer);
}

// Route the pointer canceled event to the gesture recognizer.
// The points are in the reference frame of the canvas that contains the rectangle element.
void ManipulationProcessor::OnPointerCanceled(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args)
{
    xamlGestureRecognizer->CompleteGesture();
    element->ReleasePointerCapture(args->Pointer);
}

// When a manipuliation begins, change the color of the object to reflect
// that a manipulation is in progress
void ManipulationProcessor::OnManipulationStarted(GestureRecognizer^ sender, Windows::UI::Input::ManipulationStartedEventArgs^ args)
{
    element->Background = ref new SolidColorBrush(Windows::UI::Colors::DeepSkyBlue);
}

// Process the change resulting from a manipulation
void ManipulationProcessor::OnManipulationUpdated(GestureRecognizer^ sender, Windows::UI::Input::ManipulationUpdatedEventArgs^ args)
{
    previousTransform->Matrix = cumulativeTransform->Value;

    // Get the center point of the manipulation for rotation
    Point center = Point(args->Position.X, args->Position.Y);
    deltaTransform->CenterX = center.X;
    deltaTransform->CenterY = center.Y;

    // Look at the Delta property of the ManipulationDeltaRoutedEventArgs to retrieve
    // the rotation, X, and Y changes
    deltaTransform->Rotation = args->Delta.Rotation;
    deltaTransform->TranslateX = args->Delta.Translation.X;
    deltaTransform->TranslateY = args->Delta.Translation.Y;
}

// When a manipuliation that's a result of inertia begins, change the color of the
// object to reflect that inertia has taken over
void ManipulationProcessor::OnManipulationInertiaStarting(GestureRecognizer^ sender, Windows::UI::Input::ManipulationInertiaStartingEventArgs^ args)
{
    element->Background = ref new SolidColorBrush(Windows::UI::Colors::RoyalBlue);

}

// When a manipulation has finished, rest the color of the object
void ManipulationProcessor::OnManipulationCompleted(GestureRecognizer^ sender, Windows::UI::Input::ManipulationCompletedEventArgs^ args)
{
    element->Background = ref new SolidColorBrush(Windows::UI::Colors::LightGray);
}

void ManipulationProcessor::InitializeTransforms()
{
    cumulativeTransform = ref new TransformGroup();
    deltaTransform = ref new CompositeTransform();
    previousTransform = ref new MatrixTransform();
    previousTransform->Matrix = Matrix::Identity;

    cumulativeTransform->Children->Append(previousTransform);
    cumulativeTransform->Children->Append(deltaTransform);

    element->RenderTransform = cumulativeTransform;
}

void ManipulationProcessor::ResetRect()
{
    xamlGestureRecognizer->CompleteGesture();
    InitializeTransforms();
}

void Scenario1_LowLatencyInput::ResetButton_Pressed(Object^ sender, RoutedEventArgs^ e)
{
    //set rect back to original coordinates
    directXGestureRecognizer->CompleteGesture();
    main->ResetRect();
    manipulationProcessor->ResetRect();
}

void Scenario1_LowLatencyInput::BlockUIThread_Toggled(Object^ sender, RoutedEventArgs^ e)
{
    if (dispatcherTimer->IsEnabled)
    {
        dispatcherTimer->Stop();
    }
    else
    {
        dispatcherTimer->Start();
    }
}

void Scenario1_LowLatencyInput::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args)
{
    critical_section::scoped_lock lock(main->GetCriticalSection());
    deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
}

void Scenario1_LowLatencyInput::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ args)
{
    critical_section::scoped_lock lock(main->GetCriticalSection());
    deviceResources->SetLogicalSize(args->NewSize);
}

void Scenario1_LowLatencyInput::OnSuspending(Object^ /* sender */, Windows::ApplicationModel::SuspendingEventArgs^ /* e */)
{
    critical_section::scoped_lock lock(main->GetCriticalSection());
    deviceResources->Trim();
 }

// Window event handlers.
void Scenario1_LowLatencyInput::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
    windowVisible = args->Visible;
    if (windowVisible)
    {
        main->StartRenderLoop();
    }
    else
    {
        main->StopRenderLoop();
    }
}

// DisplayInformation event handlers.
void Scenario1_LowLatencyInput::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
    critical_section::scoped_lock lock(main->GetCriticalSection());
    deviceResources->SetDpi(sender->LogicalDpi);
}

void Scenario1_LowLatencyInput::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
    critical_section::scoped_lock lock(main->GetCriticalSection());
    deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
}

void Scenario1_LowLatencyInput::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
    critical_section::scoped_lock lock(main->GetCriticalSection());
    deviceResources->ValidateDevice();
}
