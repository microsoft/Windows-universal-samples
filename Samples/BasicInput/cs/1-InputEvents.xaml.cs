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

using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    public sealed partial class Scenario1 : Page
    {
        public Scenario1()
        {
            this.InitializeComponent();

            // pointer press/release handlers
            pressedTarget.PointerPressed += new PointerEventHandler(target_PointerPressed);
            pressedTarget.PointerReleased += new PointerEventHandler(target_PointerReleased);

            // pointer enter/exit handlers
            enterExitTarget.PointerEntered += new PointerEventHandler(target_PointerEntered);
            enterExitTarget.PointerExited += new PointerEventHandler(target_PointerExited);

            // gesture handlers
            tapTarget.Tapped += new TappedEventHandler(target_Tapped);
            tapTarget.DoubleTapped += new DoubleTappedEventHandler(target_DoubleTapped);

            holdTarget.Holding += new HoldingEventHandler(target_Holding);
            holdTarget.RightTapped += new RightTappedEventHandler(target_RightTapped);
        }

        // A PointerPressed event is sent whenever a mouse button, finger, or pen is pressed to make
        // contact with an object
        void target_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            pressedTarget.Background = new SolidColorBrush(Windows.UI.Colors.RoyalBlue);
            pressedTargetText.Text = "Pointer Pressed";
        }

        // A PointerReleased event is sent whenever a mouse button, finger, or pen is released to remove
        // contact from an object
        void target_PointerReleased(object sender, PointerRoutedEventArgs e)
        {
            pressedTarget.Background = new SolidColorBrush(Windows.UI.Colors.LightGray);
            pressedTargetText.Text = "Pointer Released";
        }

        // A PointerEntered event is sent whenever a mouse cursor is moved on top of an object
        // or when a pen or finger is dragged on top of an object
        void target_PointerEntered(object sender, PointerRoutedEventArgs e)
        {
            enterExitTarget.Background = new SolidColorBrush(Windows.UI.Colors.RoyalBlue);
            enterExitTargetText.Text = "Pointer Entered";
        }

        // A PointerExited event is sent whenever a mouse cursor is moved off of an object
        // or when a pen or finger is dragged off of an object
        void target_PointerExited(object sender, PointerRoutedEventArgs e)
        {
            enterExitTarget.Background = new SolidColorBrush(Windows.UI.Colors.LightGray);
            enterExitTargetText.Text = "Pointer Exited";
        }

        // A Tapped event is sent whenever a mouse is clicked or a finger or pen taps
        // the object
        void target_Tapped(object sender, TappedRoutedEventArgs e)
        {
            tapTarget.Background = new SolidColorBrush(Windows.UI.Colors.DeepSkyBlue);
            tapTargetText.Text = "Tapped";
        }

        // A DoubleTapped event is sent whenever a mouse is double-clicked or a finger or pen taps
        // the object twice in quick succession
        void target_DoubleTapped(object sender, DoubleTappedRoutedEventArgs e)
        {
            tapTarget.Background = new SolidColorBrush(Windows.UI.Colors.RoyalBlue);
            tapTargetText.Text = "Double-Tapped";
        }

        // A RightTapped event is sent whenever a mouse is right-clicked or a finger or pen
        // completes a Holding event.  This is intended to be used to handle secondary actions
        // on an object.
        void target_RightTapped(object sender, RightTappedRoutedEventArgs e)
        {
            holdTarget.Background = new SolidColorBrush(Windows.UI.Colors.RoyalBlue);
            holdTargetText.Text = "Right Tapped";
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
        void target_Holding(object sender, HoldingRoutedEventArgs e)
        {
            if (e.HoldingState == Windows.UI.Input.HoldingState.Started)
            {
                holdTarget.Background = new SolidColorBrush(Windows.UI.Colors.DeepSkyBlue);
                holdTargetText.Text = "Holding";
            }
            else if (e.HoldingState == Windows.UI.Input.HoldingState.Completed)
            {
                holdTarget.Background = new SolidColorBrush(Windows.UI.Colors.LightGray);
                holdTargetText.Text = "Held";
            }
            else
            {
                holdTarget.Background = new SolidColorBrush(Windows.UI.Colors.LightGray);
                holdTargetText.Text = "Hold Canceled";
            }
        }
    }
}
