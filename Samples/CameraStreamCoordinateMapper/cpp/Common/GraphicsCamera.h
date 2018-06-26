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

#pragma once

ref class FirstPersonCamera sealed
{
public:
    FirstPersonCamera();

    void Update(float dt);
    void Reset();

    Windows::Foundation::Numerics::float4x4 GetWorldToCamera();
    Windows::Foundation::Numerics::float4x4 GetCameraToWorld();

private:
    void OnPointerMoved(
        Windows::UI::Core::CoreWindow^ window,
        Windows::UI::Core::PointerEventArgs^ args);

    void RotateYaw(float radians);
    void RotatePitch(float radians);

    void MoveAlongX(float distance);
    void MoveAlongY(float distance);
    void MoveAlongZ(float distance);

    // Parameters for configuring the camera movement speed
    static constexpr float cPixelsToRadians = 0.00125f;
    static constexpr float cPixelsToMeters = 0.005f;
    static constexpr float cMovementMetersPerSecond = 1.0f;
    static constexpr float cRotationRadiansPerSecond = 1.0f;
    static constexpr float cPixelsToRadiansPerSecond = cPixelsToRadians * cRotationRadiansPerSecond;
    static constexpr float cPixelsToMetersPerSecond = cPixelsToMeters * cMovementMetersPerSecond;

    Windows::Foundation::Numerics::float3 m_position;
    Windows::Foundation::Point m_prevCursor;

    float m_yawAngle;
    float m_pitchAngle;
};
