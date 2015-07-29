//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "BasicMath.h"

// Defines the vertex format for the shapes generated in the functions below.
struct BasicVertex
{
    float3 pos;  // position
    float3 norm; // surface normal vector
    float2 tex;  // texture coordinate
};

// Defines the vertex format for all shapes generated in the functions below.
struct TangentVertex
{
    float3 pos;  // position
    float2 tex;  // texture coordinate
    float3 uTan; // texture coordinate u-tangent vector
    float3 vTan; // texture coordinate v-tangent vector
};

// A helper class that provides convenient functions for creating common
// geometrical shapes used by DirectX SDK samples.
ref class BasicShapes
{
internal:
    BasicShapes(ID3D11Device *d3dDevice);
    void CreateCube(
        _Out_ ID3D11Buffer **vertexBuffer,
        _Out_ ID3D11Buffer **indexBuffer,
        _Out_opt_ unsigned int *vertexCount,
        _Out_opt_ unsigned int *indexCount
        );
    void CreateBox(
        float3 radii,
        _Out_ ID3D11Buffer **vertexBuffer,
        _Out_ ID3D11Buffer **indexBuffer,
        _Out_opt_ unsigned int *vertexCount,
        _Out_opt_ unsigned int *indexCount
        );
    void CreateSphere(
        _Out_ ID3D11Buffer **vertexBuffer,
        _Out_ ID3D11Buffer **indexBuffer,
        _Out_opt_ unsigned int *vertexCount,
        _Out_opt_ unsigned int *indexCount
        );
    void CreateTangentSphere(
        _Out_ ID3D11Buffer **vertexBuffer,
        _Out_ ID3D11Buffer **indexBuffer,
        _Out_opt_ unsigned int *vertexCount,
        _Out_opt_ unsigned int *indexCount
        );
    void CreateReferenceAxis(
        _Out_ ID3D11Buffer **vertexBuffer,
        _Out_ ID3D11Buffer **indexBuffer,
        _Out_opt_ unsigned int *vertexCount,
        _Out_opt_ unsigned int *indexCount
        );

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;

    void CreateVertexBuffer(
        _In_ unsigned int numVertices,
        _In_ BasicVertex *vertexData,
        _Out_ ID3D11Buffer **vertexBuffer
        );

    void CreateIndexBuffer(
        _In_ unsigned int numIndices,
        _In_ unsigned short *indexData,
        _Out_ ID3D11Buffer **indexBuffer
        );

    void CreateTangentVertexBuffer(
        _In_ unsigned int numVertices,
        _In_ TangentVertex *vertexData,
        _Out_ ID3D11Buffer **vertexBuffer
        );

};
