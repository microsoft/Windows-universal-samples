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
#include "Cylinder.h"

using namespace DirectX;

//----------------------------------------------------------------------

Cylinder::Cylinder()
{
    Initialize(XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, XMFLOAT3(0.0f, 0.0f, 1.0f));
}

//----------------------------------------------------------------------

Cylinder::Cylinder(
    XMFLOAT3 position,
    float radius,
    XMFLOAT3 direction
    )
{
    Initialize(position, radius, direction);
}

//----------------------------------------------------------------------

void Cylinder::Initialize(
    XMFLOAT3 position,
    float radius,
    XMFLOAT3 direction
    )
{
    m_position = position;
    m_radius = radius;
    m_length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&direction)));
    XMStoreFloat3(&m_axis, XMVector3Normalize(XMLoadFloat3(&direction)));

    XMVECTOR up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
    float angle1 = XMVectorGetX(
        XMVector3AngleBetweenVectors(up, XMLoadFloat3(&m_axis))
        );

    XMMATRIX mat1 = XMMatrixIdentity();

    if ((angle1 * angle1) > 0.025)
    {
        XMVECTOR axis1 = XMVector3Cross(up, XMLoadFloat3(&m_axis));

        mat1 = XMMatrixRotationAxis(axis1, angle1);
    }
    XMStoreFloat4x4(&m_rotationMatrix, mat1);

    XMStoreFloat4x4(
        &m_modelMatrix,
        XMMatrixScaling(m_radius, m_radius, m_length) *
        mat1 *
        XMMatrixTranslation(m_position.x, m_position.y, m_position.z)
        );
}

//--------------------------------------------------------------------------------

void Cylinder::UpdatePosition()
{
    XMStoreFloat4x4(
        &m_modelMatrix,
        XMMatrixScaling(m_radius, m_radius, m_length) *
        XMLoadFloat4x4(&m_rotationMatrix) *
        XMMatrixTranslation(m_position.x, m_position.y, m_position.z)
        );
}

//--------------------------------------------------------------------------------

bool Cylinder::IsTouching(
    XMFLOAT3 point,
    float radius,
    _Out_ XMFLOAT3 *contact,
    _Out_ XMFLOAT3 *normal
    )
{
    XMVECTOR p0;
    XMVECTOR axis;
    XMVECTOR p1;
    XMVECTOR px;

    // Determine if a point is within radius distance of the cylinder and
    // return the point of contact (projection of the point onto the cylinder).

    p0 = XMLoadFloat3(&m_position);
    axis = XMLoadFloat3(&m_axis);
    p1 = p0 + axis * m_length;
    px = XMLoadFloat3(&point);

    float dist = XMVectorGetX(
        XMVector3LinePointDistance(p0, p1, px)
        );

    // Projection of the point onto the Vector.
    float dist2 = XMVectorGetX(
        XMVector3Dot(px - p0, axis)
        );

    XMVECTOR ptOnVector =  p0 + dist2 * axis;

    XMVECTOR normalVector = XMVector3Normalize(px - ptOnVector);

    XMStoreFloat3(normal, normalVector);
    XMStoreFloat3(contact, ptOnVector + normalVector * m_radius);

    if (dist < 0.0f)
    {
        dist *= -1.0f;
    }

    if (dist < (radius + m_radius))
    {
        if (dist2 >= 0.0f && dist2 <= m_length)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

//--------------------------------------------------------------------------------
