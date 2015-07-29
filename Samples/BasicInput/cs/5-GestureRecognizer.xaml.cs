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

using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Input;
using Windows.UI.Input;

namespace BasicInput
{
    public sealed partial class Scenario5 : Page
    {
        GestureRecognizer recognizer;
        ManipulationInputProcessor manipulationProcessor;

        public Scenario5()
        {
            this.InitializeComponent();

            InitOptions();

            // Create a GestureRecognizer which will be used to process the manipulations
            // done on the rectangle
            recognizer = new GestureRecognizer();

            // Create a ManipulationInputProcessor which will listen for events on the
            // rectangle, process them, and update the rectangle's position, size, and rotation
            manipulationProcessor = new ManipulationInputProcessor(recognizer, manipulateMe, mainCanvas);
        }

        private void InitOptions()
        {
            movementAxis.SelectedIndex = 0;
            InertiaSwitch.IsOn = true;
        }

        private void movementAxis_Changed(object sender, SelectionChangedEventArgs e)
        {
            if (manipulationProcessor == null)
            {
                return;
            }

            ComboBoxItem selectedItem = (ComboBoxItem)((ComboBox)sender).SelectedItem;
            switch (selectedItem.Content.ToString())
            {
                case "X only":
                    manipulationProcessor.LockToXAxis();
                    break;
                case "Y only":
                    manipulationProcessor.LockToYAxis();
                    break;
                default:
                    manipulationProcessor.MoveOnXAndYAxes();
                    break;
            }
        }

        private void InertiaSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            if (manipulationProcessor == null)
            {
                return;
            }

            manipulationProcessor.UseInertia(InertiaSwitch.IsOn);
        }

        void resetButton_Pressed(object sender, RoutedEventArgs e)
        {
            InitOptions();
            manipulationProcessor.Reset();
        }
    }

    class ManipulationInputProcessor
    {
        GestureRecognizer recognizer;
        UIElement element;
        UIElement reference;
        TransformGroup cumulativeTransform;
        MatrixTransform previousTransform;
        CompositeTransform deltaTransform;

        public ManipulationInputProcessor(GestureRecognizer gestureRecognizer, UIElement target, UIElement referenceFrame)
        {
            recognizer = gestureRecognizer;
            element = target;
            reference = referenceFrame;

            // Initialize the transforms that will be used to manipulate the shape
            InitializeTransforms();

            // The GestureSettings property dictates what manipulation events the
            // Gesture Recognizer will listen to.  This will set it to a limited
            // subset of these events.
            recognizer.GestureSettings = GenerateDefaultSettings();

            // Set up pointer event handlers. These receive input events that are used by the gesture recognizer.
            element.PointerPressed += OnPointerPressed;
            element.PointerMoved += OnPointerMoved;
            element.PointerReleased += OnPointerReleased;
            element.PointerCanceled += OnPointerCanceled;

            // Set up event handlers to respond to gesture recognizer output
            recognizer.ManipulationStarted += OnManipulationStarted;
            recognizer.ManipulationUpdated += OnManipulationUpdated;
            recognizer.ManipulationCompleted += OnManipulationCompleted;
            recognizer.ManipulationInertiaStarting += OnManipulationInertiaStarting;
        }

        public void InitializeTransforms()
        {
            cumulativeTransform = new TransformGroup();
            deltaTransform = new CompositeTransform();
            previousTransform = new MatrixTransform() { Matrix = Matrix.Identity };

            cumulativeTransform.Children.Add(previousTransform);
            cumulativeTransform.Children.Add(deltaTransform);

            element.RenderTransform = cumulativeTransform;
        }

        // Return the default GestureSettings for this sample
        GestureSettings GenerateDefaultSettings()
        {
            return GestureSettings.ManipulationTranslateX |
                GestureSettings.ManipulationTranslateY |
                GestureSettings.ManipulationRotate |
                GestureSettings.ManipulationTranslateInertia |
                GestureSettings.ManipulationRotateInertia;
        }

        // Route the pointer pressed event to the gesture recognizer.
        // The points are in the reference frame of the canvas that contains the rectangle element.
        void OnPointerPressed(object sender, PointerRoutedEventArgs args)
        {
            // Set the pointer capture to the element being interacted with so that only it
            // will fire pointer-related events
            element.CapturePointer(args.Pointer);

            // Feed the current point into the gesture recognizer as a down event
            recognizer.ProcessDownEvent(args.GetCurrentPoint(reference));
        }

        // Route the pointer moved event to the gesture recognizer.
        // The points are in the reference frame of the canvas that contains the rectangle element.
        void OnPointerMoved(object sender, PointerRoutedEventArgs args)
        {
            // Feed the set of points into the gesture recognizer as a move event
            recognizer.ProcessMoveEvents(args.GetIntermediatePoints(reference));
        }

        // Route the pointer released event to the gesture recognizer.
        // The points are in the reference frame of the canvas that contains the rectangle element.
        void OnPointerReleased(object sender, PointerRoutedEventArgs args)
        {
            // Feed the current point into the gesture recognizer as an up event
            recognizer.ProcessUpEvent(args.GetCurrentPoint(reference));

            // Release the pointer
            element.ReleasePointerCapture(args.Pointer);
        }

        // Route the pointer canceled event to the gesture recognizer.
        // The points are in the reference frame of the canvas that contains the rectangle element.
        void OnPointerCanceled(object sender, PointerRoutedEventArgs args)
        {
            recognizer.CompleteGesture();
            element.ReleasePointerCapture(args.Pointer);
        }

        // When a manipulation begins, change the color of the object to reflect
        // that a manipulation is in progress
        void OnManipulationStarted(object sender, ManipulationStartedEventArgs e)
        {
            Border b = element as Border;
            b.Background = new SolidColorBrush(Windows.UI.Colors.DeepSkyBlue);
        }

        // Process the change resulting from a manipulation
        void OnManipulationUpdated(object sender, ManipulationUpdatedEventArgs e)
        {
            previousTransform.Matrix = cumulativeTransform.Value;

            // Get the center point of the manipulation for rotation
            Point center = new Point(e.Position.X, e.Position.Y);
            deltaTransform.CenterX = center.X;
            deltaTransform.CenterY = center.Y;

            // Look at the Delta property of the ManipulationDeltaRoutedEventArgs to retrieve
            // the rotation, X, and Y changes
            deltaTransform.Rotation = e.Delta.Rotation;
            deltaTransform.TranslateX = e.Delta.Translation.X;
            deltaTransform.TranslateY = e.Delta.Translation.Y;
        }

        // When a manipulation that's a result of inertia begins, change the color of the
        // the object to reflect that inertia has taken over
        void OnManipulationInertiaStarting(object sender, ManipulationInertiaStartingEventArgs e)
        {
            Border b = element as Border;
            b.Background = new SolidColorBrush(Windows.UI.Colors.RoyalBlue);
        }

        // When a manipulation has finished, reset the color of the object
        void OnManipulationCompleted(object sender, ManipulationCompletedEventArgs e)
        {
            Border b = element as Border;
            b.Background = new SolidColorBrush(Windows.UI.Colors.LightGray);
        }

        // Modify the GestureSettings property to only allow movement on the X axis
        public void LockToXAxis()
        {
            recognizer.CompleteGesture();
            recognizer.GestureSettings |= GestureSettings.ManipulationTranslateY | GestureSettings.ManipulationTranslateX;
            recognizer.GestureSettings ^= GestureSettings.ManipulationTranslateY;
        }

        // Modify the GestureSettings property to only allow movement on the Y axis
        public void LockToYAxis()
        {
            recognizer.CompleteGesture();
            recognizer.GestureSettings |= GestureSettings.ManipulationTranslateY | GestureSettings.ManipulationTranslateX;
            recognizer.GestureSettings ^= GestureSettings.ManipulationTranslateX;
        }

        // Modify the GestureSettings property to allow movement on both the the X and Y axes
        public void MoveOnXAndYAxes()
        {
            recognizer.CompleteGesture();
            recognizer.GestureSettings |= GestureSettings.ManipulationTranslateX | GestureSettings.ManipulationTranslateY;
        }

        // Modify the GestureSettings property to enable or disable inertia based on the passed-in value
        public void UseInertia(bool inertia)
        {
            if (!inertia)
            {
                recognizer.CompleteGesture();
                recognizer.GestureSettings ^= GestureSettings.ManipulationTranslateInertia | GestureSettings.ManipulationRotateInertia;
            }
            else
            {
                recognizer.GestureSettings |= GestureSettings.ManipulationTranslateInertia | GestureSettings.ManipulationRotateInertia;
            }
        }

        public void Reset()
        {
            element.RenderTransform = null;
            recognizer.CompleteGesture();
            InitializeTransforms();
            recognizer.GestureSettings = GenerateDefaultSettings();
        }
    }
}