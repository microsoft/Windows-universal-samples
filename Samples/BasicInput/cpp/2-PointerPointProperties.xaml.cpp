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
#include "2-PointerPointProperties.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario2::Scenario2() : rootPage(SDKTemplate::MainPage::Current)
{
    InitializeComponent();

    // Add event handlers
    mainCanvas->PointerPressed += ref new PointerEventHandler(this, &Scenario2::Pointer_Pressed);
    mainCanvas->PointerMoved += ref new PointerEventHandler(this, &Scenario2::Pointer_Moved);
    mainCanvas->PointerReleased += ref new PointerEventHandler(this, &Scenario2::Pointer_Released);
    mainCanvas->PointerExited += ref new PointerEventHandler(this, &Scenario2::Pointer_Released);
    mainCanvas->PointerEntered += ref new PointerEventHandler(this, &Scenario2::Pointer_Entered);
    mainCanvas->PointerWheelChanged += ref new PointerEventHandler(this, &Scenario2::Pointer_Wheel_Changed);
}

void Scenario2::Pointer_Pressed(Platform::Object^ sender, PointerRoutedEventArgs^ e)
{
    Windows::UI::Input::PointerPoint^ currentPoint = e->GetCurrentPoint(mainCanvas);

    // Retrieve the properties for the curret PointerPoint and display
    // them alongside the pointer's location on screen
    CreateOrUpdatePropertyPopUp(currentPoint);
}

void Scenario2::Pointer_Entered(Platform::Object^ sender, PointerRoutedEventArgs^ e)
{
    Windows::UI::Input::PointerPoint^ currentPoint = e->GetCurrentPoint(mainCanvas);

    // Determine if the point that entered the canvas is "in contact" with the
    // screen (mouse button pressed, finger down, or pen down).
    // This will be encountered if the user touches down in the app surface,
    // drags their finger off of the app, and drags it back onto the app without
    // lifting their finger.
    if (currentPoint->IsInContact)
    {
        CreateOrUpdatePropertyPopUp(currentPoint);
    }
}

void Scenario2::Pointer_Moved(Platform::Object^ sender, PointerRoutedEventArgs^ e)
{
    // Retrieve the point associated with the current event
    Windows::UI::Input::PointerPoint^ currentPoint = e->GetCurrentPoint(mainCanvas);

    // Create a popup if the pointer being moved is in contact with the screen
    if (currentPoint->IsInContact)
    {
        CreateOrUpdatePropertyPopUp(currentPoint);
    }
}

void Scenario2::Pointer_Released(Platform::Object^ sender, PointerRoutedEventArgs^ e)
{
    // Retrieve the point associated with the current event
    Windows::UI::Input::PointerPoint^ currentPoint = e->GetCurrentPoint(mainCanvas);

    // Remove the popup corresponding to the pointer
    HidePropertyPopUp(currentPoint);
}

void Scenario2::Pointer_Wheel_Changed(Platform::Object^ sender, PointerRoutedEventArgs^ e)
{
    // Retrieve the point associated with the current event and record the
    // mouse wheel delta
    Windows::UI::Input::PointerPoint^ currentPoint = e->GetCurrentPoint(mainCanvas);
    if (currentPoint->IsInContact)
    {
        CreateOrUpdatePropertyPopUp(currentPoint);
    }
}

void Scenario2::CreateOrUpdatePropertyPopUp(Windows::UI::Input::PointerPoint^ currentPoint)
{
    // Retrieve the properties that are common to all pointers
    Platform::String^ pointerProperties = GetPointerProperties(currentPoint);

    // Retrieve the properties that are specific to the device type associated
    // with the current PointerPoint
    Platform::String^ deviceSpecificProperties = GetDeviceSpecificProperties(currentPoint);

    RenderPropertyPopUp(pointerProperties, deviceSpecificProperties, currentPoint);
}

// Return the properties that are common to all pointers
Platform::String^ Scenario2::GetPointerProperties(Windows::UI::Input::PointerPoint^ currentPoint)
{
    Platform::String^ sb = ref new Platform::String();
    sb += "ID: " + currentPoint->PointerId;
    sb += "\nX: " + currentPoint->Position.X;
    sb += "\nY: " + currentPoint->Position.Y;
    sb += "\nContact: " + currentPoint->IsInContact + "\n";
    return sb;
}

Platform::String^ Scenario2::GetDeviceSpecificProperties(Windows::UI::Input::PointerPoint^ currentPoint)
{
    Platform::String^ deviceSpecificProperties = "";

    // Detect the type of device being used by examining PointerDeviceType
    // to distinguish between mouse, touch, and pen
    switch (currentPoint->PointerDevice->PointerDeviceType)
    {
    case Windows::Devices::Input::PointerDeviceType::Mouse:
        deviceSpecificProperties = GetMouseProperties(currentPoint);
        break;
    case Windows::Devices::Input::PointerDeviceType::Pen:
        deviceSpecificProperties = GetPenProperties(currentPoint);
        break;
    case Windows::Devices::Input::PointerDeviceType::Touch:
        deviceSpecificProperties = GetTouchProperties(currentPoint);
        break;
    }

    return deviceSpecificProperties;
}

// Return the properties that are specific to mice
Platform::String^ Scenario2::GetMouseProperties(Windows::UI::Input::PointerPoint^ currentPoint)
{
    Windows::UI::Input::PointerPointProperties^ pointerProperties = currentPoint->Properties;
    Platform::String^ sb = ref new Platform::String();

    sb += "Left button: " + pointerProperties->IsLeftButtonPressed;
    sb += "\nRight button: " + pointerProperties->IsRightButtonPressed;
    sb += "\nMiddle button: " + pointerProperties->IsMiddleButtonPressed;
    sb += "\nX1 button: " + pointerProperties->IsXButton1Pressed;
    sb += "\nX2 button: " + pointerProperties->IsXButton2Pressed;
    sb += "\nMouse wheel delta: " + pointerProperties->MouseWheelDelta;

    return sb;
}

// Return the properties that are specific to touch
Platform::String^ Scenario2::GetTouchProperties(Windows::UI::Input::PointerPoint^ currentPoint)
{
    Windows::UI::Input::PointerPointProperties^ pointerProperties = currentPoint->Properties;
    Platform::String^ sb = ref new Platform::String();

    sb += "Contact Rect X: " + pointerProperties->ContactRect.X;
    sb += "\nContact Rect Y: " + pointerProperties->ContactRect.Y;
    sb += "\nContact Rect Width: " + pointerProperties->ContactRect.Width;
    sb += "\nContact Rect Height: " + pointerProperties->ContactRect.Height;

    return sb;
}

// Return the properties that are specific to pen
Platform::String^ Scenario2::GetPenProperties(Windows::UI::Input::PointerPoint^ currentPoint)
{
    Windows::UI::Input::PointerPointProperties^ pointerProperties = currentPoint->Properties;
    Platform::String^ sb = ref new Platform::String();

    sb += "Barrel button: " + pointerProperties->IsBarrelButtonPressed;
    sb += "\nEraser: " + pointerProperties->IsEraser;
    sb += "\nPressure: " + pointerProperties->Pressure;

    return sb;
}

//
// Popup UI code below
//
void Scenario2::RenderPropertyPopUp(Platform::String^ pointerProperties, Platform::String^ deviceSpecificProperties, Windows::UI::Input::PointerPoint^ currentPoint)
{
    auto it = popups.find(currentPoint->PointerId);
    if (it != popups.end())
    {
        auto tb = dynamic_cast<StackPanel^>(it->second);
        TextBlock^ pointerText = dynamic_cast<TextBlock^>(tb->Children->GetAt(0));
        pointerText->Text = pointerProperties;
        TextBlock^ deviceSpecificText = dynamic_cast<TextBlock^>(tb->Children->GetAt(1));
        deviceSpecificText->Text = deviceSpecificProperties;
    }
    else
    {
        StackPanel^ pointerPanel = ref new StackPanel();
        TextBlock^ pointerText = ref new TextBlock();
        pointerText->Text = pointerProperties;
        pointerPanel->Children->Append(pointerText);

        TextBlock^ deviceSpecificText = ref new TextBlock();
        deviceSpecificText->Text = deviceSpecificProperties;

        // Detect the type of device being used by examining PointerDeviceType
        // to distinguish between mouse, touch, and pen
        switch (currentPoint->PointerDevice->PointerDeviceType)
        {
        case Windows::Devices::Input::PointerDeviceType::Mouse:
            deviceSpecificText->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);
            break;

        case Windows::Devices::Input::PointerDeviceType::Touch:
            deviceSpecificText->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Green);
            break;

        case Windows::Devices::Input::PointerDeviceType::Pen:
            deviceSpecificText->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Yellow);
            break;
        }
        pointerPanel->Children->Append(deviceSpecificText);
        popups[currentPoint->PointerId] = pointerPanel;
        mainCanvas->Children->Append(popups[currentPoint->PointerId]);
    }

    TranslateTransform^ transform = ref new TranslateTransform();
    transform->X = currentPoint->Position.X + 24;
    transform->Y = currentPoint->Position.Y + 24;
    popups[currentPoint->PointerId]->RenderTransform = transform;
}

void Scenario2::HidePropertyPopUp(Windows::UI::Input::PointerPoint^ currentPoint)
{
    auto it = popups.find(currentPoint->PointerId);
    if (it != popups.end())
    {
        auto stackPanel = dynamic_cast<StackPanel^>(it->second);
        unsigned int index;
        mainCanvas->Children->IndexOf(stackPanel, &index);
        mainCanvas->Children->RemoveAt(index);
        popups.erase(it);
    }
}
