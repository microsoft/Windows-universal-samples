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

using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    public sealed partial class Scenario2 : Page
    {
        Dictionary<uint, StackPanel> popups;

        public Scenario2()
        {
            this.InitializeComponent();

            // Contact tracking for UI popups
            popups = new Dictionary<uint, StackPanel>();

            // Add event handlers
            mainCanvas.PointerPressed += new PointerEventHandler(Pointer_Pressed);
            mainCanvas.PointerMoved += new PointerEventHandler(Pointer_Moved);
            mainCanvas.PointerReleased += new PointerEventHandler(Pointer_Released);
            mainCanvas.PointerExited += new PointerEventHandler(Pointer_Released);
            mainCanvas.PointerEntered += new PointerEventHandler(Pointer_Entered);
            mainCanvas.PointerWheelChanged += new PointerEventHandler(Pointer_Wheel_Changed);
        }

        void Pointer_Pressed(object sender, PointerRoutedEventArgs e)
        {
            Windows.UI.Input.PointerPoint currentPoint = e.GetCurrentPoint(mainCanvas);

            // Retrieve the properties for the curret PointerPoint and display
            // them alongside the pointer's location on screen
            CreateOrUpdatePropertyPopUp(currentPoint);
        }

        void Pointer_Entered(object sender, PointerRoutedEventArgs e)
        {
            Windows.UI.Input.PointerPoint currentPoint = e.GetCurrentPoint(mainCanvas);

            // Determine if the point that entered the canvas is "in contact" with the
            // screen (mouse button pressed, finger down, or pen down).
            // This will be encountered if the user touches down in the app surface,
            // drags their finger off of the app, and drags it back onto the app without
            // lifting their finger.
            if (currentPoint.IsInContact)
            {
                CreateOrUpdatePropertyPopUp(currentPoint);
            }
        }

        void Pointer_Moved(object sender, PointerRoutedEventArgs e)
        {
            // Retrieve the point associated with the current event
            Windows.UI.Input.PointerPoint currentPoint = e.GetCurrentPoint(mainCanvas);

            // Create a popup if the pointer being moved is in contact with the screen
            if (currentPoint.IsInContact)
            {
                CreateOrUpdatePropertyPopUp(currentPoint);
            }
        }

        void Pointer_Released(object sender, PointerRoutedEventArgs e)
        {
            // Retrieve the point associated with the current event
            Windows.UI.Input.PointerPoint currentPoint = e.GetCurrentPoint(mainCanvas);

            // Remove the popup corresponding to the pointer
            HidePropertyPopUp(currentPoint);
        }

        void Pointer_Wheel_Changed(object sender, PointerRoutedEventArgs e)
        {
            // Retrieve the point associated with the current event and record the
            // mouse wheel delta
            Windows.UI.Input.PointerPoint currentPoint = e.GetCurrentPoint(mainCanvas);
            if (currentPoint.IsInContact)
            {
                CreateOrUpdatePropertyPopUp(currentPoint);
            }
        }

        void CreateOrUpdatePropertyPopUp(Windows.UI.Input.PointerPoint currentPoint)
        {
            // Retrieve the properties that are common to all pointers
            String pointerProperties = GetPointerProperties(currentPoint);

            // Retrieve the properties that are specific to the device type associated
            // with the current PointerPoint
            String deviceSpecificProperties = GetDeviceSpecificProperties(currentPoint);

            RenderPropertyPopUp(pointerProperties, deviceSpecificProperties, currentPoint);
        }

        // Return the properties that are common to all pointers
        String GetPointerProperties(Windows.UI.Input.PointerPoint currentPoint)
        {
            System.Text.StringBuilder sb = new System.Text.StringBuilder();

            sb.Append("ID: " + currentPoint.PointerId);
            sb.Append("\nX: " + currentPoint.Position.X);
            sb.Append("\nY: " + currentPoint.Position.Y);
            sb.Append("\nContact: " + currentPoint.IsInContact + "\n");

            return sb.ToString(); ;
        }

        String GetDeviceSpecificProperties(Windows.UI.Input.PointerPoint currentPoint)
        {
            String deviceSpecificProperties = "";

            // Detect the type of device being used by examining PointerDeviceType
            // to distinguish between mouse, touch, and pen
            switch (currentPoint.PointerDevice.PointerDeviceType)
            {
                case Windows.Devices.Input.PointerDeviceType.Mouse:
                    deviceSpecificProperties = GetMouseProperties(currentPoint);
                    break;
                case Windows.Devices.Input.PointerDeviceType.Pen:
                    deviceSpecificProperties = GetPenProperties(currentPoint);
                    break;
                case Windows.Devices.Input.PointerDeviceType.Touch:
                    deviceSpecificProperties = GetTouchProperties(currentPoint);
                    break;
            }

            return deviceSpecificProperties;
        }

        // Return the properties that are specific to mice
        String GetMouseProperties(Windows.UI.Input.PointerPoint currentPoint)
        {
            Windows.UI.Input.PointerPointProperties pointerProperties = currentPoint.Properties;
            System.Text.StringBuilder sb = new System.Text.StringBuilder();

            sb.Append("Left button: " + pointerProperties.IsLeftButtonPressed);
            sb.Append("\nRight button: " + pointerProperties.IsRightButtonPressed);
            sb.Append("\nMiddle button: " + pointerProperties.IsMiddleButtonPressed);
            sb.Append("\nX1 button: " + pointerProperties.IsXButton1Pressed);
            sb.Append("\nX2 button: " + pointerProperties.IsXButton2Pressed);
            sb.Append("\nMouse wheel delta: " + pointerProperties.MouseWheelDelta);

            return sb.ToString();
        }

        // Return the properties that are specific to touch
        String GetTouchProperties(Windows.UI.Input.PointerPoint currentPoint)
        {
            Windows.UI.Input.PointerPointProperties pointerProperties = currentPoint.Properties;
            System.Text.StringBuilder sb = new System.Text.StringBuilder();

            sb.Append("Contact Rect X: " + pointerProperties.ContactRect.X);
            sb.Append("\nContact Rect Y: " + pointerProperties.ContactRect.Y);
            sb.Append("\nContact Rect Width: " + pointerProperties.ContactRect.Width);
            sb.Append("\nContact Rect Height: " + pointerProperties.ContactRect.Height);

            return sb.ToString(); ;
        }

        // Return the properties that are specific to pen
        String GetPenProperties(Windows.UI.Input.PointerPoint currentPoint)
        {
            Windows.UI.Input.PointerPointProperties pointerProperties = currentPoint.Properties;
            System.Text.StringBuilder sb = new System.Text.StringBuilder();

            sb.Append("Barrel button: " + pointerProperties.IsBarrelButtonPressed);
            sb.Append("\nEraser: " + pointerProperties.IsEraser);
            sb.Append("\nPressure: " + pointerProperties.Pressure);

            return sb.ToString();
        }

        //
        // Popup UI code below
        //
        void RenderPropertyPopUp(String pointerProperties, String deviceSpecificProperties, Windows.UI.Input.PointerPoint currentPoint)
        {
            if (popups.ContainsKey(currentPoint.PointerId))
            {
                TextBlock pointerText = (TextBlock)popups[currentPoint.PointerId].Children[0];
                pointerText.Text = pointerProperties;
                TextBlock deviceSpecificText = (TextBlock)popups[currentPoint.PointerId].Children[1];
                deviceSpecificText.Text = deviceSpecificProperties;
            }
            else
            {
                StackPanel pointerPanel = new StackPanel();
                TextBlock pointerText = new TextBlock();
                pointerText.Text = pointerProperties;
                pointerPanel.Children.Add(pointerText);

                TextBlock deviceSpecificText = new TextBlock();
                deviceSpecificText.Text = deviceSpecificProperties;

                // Detect the type of device being used by examining PointerDeviceType
                // to distinguish between mouse, touch, and pen
                switch (currentPoint.PointerDevice.PointerDeviceType)
                {
                    case Windows.Devices.Input.PointerDeviceType.Mouse:
                        deviceSpecificText.Foreground = new SolidColorBrush(Windows.UI.Colors.Red);
                        break;

                    case Windows.Devices.Input.PointerDeviceType.Touch:
                        deviceSpecificText.Foreground = new SolidColorBrush(Windows.UI.Colors.Green);
                        break;

                    case Windows.Devices.Input.PointerDeviceType.Pen:
                        deviceSpecificText.Foreground = new SolidColorBrush(Windows.UI.Colors.Yellow);
                        break;
                }
                pointerPanel.Children.Add(deviceSpecificText);
                popups[currentPoint.PointerId] = pointerPanel;
                mainCanvas.Children.Add(popups[currentPoint.PointerId]);
            }
            TranslateTransform transform = new TranslateTransform();
            transform.X = currentPoint.Position.X + 24;
            transform.Y = currentPoint.Position.Y + 24;
            popups[currentPoint.PointerId].RenderTransform = transform;
        }

        void HidePropertyPopUp(Windows.UI.Input.PointerPoint currentPoint)
        {
            if (popups.ContainsKey(currentPoint.PointerId))
            {
                mainCanvas.Children.Remove(popups[currentPoint.PointerId]);
                popups[currentPoint.PointerId] = null;
                popups.Remove(currentPoint.PointerId);
            }
        }
    }
}
