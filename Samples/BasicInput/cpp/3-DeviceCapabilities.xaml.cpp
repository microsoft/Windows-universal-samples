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
#include "3-DeviceCapabilities.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario3::Scenario3()
{
    InitializeComponent();

    // Retrieve information about whether or not a keyboard is present
    KeyboardCapabilities^ kbdCapabilities = ref new KeyboardCapabilities();
    keyboardText->Text = "Keyboard present = " + kbdCapabilities->KeyboardPresent.ToString();

    // Retrieve information about the capabilities of the device's mouse.  This includes:
    // - Whether or not a mouse is present
    // - The number of buttons on the mouse
    // - Whether or not the mouse has a vertical scroll wheel
    // - Whether or not the mouse has a horizontal scroll wheel
    // - Whether or not the user has elected to swap the mouse buttons, causing
    //   the right mouse button to be the primary button
    MouseCapabilities^ mouseCapabilities = ref new MouseCapabilities();
    Platform::String^ sb = ref new Platform::String();

    sb += "Mouse present = " + mouseCapabilities->MousePresent.ToString() + "\n";
    sb += "Number of buttons = " + mouseCapabilities->NumberOfButtons.ToString() + "\n";
    sb += "Vertical wheel present = " + mouseCapabilities->VerticalWheelPresent.ToString() + "\n";
    sb += "Horizontal wheel present = " + mouseCapabilities->HorizontalWheelPresent.ToString() + "\n";
    sb += "Buttons swapped = " + mouseCapabilities->SwapButtons.ToString();
    mouseText->Text = sb;

    // Retrieve information about the capabilities of the device's mouse.  This includes:
    // - Whether or not the device supports touch
    // - The supported number of simultaneous touch contacts
    TouchCapabilities^ touchCapabilities = ref new TouchCapabilities();
    sb = ref new Platform::String();
    sb += "Touch present = " + touchCapabilities->TouchPresent.ToString() + "\n";
    sb += "Touch contacts supported = " + touchCapabilities->Contacts.ToString();
    touchText->Text = sb;
}
