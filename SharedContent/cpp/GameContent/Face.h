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

// Face:
// This class is a specialization of GameObject that represents a parallelogram primitive.
// The face is defined by three points.  It is positioned at 'origin'.  The four corners
// of the face are defined at 'origin', 'p1', 'p2' and 'p1' + ('p2' - 'origin').

#include "GameObject.h"

ref class Face: public GameObject
{
internal:
    Face();
    Face(
        DirectX::XMFLOAT3 origin,
        DirectX::XMFLOAT3 p1,
        DirectX::XMFLOAT3 p2
        );

    void SetPlane(
        DirectX::XMFLOAT3 origin,
        DirectX::XMFLOAT3 p1,
        DirectX::XMFLOAT3 p2
        );

    virtual bool IsTouching(
        DirectX::XMFLOAT3 point,
        float radius,
        _Out_ DirectX::XMFLOAT3 *contact,
        _Out_ DirectX::XMFLOAT3 *normal
        ) override;

protected:
    virtual void UpdatePosition() override;

private:
    void UpdateMatrix();

    DirectX::XMFLOAT3   m_widthVector;
    DirectX::XMFLOAT3   m_heightVector;
    DirectX::XMFLOAT3   m_normal;
    DirectX::XMFLOAT3   m_point[4];
    float               m_width;
    float               m_height;
    DirectX::XMFLOAT4X4 m_rotationMatrix;
};
