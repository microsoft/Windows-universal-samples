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

// FaceMesh:
// This class derives from MeshObject and creates a ID3D11Buffer of
// vertices and indices to represent a canonical face defined as a
// rectangle at the origin extending 1 unit in the +X and
// 1 unit in the +Y direction.
// The face is defined to be two sided, so it is visible from either
// side.

#include "MeshObject.h"

ref class FaceMesh: public MeshObject
{
internal:
    FaceMesh(_In_ ID3D11Device *device);
};
