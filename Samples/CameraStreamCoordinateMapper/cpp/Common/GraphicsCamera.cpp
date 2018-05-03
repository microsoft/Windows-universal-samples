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
#include "Common\GraphicsCamera.h"

using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Core;

FirstPersonCamera::FirstPersonCamera()
{
    CoreWindow::GetForCurrentThread()->PointerMoved +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &FirstPersonCamera::OnPointerMoved);
}

void FirstPersonCamera::OnPointerMoved(
    CoreWindow^ window, 
    PointerEventArgs^ args)
{
    if (args->CurrentPoint->PointerDevice->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Mouse)
    {
        const Point currCursor = args->CurrentPoint->Position;

        // We should ignore the first mouse event as we have no previous to compare against.
        const bool validPrevious = !std::isnan(m_prevCursor.X) && !std::isnan(m_prevCursor.Y);

        ////////////////////////////////////////////////////////////////////////////////
        // Rotation by Mouse
        if (args->CurrentPoint->Properties->IsLeftButtonPressed && validPrevious)
        {
            const float deltaX = currCursor.X - m_prevCursor.X;
            const float deltaY = currCursor.Y - m_prevCursor.Y;

            RotateYaw(deltaX * cPixelsToRadiansPerSecond);
            RotatePitch(deltaY * cPixelsToRadiansPerSecond);
        }

        m_prevCursor = currCursor;
    }
}

void FirstPersonCamera::Update(float dt)
{
    auto isKeyHeldDown = [](VirtualKey key) {
        return (CoreWindow::GetForCurrentThread()->GetKeyState(key) & CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Movement by Gamepad or Keyboard

    if (isKeyHeldDown(VirtualKey::W) ||
        isKeyHeldDown(VirtualKey::GamepadLeftThumbstickUp))
    {
        MoveAlongZ(-cMovementMetersPerSecond * dt);
    }

    if (isKeyHeldDown(VirtualKey::S) ||
        isKeyHeldDown(VirtualKey::GamepadLeftThumbstickDown))
    {
        MoveAlongZ(+cMovementMetersPerSecond * dt);
    }

    if (isKeyHeldDown(VirtualKey::A) ||
        isKeyHeldDown(VirtualKey::GamepadLeftThumbstickLeft))
    {
        MoveAlongX(-cMovementMetersPerSecond * dt);
    }

    if (isKeyHeldDown(VirtualKey::D) ||
        isKeyHeldDown(VirtualKey::GamepadLeftThumbstickRight))
    {
        MoveAlongX(+cMovementMetersPerSecond * dt);
    }

    if (isKeyHeldDown(VirtualKey::Q) ||
        isKeyHeldDown(VirtualKey::GamepadLeftTrigger))
    {
        MoveAlongY(-cMovementMetersPerSecond * dt);
    }

    if (isKeyHeldDown(VirtualKey::E) ||
        isKeyHeldDown(VirtualKey::GamepadRightTrigger))
    {
        MoveAlongY(+cMovementMetersPerSecond * dt);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Rotation by Gamepad

    if (isKeyHeldDown(VirtualKey::GamepadRightThumbstickLeft))
    {
        RotateYaw(-cRotationRadiansPerSecond * dt);
    }

    if (isKeyHeldDown(VirtualKey::GamepadRightThumbstickRight))
    {
        RotateYaw(+cRotationRadiansPerSecond * dt);
    }

    if (isKeyHeldDown(VirtualKey::GamepadRightThumbstickUp))
    {
        RotatePitch(-cRotationRadiansPerSecond * dt);
    }

    if (isKeyHeldDown(VirtualKey::GamepadRightThumbstickDown))
    {
        RotatePitch(+cRotationRadiansPerSecond * dt);
    }
}

void FirstPersonCamera::Reset()
{
    m_position = Windows::Foundation::Numerics::float3::zero();
    m_prevCursor = {
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::quiet_NaN()
    };

    m_yawAngle = 0.0f;
    m_pitchAngle = 0.0f;
}

float4x4 FirstPersonCamera::GetWorldToCamera()
{
    return make_float4x4_translation(-m_position) * make_float4x4_rotation_y(m_yawAngle) * make_float4x4_rotation_x(m_pitchAngle);
}

float4x4 FirstPersonCamera::GetCameraToWorld()
{
    return make_float4x4_rotation_x(-m_pitchAngle) * make_float4x4_rotation_y(-m_yawAngle) * make_float4x4_translation(m_position);
}

void FirstPersonCamera::RotateYaw(float radians)
{
    m_yawAngle += radians;
}

void FirstPersonCamera::RotatePitch(float radians)
{
    m_pitchAngle += radians;
}

void FirstPersonCamera::MoveAlongX(float distance)
{
    const float4x4 cameraToWorld = GetCameraToWorld();
    const float3 rightVec = { cameraToWorld.m11, cameraToWorld.m12, cameraToWorld.m13 };
    m_position += rightVec * distance;
}

void FirstPersonCamera::MoveAlongY(float distance)
{
    const float4x4 cameraToWorld = GetCameraToWorld();
    const float3 upVec = { cameraToWorld.m21, cameraToWorld.m22, cameraToWorld.m23 };
    m_position += upVec * distance;
}

void FirstPersonCamera::MoveAlongZ(float distance)
{
    const float4x4 cameraToWorld = GetCameraToWorld();
    const float3 forwardVec = { cameraToWorld.m31, cameraToWorld.m32, cameraToWorld.m33 };
    m_position += forwardVec * distance;
}
