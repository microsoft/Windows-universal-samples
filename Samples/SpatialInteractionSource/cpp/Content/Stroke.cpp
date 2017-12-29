#include "pch.h"
#include "stroke.h"
#include "Common\DirectXHelper.h"

using namespace HolographicPaint;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;


Stroke::Stroke(Windows::UI::Color color)
{
    m_modelConstantBufferData.color = DX::GetDXColor(color);
    XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixIdentity());
}

void Stroke::AddPosition(float3 position, quaternion orientation, float diameter)
{
    const XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
    const XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadQuaternion(&orientation));
    const XMMATRIX transformationMatrix = rotation * translation;
    Square square;
    const float radius = diameter / 2.0f;
    XMFLOAT3 pos(-radius, 0.0f, radius);
    XMStoreFloat3(&square.TopLeft, XMVector3TransformCoord(XMLoadFloat3(&pos), transformationMatrix));
    pos.x = radius;
    XMStoreFloat3(&square.TopRight, XMVector3TransformCoord(XMLoadFloat3(&pos), transformationMatrix));
    pos.z = -radius;
    XMStoreFloat3(&square.BottomRight, XMVector3TransformCoord(XMLoadFloat3(&pos), transformationMatrix));
    pos.x = -radius;
    XMStoreFloat3(&square.BottomLeft, XMVector3TransformCoord(XMLoadFloat3(&pos), transformationMatrix));
    m_squares.push_back(square);
    m_resetBuffers = true;
}

void Stroke::Update(ID3D11DeviceContext* pDeviceContext)
{
    if (m_resetBuffers)
    {
        if (m_squares.size() > 0)
        {
            Microsoft::WRL::ComPtr<ID3D11Device> spDevice;
            pDeviceContext->GetDevice(spDevice.ReleaseAndGetAddressOf());
            std::vector<VertexPosition> vertices;
            for (auto& it : m_squares)
            {
                vertices.push_back({ it.TopLeft });
                vertices.push_back({ it.TopRight });
                vertices.push_back({ it.BottomRight });
                vertices.push_back({ it.BottomLeft });
            }

            D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
            vertexBufferData.pSysMem = vertices.data();
            vertexBufferData.SysMemPitch = 0;
            vertexBufferData.SysMemSlicePitch = 0;
            const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPosition) * static_cast<UINT>(vertices.size()), D3D11_BIND_VERTEX_BUFFER);
            DX::ThrowIfFailed(
                spDevice->CreateBuffer(
                    &vertexBufferDesc,
                    &vertexBufferData,
                    m_vertexBuffer.ReleaseAndGetAddressOf()
                )
            );


            std::vector<VertexIndex_t> indices;
            auto makeSquare = [&indices](VertexIndex_t tl, VertexIndex_t tr, VertexIndex_t br, VertexIndex_t bl)
            {
                indices.push_back(tl); indices.push_back(tr); indices.push_back(br);
                indices.push_back(tl); indices.push_back(br); indices.push_back(bl);
            };
            // Back face
            makeSquare(1, 0, 3, 2);
            // Front face
            VertexIndex_t start = static_cast<VertexIndex_t>(4 * (m_squares.size() - 1));
            makeSquare(start, start + 1, start + 2, start + 3);
            // Now, sides
            unsigned int sidesCount = static_cast<unsigned int>(m_squares.size() - 1);
            for (unsigned int sideIndex = 0; sideIndex < sidesCount; sideIndex++)
            {
                start = static_cast<VertexIndex_t>(4 * sideIndex);
                // +0 = back / top / left
                // +1 = back / top / right
                // +2 = back / bottom / right
                // +3 = back / bottom / left
                // +4 = front / top / left
                // +5 = front / top / right
                // +6 = front / bottom / right
                // +7 = front / bottom / left

                makeSquare(start + 0, start + 1, start + 5, start + 4); // Top
                makeSquare(start + 5, start + 1, start + 2, start + 6); // Right
                makeSquare(start + 0, start + 4, start + 7, start + 3); // Left
                makeSquare(start + 7, start + 6, start + 2, start + 3); // Bottom
            }

            m_indexCount = static_cast<unsigned int>(indices.size());

            D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
            indexBufferData.pSysMem = indices.data();
            indexBufferData.SysMemPitch = 0;
            indexBufferData.SysMemSlicePitch = 0;
            CD3D11_BUFFER_DESC indexBufferDesc(sizeof(VertexIndex_t) * static_cast<UINT>(indices.size()), D3D11_BIND_INDEX_BUFFER);
            DX::ThrowIfFailed(
                spDevice->CreateBuffer(
                    &indexBufferDesc,
                    &indexBufferData,
                    m_indexBuffer.ReleaseAndGetAddressOf()
                )
            );
        }
        m_resetBuffers = false;
    }
}

void Stroke::Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper)
{
    if (m_squares.size() > 0)
    {
        const UINT stride = sizeof(VertexPosition);
        const UINT offset = 0;
        pRenderingHelper->SetConstantBuffer(&m_modelConstantBufferData);
        pDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
        pDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DX::GetDxgiFormat(), 0);
        pDeviceContext->DrawIndexedInstanced(m_indexCount, 2, 0, 0, 0);
    }
}

