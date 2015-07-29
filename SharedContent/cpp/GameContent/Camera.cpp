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
#include "Camera.h"
#include "StereoProjection.h"

using namespace DirectX;

#undef min // Use __min instead.
#undef max // Use __max instead.

//--------------------------------------------------------------------------------------

Camera::Camera()
{
    // Setup the view matrix.
    SetViewParams(
        XMFLOAT3(0.0f, 0.0f, 0.0f),   // Default eye position.
        XMFLOAT3(0.0f, 0.0f, 1.0f),   // Default look at position.
        XMFLOAT3(0.0f, 1.0f, 0.0f)    // Default up vector.
        );

    // Setup the projection matrix.
    SetProjParams(XM_PI / 4, 1.0f, 1.0f, 1000.0f);
}

//--------------------------------------------------------------------------------------

void Camera::LookDirection (_In_ XMFLOAT3 lookDirection)
{
    XMFLOAT3 lookAt;
    lookAt.x = m_eye.x + lookDirection.x;
    lookAt.y = m_eye.y + lookDirection.y;
    lookAt.z = m_eye.z + lookDirection.z;

    SetViewParams(m_eye, lookAt, m_up);
}

//--------------------------------------------------------------------------------------

void Camera::Eye(_In_ XMFLOAT3 eye)
{
    SetViewParams(eye, m_lookAt, m_up);
}

//--------------------------------------------------------------------------------------

void Camera::SetViewParams(
    _In_ XMFLOAT3 eye,
    _In_ XMFLOAT3 lookAt,
    _In_ XMFLOAT3 up
    )
{
    m_eye = eye;
    m_lookAt = lookAt;
    m_up = up;

    // Calculate the view matrix.
    XMMATRIX view = XMMatrixLookAtLH(
        XMLoadFloat3(&m_eye),
        XMLoadFloat3(&m_lookAt),
        XMLoadFloat3(&m_up)
        );

    XMVECTOR det;
    XMMATRIX inverseView = XMMatrixInverse(&det, view);
    XMStoreFloat4x4(&m_viewMatrix, view);
    XMStoreFloat4x4(&m_inverseView, inverseView);

    // The axis basis vectors and camera position are stored inside the
    // position matrix in the 4 rows of the camera's world matrix.
    // To figure out the yaw/pitch of the camera, we just need the Z basis vector.
    XMFLOAT3 zBasis;
    XMStoreFloat3(&zBasis, inverseView.r[2]);

    m_cameraYawAngle = atan2f(zBasis.x, zBasis.z);

    float len = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
    m_cameraPitchAngle = atan2f(zBasis.y, len);
}

//--------------------------------------------------------------------------------------

void Camera::SetProjParams(
    _In_ float fieldOfView,
    _In_ float aspectRatio,
    _In_ float nearPlane,
    _In_ float farPlane
    )
{
    // Set attributes for the projection matrix.
    m_fieldOfView = fieldOfView;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    XMStoreFloat4x4(
        &m_projectionMatrix,
        XMMatrixPerspectiveFovLH(
            m_fieldOfView,
            m_aspectRatio,
            m_nearPlane,
            m_farPlane
            )
        );

    STEREO_PARAMETERS* stereoParams = nullptr;
    // Update the projection matrix.
    XMStoreFloat4x4(
        &m_projectionMatrixLeft,
        MatrixStereoProjectionFovLH(
            stereoParams,
            STEREO_CHANNEL::LEFT,
            m_fieldOfView,
            m_aspectRatio,
            m_nearPlane,
            m_farPlane,
            STEREO_MODE::NORMAL
            )
        );

    XMStoreFloat4x4(
        &m_projectionMatrixRight,
        MatrixStereoProjectionFovLH(
            stereoParams,
            STEREO_CHANNEL::RIGHT,
            m_fieldOfView,
            m_aspectRatio,
            m_nearPlane,
            m_farPlane,
            STEREO_MODE::NORMAL
            )
        );
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::View()
{
    return XMLoadFloat4x4(&m_viewMatrix);
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::Projection()
{
    return XMLoadFloat4x4(&m_projectionMatrix);
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::LeftEyeProjection()
{
    return XMLoadFloat4x4(&m_projectionMatrixLeft);
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::RightEyeProjection()
{
    return XMLoadFloat4x4(&m_projectionMatrixRight);
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::World()
{
    return XMLoadFloat4x4(&m_inverseView);
}

//--------------------------------------------------------------------------------------

DirectX::XMFLOAT3 Camera::Eye()
{
    return m_eye;
}

//--------------------------------------------------------------------------------------

DirectX::XMFLOAT3 Camera::LookAt()
{
    return m_lookAt;
}

//--------------------------------------------------------------------------------------

float Camera::NearClipPlane()
{
    return m_nearPlane;
}

//--------------------------------------------------------------------------------------

float Camera::FarClipPlane()
{
    return m_farPlane;
}

//--------------------------------------------------------------------------------------

float Camera::Pitch()
{
    return m_cameraPitchAngle;
}

//--------------------------------------------------------------------------------------

float Camera::Yaw()
{
    return m_cameraYawAngle;
}

//--------------------------------------------------------------------------------------