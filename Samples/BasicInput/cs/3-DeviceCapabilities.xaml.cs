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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace BasicInput
{
    public sealed partial class Scenario3 : Page
    {
        public Scenario3()
        {
            this.InitializeComponent();

            // Retrieve information about whether or not a keyboard is present
            Windows.Devices.Input.KeyboardCapabilities kbdCapabilities = new Windows.Devices.Input.KeyboardCapabilities();
            keyboardText.Text = "Keyboard present = " + kbdCapabilities.KeyboardPresent.ToString();

            // Retrieve information about the capabilities of the device's mouse.  This includes:
            // - Whether or not a mouse is present
            // - The number of buttons on the mouse
            // - Whether or not the mouse has a vertical scroll wheel
            // - Whether or not the mouse has a horizontal scroll wheel
            // - Whether or not the user has elected to swap the mouse buttons, causing
            //   the right mouse button to be the primary button
            Windows.Devices.Input.MouseCapabilities mouseCapabilities = new Windows.Devices.Input.MouseCapabilities();
            System.Text.StringBuilder sb = new System.Text.StringBuilder();
            sb.Append("Mouse present = " + mouseCapabilities.MousePresent.ToString() + "\n");
            sb.Append("Number of buttons = " + mouseCapabilities.NumberOfButtons.ToString() + "\n");
            sb.Append("Vertical wheel present = " + mouseCapabilities.VerticalWheelPresent.ToString() + "\n");
            sb.Append("Horizontal wheel present = " + mouseCapabilities.HorizontalWheelPresent.ToString() + "\n");
            sb.Append("Buttons swapped = " + mouseCapabilities.SwapButtons.ToString());
            mouseText.Text = sb.ToString();

            // Retrieve information about the capabilities of the device's mouse.  This includes:
            // - Whether or not the device supports touch
            // - The supported number of simultaneous touch contacts
            Windows.Devices.Input.TouchCapabilities touchCapabilities = new Windows.Devices.Input.TouchCapabilities();
            sb = new System.Text.StringBuilder();
            sb.Append("Touch present = " + touchCapabilities.TouchPresent.ToString() + "\n");
            sb.Append("Touch contacts supported = " + touchCapabilities.Contacts.ToString());
            touchText.Text = sb.ToString();
        }
    }
}
