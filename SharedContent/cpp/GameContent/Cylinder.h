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

// Cylinder:
// This class is a specialization of GameObject that represents a cylinder primitive.
// The cylinder is defined by a vector starting at 'position' and oriented along the
// 'direction' vector.  The length of the cylinder is just the length of the 'direction'
// vector.

#include "GameObject.h"

ref class Cylinder: public GameObject
{
internal:
    Cylinder();
    Cylinder(
        DirectX::XMFLOAT3 position,
        float radius,
        DirectX::XMFLOAT3 direction
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
    void Initialize(
        DirectX::XMFLOAT3 position,
        float radius,
        DirectX::XMFLOAT3 direction
        );

    DirectX::XMFLOAT3   m_axis;
    float               m_length;
    float               m_radius;
    DirectX::XMFLOAT4X4 m_rotationMatrix;
};