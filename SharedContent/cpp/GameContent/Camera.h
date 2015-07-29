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

// Camera:
// This class defines the position, orientation and viewing frustum of a camera looking into
// a 3D world.  It will generate both the View matrix and Projection matrix.  It can also
// provide a pair of Projection matrices to be used when stereoscopic 3D is used.

ref class Camera
{
internal:
    Camera();

    void SetViewParams(_In_ DirectX::XMFLOAT3 eye, _In_ DirectX::XMFLOAT3 lookAt, _In_ DirectX::XMFLOAT3 up);
    void SetProjParams(_In_ float fieldOfView, _In_ float aspectRatio, _In_ float nearPlane, _In_ float farPlane);

    void LookDirection (_In_ DirectX::XMFLOAT3 lookDirection);
    void Eye (_In_ DirectX::XMFLOAT3 position);

    DirectX::XMMATRIX View();
    DirectX::XMMATRIX Projection();
    DirectX::XMMATRIX LeftEyeProjection();
    DirectX::XMMATRIX RightEyeProjection();
    DirectX::XMMATRIX World();
    DirectX::XMFLOAT3 Eye();
    DirectX::XMFLOAT3 LookAt();
    DirectX::XMFLOAT3 Up();
    float NearClipPlane();
    float FarClipPlane();
    float Pitch();
    float Yaw();

protected private:
    DirectX::XMFLOAT4X4 m_viewMatrix;
    DirectX::XMFLOAT4X4 m_projectionMatrix;
    DirectX::XMFLOAT4X4 m_projectionMatrixLeft;
    DirectX::XMFLOAT4X4 m_projectionMatrixRight;

    DirectX::XMFLOAT4X4 m_inverseView;

    DirectX::XMFLOAT3 m_eye;
    DirectX::XMFLOAT3 m_lookAt;
    DirectX::XMFLOAT3 m_up;
    float             m_cameraYawAngle;
    float             m_cameraPitchAngle;

    float             m_fieldOfView;
    float             m_aspectRatio;
    float             m_nearPlane;
    float             m_farPlane;
};
