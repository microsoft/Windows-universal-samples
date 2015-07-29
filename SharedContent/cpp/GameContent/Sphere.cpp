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
#include "Sphere.h"

using namespace DirectX;

//----------------------------------------------------------------------

Sphere::Sphere()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_radius = 1.0f;
    Update();
}

//----------------------------------------------------------------------

Sphere::Sphere(
    DirectX::XMFLOAT3 position,
    float radius
    )
{
    m_position = position;
    m_radius = radius;
    Update();
}

//----------------------------------------------------------------------

void Sphere::Update()
{
    XMStoreFloat4x4(
        &m_modelMatrix,
        XMMatrixScaling(m_radius, m_radius, m_radius) *
        XMMatrixTranslation(m_position.x, m_position.y, m_position.z)
        );
}

//----------------------------------------------------------------------

bool Sphere::IsTouching(
    XMFLOAT3 point,
    float radius,
    _Out_ XMFLOAT3 *contact,
    _Out_ XMFLOAT3 *normal
    )
{
    // Check collision between instances One and Two.
    // oneToTwo is the collision normal vector.
    XMVECTOR oneToTwo = XMLoadFloat3(&m_position) - XMLoadFloat3(&point);

    float distance = XMVectorGetX(XMVector3Length(oneToTwo));

    oneToTwo = XMVector3Normalize(oneToTwo);
    XMStoreFloat3(normal, oneToTwo);
    XMStoreFloat3(contact, oneToTwo * m_radius);

    if (distance < 0.0f)
    {
        distance *= -1.0f;
    }

    if (distance < (radius + m_radius))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//----------------------------------------------------------------------
