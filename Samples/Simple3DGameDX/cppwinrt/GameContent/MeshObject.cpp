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
#include "MeshObject.h"
#include "ConstantBuffers.h"

MeshObject::MeshObject() :
    m_vertexCount(0),
    m_indexCount(0)
{
}

void MeshObject::Render(_In_ ID3D11DeviceContext* context)
{
    uint32_t stride{ sizeof(PNTVertex) };
    uint32_t offset{ 0 };

    ID3D11Buffer* vertexBuffer{ m_vertexBuffer.get() };
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->DrawIndexed(m_indexCount, 0, 0);
}