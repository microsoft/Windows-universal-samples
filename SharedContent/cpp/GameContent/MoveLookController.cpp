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
#include "MoveLookController.h"
#include "DirectXSample.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace std;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI;
using namespace Windows::Foundation;
using namespace Windows::Devices::Input;
using namespace Windows::Gaming::Input;
using namespace Windows::System;

// Analog control deadzone definitions. Tune these values to adjust the size of the deadzone.
// Thumbstick range in each dimension is defined as [-1, 1].
#define THUMBSTICK_DEADZONE 0.25f

// Trigger range is defined as [0, 1].
#define TRIGGER_DEADZONE 0.1f

// If a CoreDispatcher is provided, this indicates the game is running on a separate thread
// from the Xaml UI thread, and this separate thread does NOT have access to
// the CoreWindow.  The dispatcher can be used to marshal execution back to
// the Xaml UI thread which does have a CoreWindow.  The Dispatcher is cached
// to enable execution of code on the UI thread to turn on and off the cursor glyph.
MoveLookController::MoveLookController(_In_ CoreWindow^ window, _In_ CoreDispatcher^ dispatcher):
    m_dispatcher(dispatcher),
    m_activeGamepad(nullptr),
    m_autoFire(true),
    m_pitch(0.0f),
    m_yaw(0.0f),
    m_state(MoveLookControllerState::None),
    m_gamepadStartButtonInUse(false),
    m_gamepadTriggerInUse(false),
    m_gamepadsChanged(true)
{
    InitWindow(window);
}

MoveLookController::MoveLookController(_In_ CoreWindow^ window):
    m_dispatcher(nullptr),
    m_activeGamepad(nullptr),
    m_autoFire(true),
    m_pitch(0.0f),
    m_yaw(0.0f),
    m_state(MoveLookControllerState::None),
    m_gamepadStartButtonInUse(false),
    m_gamepadTriggerInUse(false),
    m_gamepadsChanged(true)
{
    InitWindow(window);
}

void MoveLookController::InitWindow(_In_ CoreWindow^ window)
{
    ResetState();

    window->PointerPressed +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerPressed);

    window->PointerMoved +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerMoved);

    window->PointerReleased +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerReleased);

    window->PointerExited +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerExited);

    window->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &MoveLookController::OnKeyDown);

    window->KeyUp +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &MoveLookController::OnKeyUp);

    // There is a separate handler for mouse only relative mouse movement events.
    MouseDevice::GetForCurrentView()->MouseMoved +=
        ref new TypedEventHandler<MouseDevice^, MouseEventArgs^>(this, &MoveLookController::OnMouseMoved);

    SystemNavigationManager::GetForCurrentView()->BackRequested +=
            ref new EventHandler<BackRequestedEventArgs^>(this, &MoveLookController::OnBackRequested);

    // Detect gamepad connection and disconnection events.
    Gamepad::GamepadAdded +=
        ref new EventHandler<Gamepad^>(this, &MoveLookController::OnGamepadAdded);

    Gamepad::GamepadRemoved +=
        ref new EventHandler<Gamepad^>(this, &MoveLookController::OnGamepadRemoved);
}

//----------------------------------------------------------------------

bool MoveLookController::IsPauseRequested()
{
    switch (m_state)
    {
    case MoveLookControllerState::Active:
        UpdatePollingDevices();
        if (m_pausePressed)
        {
#ifdef MOVELOOKCONTROLLER_TRACE
            DebugTrace(L"IsPauseRequested == true\n");
#endif
            m_pausePressed = false;
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

//----------------------------------------------------------------------

bool MoveLookController::IsFiring()
{
    if (m_state == MoveLookControllerState::Active)
    {
        if (m_autoFire)
        {
            return (m_fireInUse || (m_mouseInUse && m_mouseLeftInUse) || PollingFireInUse());
        }
        else
        {
            if (m_firePressed)
            {
                m_firePressed = false;
                return true;
            }
        }
    }
    return false;
}

//----------------------------------------------------------------------

bool MoveLookController::IsPressComplete()
{
    switch (m_state)
    {
    case MoveLookControllerState::WaitForInput:
        UpdatePollingDevices();
        if (m_buttonPressed)
        {
#ifdef MOVELOOKCONTROLLER_TRACE
            DebugTrace(L"IsPressComplete == true\n");
#endif
            m_buttonPressed = false;
            return true;
        }
        else
        {
            return false;
        }
        break;
    }

    return false;
}

//----------------------------------------------------------------------

void MoveLookController::OnPointerPressed(
    _In_ CoreWindow^ /* sender */,
    _In_ PointerEventArgs^ args
    )
{
    PointerPoint^ point = args->CurrentPoint;
    uint32 pointerID = point->PointerId;
    Point pointerPosition = point->Position;
    PointerPointProperties^ pointProperties = point->Properties;
    auto pointerDevice = point->PointerDevice;
    auto pointerDeviceType = pointerDevice->PointerDeviceType;

    XMFLOAT2 position = XMFLOAT2(pointerPosition.X, pointerPosition.Y);

#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"%-7s (%d) at (%4.0f, %4.0f)", L"Pressed", pointerID, position.x, position.y);
#endif

    switch (m_state)
    {
    case MoveLookControllerState::WaitForInput:
        if (position.x > m_buttonUpperLeft.x &&
            position.x < m_buttonLowerRight.x &&
            position.y > m_buttonUpperLeft.y &&
            position.y < m_buttonLowerRight.y)
        {
            // Wait until button released before setting variable.
            m_buttonPointerID = pointerID;
            m_buttonInUse = true;
#ifdef MOVELOOKCONTROLLER_TRACE
            DebugTrace(L"\tWaitForInput(%d) - BUTTON in USE", pointerID);
#endif
        }
        break;

    case MoveLookControllerState::Active:
        switch (pointerDeviceType)
        {
        case Windows::Devices::Input::PointerDeviceType::Touch:
            if (position.x > m_moveUpperLeft.x &&
                position.x < m_moveLowerRight.x &&
                position.y > m_moveUpperLeft.y &&
                position.y < m_moveLowerRight.y)
            {
                // This pointer is in the move control.
                if (!m_moveInUse)
                {
                    // There is not an active pointer in this control yet.
                    // Process a DPad touch down event.
                    m_moveFirstDown = position;                 // Save location of initial contact.
                    m_movePointerID = pointerID;                // Store the pointer using this control.
                    m_moveInUse = true;
                }
            }
            else if (position.x > m_fireUpperLeft.x &&
                position.x < m_fireLowerRight.x &&
                position.y > m_fireUpperLeft.y &&
                position.y < m_fireLowerRight.y)
            {
                // This pointer is in the fire control.
                if (!m_fireInUse)
                {
                    m_fireLastPoint = position;
                    m_firePointerID = pointerID;
                    m_fireInUse = true;
                    if (!m_autoFire)
                    {
                        m_firePressed = true;
                    }
                }
            }
            else
            {
                if (!m_lookInUse)
                {
                    // There is not an active pointer in this control yet.
                    m_lookLastPoint = position;                   // Save point for later move.
                    m_lookPointerID = pointerID;                  // Store the pointer using this control.
                    m_lookLastDelta.x = m_lookLastDelta.y = 0;    // These are for smoothing.
                    m_lookInUse = true;
                }
            }
            break;

        default:
            bool rightButton = pointProperties->IsRightButtonPressed;
            bool leftButton = pointProperties->IsLeftButtonPressed;

            if (!m_autoFire && (!m_mouseLeftInUse && leftButton))
            {
                m_firePressed = true;
            }

            if (!m_mouseInUse)
            {
                m_mouseInUse = true;
                m_mouseLastPoint = position;
                m_mousePointerID = pointerID;
                m_mouseLeftInUse = leftButton;
                m_mouseRightInUse = rightButton;
                m_lookLastDelta.x = m_lookLastDelta.y = 0;          // These are for smoothing.
            }
            else
            {
#ifdef MOVELOOKCONTROLLER_TRACE
                DebugTrace(L"\tWARNING: OnPointerPressed()  Mouse aleady in use (%d-%s%s) and new event id: %d %s%s",
                    m_mousePointerID,
                    m_mouseLeftInUse ? "L" : "",
                    m_mouseRightInUse ? "R" : "",
                    pointerID,
                    leftButton ? "L" : "",
                    rightButton ? "R" : ""
                    );
#endif
            }
            break;
        }
#ifdef MOVELOOKCONTROLLER_TRACE
        DebugTrace(
            L"\t%s%s%s %s%s%s",
            m_moveInUse ? L"Move " : L"",
            m_lookInUse ? L"Look " : L"",
            m_fireInUse ? L"Fire " : L"",
            m_mouseInUse ? L"Mouse:" : L"",
            m_mouseLeftInUse ? L"L" : L"-",
            m_mouseRightInUse ? L"R" : L"-"
            );
#endif
        break;
    }
#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"\n");
#endif
    return;
}

//----------------------------------------------------------------------

void MoveLookController::OnPointerMoved(
    _In_ CoreWindow^ /* sender */,
    _In_ PointerEventArgs^ args
    )
{
    PointerPoint^ point = args->CurrentPoint;
    uint32 pointerID = point->PointerId;
    Point pointerPosition = point->Position;
    PointerPointProperties^ pointProperties = point->Properties;
    auto pointerDevice = point->PointerDevice;

    XMFLOAT2 position = XMFLOAT2(pointerPosition.X, pointerPosition.Y);     // convert to allow math

#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"%-7s (%d) at (%4.0f, %4.0f)", L"Moved", pointerID, position.x, position.y);
#endif

    switch (m_state)
    {
    case MoveLookControllerState::Active:
        // Decide which control this pointer is operating.
        if (pointerID == m_movePointerID)
        {
            m_movePointerPosition = position;       // Save the current position.
        }
        else if (pointerID == m_lookPointerID)
        {
            // Look control.
            XMFLOAT2 pointerDelta;
            pointerDelta.x = position.x - m_lookLastPoint.x;        // How far did pointer move?
            pointerDelta.y = position.y - m_lookLastPoint.y;

            XMFLOAT2 rotationDelta;
            rotationDelta.x = pointerDelta.x * MoveLookConstants::RotationGain;       // Scale for control sensitivity.
            rotationDelta.y = pointerDelta.y * MoveLookConstants::RotationGain;
            m_lookLastPoint = position;                             // Save for next time through.

#ifdef MOVELOOKCONTROLLER_TRACE
            DebugTrace(L"\tDelta (%4.0f, %4.0f)", pointerDelta.x, pointerDelta.y);
#endif

            // Update our orientation based on the command.
            m_pitch -= rotationDelta.y;
            m_yaw   += rotationDelta.x;

            // Limit pitch to straight up or straight down.
            float limit = XM_PI / 2.0f - 0.01f;
            m_pitch = __max(-limit, m_pitch);
            m_pitch = __min(+limit, m_pitch);

            // Keep longitude in sane range by wrapping.
            if (m_yaw >  XM_PI)
            {
                m_yaw -= XM_PI * 2.0f;
            }
            else if (m_yaw < -XM_PI)
            {
                m_yaw += XM_PI * 2.0f;
            }
        }
        else if (pointerID == m_firePointerID)
        {
            m_fireLastPoint = position;
        }
        else if (pointerID == m_mousePointerID)
        {
            m_mouseLeftInUse  = pointProperties->IsLeftButtonPressed;
            m_mouseRightInUse = pointProperties->IsRightButtonPressed;
            m_mouseLastPoint = position;                            // Save for next time through.
        }

        // Mouse movement is handled via a separate relative mouse movement handler (OnMouseMoved).

        break;
    }
#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"\n");
#endif
}

//----------------------------------------------------------------------

void MoveLookController::OnMouseMoved(
    _In_ MouseDevice^ /* mouseDevice */,
    _In_ MouseEventArgs^ args
    )
{
    // Handle Mouse Input via dedicated relative movement handler.

    switch (m_state)
    {
    case MoveLookControllerState::Active:
        XMFLOAT2 mouseDelta;
        mouseDelta.x = static_cast<float>(args->MouseDelta.X);
        mouseDelta.y = static_cast<float>(args->MouseDelta.Y);

        XMFLOAT2 rotationDelta;
        rotationDelta.x = mouseDelta.x * MoveLookConstants::RotationGain;   // Scale for control sensitivity.
        rotationDelta.y = mouseDelta.y * MoveLookConstants::RotationGain;

        // Update our orientation based on the command.
        m_pitch -= rotationDelta.y;
        m_yaw += rotationDelta.x;

        // Limit pitch to straight up or straight down.
        float limit = XM_PI / 2.0f - 0.01f;
        m_pitch = __max(-limit, m_pitch);
        m_pitch = __min(+limit, m_pitch);

        // Keep longitude in sane range by wrapping.
        if (m_yaw >  XM_PI)
        {
            m_yaw -= XM_PI * 2.0f;
        }
        else if (m_yaw < -XM_PI)
        {
            m_yaw += XM_PI * 2.0f;
        }
        break;
    }
}

//----------------------------------------------------------------------

void MoveLookController::OnPointerReleased(
    _In_ CoreWindow^ /* sender */,
    _In_ PointerEventArgs^ args
    )
{
    PointerPoint^ point = args->CurrentPoint;
    uint32 pointerID = point->PointerId;
    Point pointerPosition = point->Position;
    PointerPointProperties^ pointProperties = point->Properties;

    XMFLOAT2 position = XMFLOAT2(pointerPosition.X, pointerPosition.Y);

#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"%-7s (%d) at (%4.0f, %4.0f)\n", L"Release", pointerID, position.x, position.y);
#endif

    switch (m_state)
    {
    case MoveLookControllerState::WaitForInput:
        if (m_buttonInUse && (pointerID == m_buttonPointerID))
        {
            m_buttonInUse = false;
            m_buttonPressed = true;
#ifdef MOVELOOKCONTROLLER_TRACE
            DebugTrace(L"\tWaitForInput: ButtonPressed = true\n");
#endif
        }
        break;

    case MoveLookControllerState::Active:
        if (pointerID == m_movePointerID)
        {
            m_velocity = XMFLOAT3(0, 0, 0);      // Stop on release.
            m_moveInUse = false;
            m_movePointerID = 0;
        }
        else if (pointerID == m_lookPointerID)
        {
            m_lookInUse = false;
            m_lookPointerID = 0;
        }
        else if (pointerID == m_firePointerID)
        {
            m_fireInUse = false;
            m_firePointerID = 0;
        }
        else if (pointerID == m_mousePointerID)
        {
            bool rightButton = pointProperties->IsRightButtonPressed;
            bool leftButton = pointProperties->IsLeftButtonPressed;

            m_mouseInUse = false;

            // Don't clear the mouse Pointer ID so that Move events still result in Look changes.
            m_mouseLeftInUse = leftButton;
            m_mouseRightInUse = rightButton;
        }
        break;
    }
}

//----------------------------------------------------------------------

void MoveLookController::OnPointerExited(
    _In_ CoreWindow^ /* sender */,
    _In_ PointerEventArgs^ args
    )
{
    PointerPoint^ point = args->CurrentPoint;
    uint32 pointerID = point->PointerId;
    Point pointerPosition = point->Position;
    PointerPointProperties^ pointProperties = point->Properties;

    XMFLOAT2 position  = XMFLOAT2(pointerPosition.X, pointerPosition.Y);

#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"%-7s (%d) at (%4.0f, %4.0f)\n", L"Exit", pointerID, position.x, position.y);
#endif

    switch (m_state)
    {
    case MoveLookControllerState::WaitForInput:
        if (m_buttonInUse && (pointerID == m_buttonPointerID))
        {
            m_buttonInUse = false;
            m_buttonPressed = false;
#ifdef MOVELOOKCONTROLLER_TRACE
            DebugTrace(L"\tWaitForPress: ButtonPressed = false -- Got Exit Event\n");
#endif
        }
        break;

    case MoveLookControllerState::Active:
        if (pointerID == m_movePointerID)
        {
            m_velocity = XMFLOAT3(0, 0, 0);      // Stop on release.
            m_moveInUse = false;
            m_movePointerID = 0;
        }
        else if (pointerID == m_lookPointerID)
        {
            m_lookInUse = false;
            m_lookPointerID = 0;
        }
        else if (pointerID == m_firePointerID)
        {
            m_fireInUse = false;
            m_firePointerID = 0;
        }
        else if (pointerID == m_mousePointerID)
        {
            m_mouseInUse = false;
            m_mousePointerID = 0;
            m_mouseLeftInUse = false;
            m_mouseRightInUse = false;
        }
        break;
    }
}

//----------------------------------------------------------------------

void MoveLookController::OnKeyDown(
    _In_ CoreWindow^ /* sender */,
    _In_ KeyEventArgs^ args
    )
{
    Windows::System::VirtualKey Key;
    Key = args->VirtualKey;

    // Figure out the command from the keyboard.
    if (Key == VirtualKey::W)
        m_forward = true;
    if (Key == VirtualKey::S)
        m_back = true;
    if (Key == VirtualKey::A)
        m_left = true;
    if (Key == VirtualKey::D)
        m_right = true;
    if (Key == VirtualKey::Space)
        m_up = true;
    if (Key == VirtualKey::X)
        m_down = true;
    if (Key == VirtualKey::P)
        m_pause = true;
}

//----------------------------------------------------------------------

void MoveLookController::OnKeyUp(
    _In_ CoreWindow^ /* sender */,
    _In_ KeyEventArgs^ args
    )
{
    Windows::System::VirtualKey Key;
    Key = args->VirtualKey;

    // Figure out the command from the keyboard.
    if (Key == VirtualKey::W)
        m_forward = false;
    if (Key == VirtualKey::S)
        m_back = false;
    if (Key == VirtualKey::A)
        m_left = false;
    if (Key == VirtualKey::D)
        m_right = false;
    if (Key == VirtualKey::Space)
        m_up = false;
    if (Key == VirtualKey::X)
        m_down = false;
    if (Key == VirtualKey::P)
    {
        if (m_pause)
        {
            // Trigger pause only once on button release.
            m_pausePressed = true;
            m_pause = false;
        }
    }
}

//----------------------------------------------------------------------

void MoveLookController::ResetState()
{
    // Reset the state of the MoveLookController.
    // Disable any active pointer IDs to stop all interaction.
    m_buttonPressed = false;
    m_pausePressed = false;
    m_buttonInUse = false;
    m_moveInUse = false;
    m_lookInUse = false;
    m_fireInUse = false;
    m_mouseInUse = false;
    m_mouseLeftInUse = false;
    m_mouseRightInUse = false;
    m_movePointerID = 0;
    m_lookPointerID = 0;
    m_firePointerID = 0;
    m_mousePointerID = 0;
    m_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_moveCommand = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_forward = false;
    m_back = false;
    m_left = false;
    m_right = false;
    m_up = false;
    m_down = false;
    m_pause = false;
}

//----------------------------------------------------------------------

void MoveLookController::UpdatePollingDevices()
{
    if (m_gamepadsChanged)
    {
        m_gamepadsChanged = false;
        unsigned int index = 0;

        // Capture the list of gamepads so it won't change while we are studying it.
        auto gamepads = Gamepad::Gamepads;

        if (gamepads->Size == 0)
        {
            m_activeGamepad = nullptr;
        }
        // Check if the cached gamepad is still connected.
        else if (!gamepads->IndexOf(m_activeGamepad, &index))
        {
            // MoveLookController is intended to handle input for a single player, so it
            // defaults to the first active gamepad.
            m_activeGamepad = gamepads->GetAt(0);
        }
    }

    if (m_activeGamepad == nullptr)
    {
        return;
    }

    GamepadReading reading = m_activeGamepad->GetCurrentReading();

    switch (m_state)
    {
    case MoveLookControllerState::WaitForInput:
        if ((reading.Buttons & GamepadButtons::Menu) == GamepadButtons::Menu)
        {
            m_gamepadStartButtonInUse = true;
        }
        else if (m_gamepadStartButtonInUse)
        {
            // Trigger once only on button release.
            m_gamepadStartButtonInUse = false;
            m_buttonPressed = true;
        }
        break;

    case MoveLookControllerState::Active:
        if ((reading.Buttons & GamepadButtons::Menu) == GamepadButtons::Menu)
        {
            m_gamepadStartButtonInUse = true;
        }
        else if (m_gamepadStartButtonInUse)
        {
            // Trigger once only on button release.
            m_gamepadStartButtonInUse = false;
            m_pausePressed = true;
        }
        // Use the left thumbstick on the game controller to control
        // the eye point position control. Thumbstick input is defined from [-1, 1].
        // We use a deadzone in the middle range to avoid drift.
        if (reading.LeftThumbstickX > THUMBSTICK_DEADZONE ||
            reading.LeftThumbstickX < -THUMBSTICK_DEADZONE)
        {
            float x = static_cast<float>(reading.LeftThumbstickX);
            m_moveCommand.x -= (x > 0) ? 1 : -1;
        }

        if (reading.LeftThumbstickY > THUMBSTICK_DEADZONE ||
            reading.LeftThumbstickY < -THUMBSTICK_DEADZONE)
        {
            float y = static_cast<float>(reading.LeftThumbstickY);
            m_moveCommand.y += (y > 0) ? 1 : -1;
        }

        // Use the right thumbstick on the game controller to control
        // the look at control. Thumbstick input is defined from [-1, 1].
        // We use a deadzone in the middle range to avoid drift.
        XMFLOAT2 pointerDelta;
        if (reading.RightThumbstickX > THUMBSTICK_DEADZONE ||
            reading.RightThumbstickX < -THUMBSTICK_DEADZONE)
        {
            float x = static_cast<float>(reading.RightThumbstickX);
            pointerDelta.x = x * x * x;
        }
        else
        {
            pointerDelta.x = 0.0f;
        }

        if (reading.RightThumbstickY > THUMBSTICK_DEADZONE ||
            reading.RightThumbstickY < -THUMBSTICK_DEADZONE)
        {
            float y = static_cast<float>(reading.RightThumbstickY);
            pointerDelta.y = y * y * y;
        }
        else
        {
            pointerDelta.y = 0.0f;
        }

        XMFLOAT2 rotationDelta;
        rotationDelta.x = pointerDelta.x *  0.08f;       // Scale for control sensitivity.
        rotationDelta.y = pointerDelta.y *  0.08f;

        // Update our orientation based on the command.
        m_pitch += rotationDelta.y;
        m_yaw += rotationDelta.x;

        // Limit pitch to straight up or straight down.
        m_pitch = __max(-XM_PI / 2.0f, m_pitch);
        m_pitch = __min(+XM_PI / 2.0f, m_pitch);

        // Check the state of the Right Trigger button.  This is used to indicate fire control.

        if (reading.RightTrigger > TRIGGER_DEADZONE)
        {
            if (!m_autoFire && !m_gamepadTriggerInUse)
            {
                m_firePressed = true;
            }

            m_gamepadTriggerInUse = true;
        }
        else
        {
            m_gamepadTriggerInUse = false;
        }
        break;
    }
}

//----------------------------------------------------------------------
// Turn on mouse cursor, which also disables relative mouse movement tracking.
void MoveLookController::ShowCursor()
{
    if (m_dispatcher != nullptr)
    {
        // Use the CoreDispatcher to ensure that the pointer is updated on the UI thread.
        // This is needed for XAML updates in a XAML app.
        m_dispatcher->RunAsync(
            CoreDispatcherPriority::Normal,
            ref new DispatchedHandler([this]()
            {
                CoreWindow::GetForCurrentThread()->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
            })
            );
    }
    else
    {
        auto window = CoreWindow::GetForCurrentThread();
        if (window)
        {
            // Protect case where there isn't a window associated with the current thread.
            // This happens on initialization or when being called from a background thread.
            window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
        }
    }
}

//----------------------------------------------------------------------

// Turn mouse cursor off (hidden), which also enables relative mouse movement tracking.
void MoveLookController::HideCursor()
{
    if (m_dispatcher != nullptr)
    {
        // Use the CoreDispatcher to ensure that the pointer is updated on the UI thread.
        // This is needed for XAML updates in a XAML app.
        m_dispatcher->RunAsync(
            CoreDispatcherPriority::Normal,
            ref new DispatchedHandler([this]()
            {
                CoreWindow::GetForCurrentThread()->PointerCursor = nullptr;
            })
            );
    }
    else
    {
        auto window = CoreWindow::GetForCurrentThread();
        if (window)
        {
            // Protect case where there isn't a window associated with the current thread.
            // This happens on initialization or when being called from a background thread.
            window->PointerCursor = nullptr;
        }
    }
}

//----------------------------------------------------------------------

void MoveLookController::SetMoveRect (
    _In_ XMFLOAT2 upperLeft,
    _In_ XMFLOAT2 lowerRight
    )
{
#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"MoveRect (%d, %d) to (%d, %d)", upperLeft.x, upperLeft.y, lowerRight.x, lowerRight.y);
#endif
    m_moveUpperLeft  = upperLeft;
    m_moveLowerRight = lowerRight;
}

//----------------------------------------------------------------------

void MoveLookController::SetFireRect (
    _In_ XMFLOAT2 upperLeft,
    _In_ XMFLOAT2 lowerRight
    )
{
#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"FireRect (%d, %d) to (%d, %d)", upperLeft.x, upperLeft.y, lowerRight.x, lowerRight.y);
#endif
    m_fireUpperLeft  = upperLeft;
    m_fireLowerRight = lowerRight;
}

//----------------------------------------------------------------------

void MoveLookController::WaitForPress(
    _In_ XMFLOAT2 upperLeft,
    _In_ XMFLOAT2 lowerRight
    )
{
#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"WaitForPress (%.1f, %.1f) to (%.1f, %.1f)\n", upperLeft.x, upperLeft.y, lowerRight.x, lowerRight.y);
#endif

    ResetState();
    m_state = MoveLookControllerState::WaitForInput;
    m_buttonUpperLeft  = upperLeft;
    m_buttonLowerRight = lowerRight;
    ShowCursor();
}

//----------------------------------------------------------------------

void MoveLookController::WaitForPress()
{
#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"WaitForPress (null rect)\n");
#endif

    ResetState();
    m_state = MoveLookControllerState::WaitForInput;
    m_buttonUpperLeft.x = 0.0f;
    m_buttonUpperLeft.y = 0.0f;
    m_buttonLowerRight.x = 0.0f;
    m_buttonLowerRight.y = 0.0f;
    ShowCursor();
}

//----------------------------------------------------------------------

XMFLOAT3 MoveLookController::Velocity()
{
    return m_velocity;
}

//----------------------------------------------------------------------

XMFLOAT3 MoveLookController::LookDirection()
{
    XMFLOAT3 lookDirection;

    float r = cosf(m_pitch);              // In the plane.
    lookDirection.y = sinf(m_pitch);      // Vertical.
    lookDirection.z = r * cosf(m_yaw);    // Fwd-back.
    lookDirection.x = r * sinf(m_yaw);    // Left-right.

    return lookDirection;
}

//----------------------------------------------------------------------

float MoveLookController::Pitch()
{
    return m_pitch;
}

//----------------------------------------------------------------------

void MoveLookController::Pitch(_In_ float pitch)
{
    m_pitch = pitch;
}

//----------------------------------------------------------------------

float MoveLookController::Yaw()
{
    return m_yaw;
}

//----------------------------------------------------------------------

void MoveLookController::Yaw(_In_ float yaw)
{
    m_yaw = yaw;
}

//----------------------------------------------------------------------

void MoveLookController::Active(_In_ bool active)
{
    ResetState();
#ifdef MOVELOOKCONTROLLER_TRACE
    DebugTrace(L"Active = %s\n", active ? L"true" : L"false");
#endif

    if (active)
    {
        m_state = MoveLookControllerState::Active;
        HideCursor();

    }
    else
    {
        m_state = MoveLookControllerState::None;
        ShowCursor();
    }
}

//----------------------------------------------------------------------

bool MoveLookController::Active()
{
    if (m_state == MoveLookControllerState::Active)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//----------------------------------------------------------------------

void MoveLookController::AutoFire(_In_ bool autoFire)
{
    m_autoFire = autoFire;
}

//----------------------------------------------------------------------

bool MoveLookController::AutoFire()
{
    return m_autoFire;
}

//----------------------------------------------------------------------

void MoveLookController::Update()
{
    UpdatePollingDevices();

    if (m_moveInUse)
    {
        // Move control.
        XMFLOAT2 pointerDelta;

        pointerDelta.x = m_movePointerPosition.x - m_moveFirstDown.x;
        pointerDelta.y = m_movePointerPosition.y - m_moveFirstDown.y;

        // Figure out the command from the virtual joystick.
        XMFLOAT3 commandDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);
        if (fabsf(pointerDelta.x) > 16.0f)         // Leave 32 pixel-wide dead spot for being still.
            m_moveCommand.x -= pointerDelta.x/fabsf(pointerDelta.x);

        if (fabsf(pointerDelta.y) > 16.0f)
            m_moveCommand.y -= pointerDelta.y/fabsf(pointerDelta.y);
    }

    // Poll our state bits set by the keyboard input events.
    if (m_forward)
    {
        m_moveCommand.y += 1.0f;
    }
    if (m_back)
    {
        m_moveCommand.y -= 1.0f;
    }
    if (m_left)
    {
        m_moveCommand.x += 1.0f;
    }
    if (m_right)
    {
        m_moveCommand.x -= 1.0f;
    }
    if (m_up)
    {
        m_moveCommand.z += 1.0f;
    }
    if (m_down)
    {
        m_moveCommand.z -= 1.0f;
    }

    // Make sure that 45deg cases are not faster.
    if (fabsf(m_moveCommand.x) > 0.1f ||
        fabsf(m_moveCommand.y) > 0.1f ||
        fabsf(m_moveCommand.z) > 0.1f)
    {
        XMStoreFloat3(&m_moveCommand, XMVector3Normalize(XMLoadFloat3(&m_moveCommand)));
    }

    // Rotate command to align with our direction (world coordinates).
    XMFLOAT3 wCommand;
    wCommand.x =  m_moveCommand.x * cosf(m_yaw) - m_moveCommand.y * sinf(m_yaw);
    wCommand.y =  m_moveCommand.x * sinf(m_yaw) + m_moveCommand.y * cosf(m_yaw);
    wCommand.z =  m_moveCommand.z;

    // Scale for sensitivity adjustment.
    // Our velocity is based on the command. Y is up.
    m_velocity.x = -wCommand.x * MoveLookConstants::MovementGain;
    m_velocity.z =  wCommand.y * MoveLookConstants::MovementGain;
    m_velocity.y =  wCommand.z * MoveLookConstants::MovementGain;

    // Clear movement input accumulator for use during next frame.
    m_moveCommand = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

//----------------------------------------------------------------------

void MoveLookController::OnBackRequested(
    _In_ Platform::Object^ sender,
    _In_ BackRequestedEventArgs^ args
    )
{
    if (m_state == MoveLookControllerState::Active)
    {
        // The game is currently in active play mode, so hitting the hardware back button
        // will cause the game to pause.
        m_pausePressed = true;
        args->Handled = true;
    }
    else
    {
        // The game is not currently in active play mode, so take the default behavior
        // for the hardware back button.
        args->Handled = false;
    }
}

//----------------------------------------------------------------------

void MoveLookController::OnGamepadAdded(_In_ Object^ sender, _In_ Gamepad^ gamepad)
{
    // OnGamepadAdded and OnGamepadRemoved can be called from a worker thread. For simplicity,
    // defer updating the active gamepad until Update().
    m_gamepadsChanged = true;
}

//----------------------------------------------------------------------

void MoveLookController::OnGamepadRemoved(_In_ Object^ sender, _In_ Gamepad^ gamepad)
{
    // OnGamepadAdded and OnGamepadRemoved can be called from a worker thread. For simplicity,
    // defer updating the active gamepad until Update().
    m_gamepadsChanged = true;
}

#ifdef MOVELOOKCONTROLLER_TRACE
void MoveLookController::DebugTrace(const wchar_t *format, ...)
{
    // Generate the message string.
    va_list args;
    va_start(args, format);
    wchar_t message[1024];
    vswprintf_s(message, 1024, format, args);
    OutputDebugStringW(message);
}
#endif

//----------------------------------------------------------------------
