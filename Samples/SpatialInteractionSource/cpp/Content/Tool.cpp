#include "pch.h"
#include "Tool.h"
#include "Common\DirectXHelper.h"

using namespace HolographicPaint;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;


Tool::Tool(Windows::UI::Color color, float diameter)
    : m_color(color), m_diameter(diameter)
{
    m_modelConstantBufferData.color = DX::GetDXColor(color);
}

void Tool::SetPosition(float3 position)
{
    m_position = position;
    m_refreshModelConstantBufferData = true;
}

void Tool::SetOrientation(quaternion orientation)
{
    m_orientation = quaternion(orientation.x, orientation.y, orientation.z, orientation.w);
    m_refreshModelConstantBufferData = true;
}

void Tool::Update(ID3D11DeviceContext*)
{
    if (m_refreshModelConstantBufferData)
    {
        const XMMATRIX modelRotation = XMMatrixRotationQuaternion(XMLoadQuaternion(&m_orientation));

        const XMMATRIX modelScale = XMMatrixScaling(m_diameter, m_diameter, max(m_diameter, .01f));
        // Position the cube.
        const XMMATRIX modelTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_position));

        // Multiply to get the transform matrix.
        // Note that this transform does not enforce a particular coordinate system. The calling
        // class is responsible for rendering this content in a consistent manner.
        const XMMATRIX modelTransform = modelScale * modelRotation * modelTranslation;

        // The view and projection matrices are provided by the system; they are associated
        // with holographic cameras, and updated on a per-camera basis.
        // Here, we provide the model transform for the sample hologram. The model transform
        // matrix is transposed to prepare it for the shader.
        XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(modelTransform));

        m_refreshModelConstantBufferData = false;
    }
}

void Tool::Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper)
{
    pRenderingHelper->SetConstantBuffer(&m_modelConstantBufferData);
    pRenderingHelper->DrawCube();
}

ClearTool::ClearTool(Windows::UI::Color color, float diameter) : Tool(color, diameter)
{}

void ClearTool::Update(ID3D11DeviceContext* pDeviceContext)
{
    __super::Update(pDeviceContext);
    if (m_resetBuffers)
    {
        Microsoft::WRL::ComPtr<ID3D11Device> spDevice;
        pDeviceContext->GetDevice(spDevice.ReleaseAndGetAddressOf());
        std::vector<VertexPosition> vertices;

        vertices.push_back({ XMFLOAT3(-0.5f,  0.5f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.0f,  0.5f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.5f,  0.5f, -0.5f) });
        vertices.push_back({ XMFLOAT3(-0.2f,  0.4f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.2f,  0.4f, -0.5f) });
        vertices.push_back({ XMFLOAT3(-0.4f,  0.2f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.0f,  0.2f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.4f,  0.2f, -0.5f) });
        vertices.push_back({ XMFLOAT3(-0.5f,  0.0f, -0.5f) });
        vertices.push_back({ XMFLOAT3(-0.2f,  0.0f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.2f,  0.0f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.5f,  0.0f, -0.5f) });
        vertices.push_back({ XMFLOAT3(-0.4f, -0.2f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.0f, -0.2f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.4f, -0.2f, -0.5f) });
        vertices.push_back({ XMFLOAT3(-0.2f, -0.4f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.2f, -0.4f, -0.5f) });
        vertices.push_back({ XMFLOAT3(-0.5f, -0.5f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.0f, -0.5f, -0.5f) });
        vertices.push_back({ XMFLOAT3( 0.5f, -0.5f, -0.5f) });

        vertices.push_back({ XMFLOAT3(-0.5f,  0.5f,  0.5f) });
        vertices.push_back({ XMFLOAT3( 0.5f,  0.5f,  0.5f) });
        vertices.push_back({ XMFLOAT3( 0.5f, -0.5f,  0.5f) });
        vertices.push_back({ XMFLOAT3(-0.5f, -0.5f,  0.5f) });

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

        static const VertexIndex_t indices[] =
        {
             0,  1,  3,
             0,  1,  3,
             1,  2,  4,
             0,  3,  5,
             1,  6,  3,
             1,  4,  6,
             2,  7,  4,
             0,  5,  8,
             2, 11,  7,
             8,  5,  9,
             7, 11, 10,
             8,  9, 12,
             8, 12, 17,
            10, 11, 14,
            17, 12, 15,
            18, 15, 18,
            13, 18, 15,
            13, 16, 18,
            16, 19, 18,
            14, 19, 16,
            11, 19, 14,
            17, 15, 18,
        };
        m_indexCount = _countof(indices);

        D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
        indexBufferData.pSysMem = indices;
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;
        CD3D11_BUFFER_DESC indexBufferDesc(static_cast<UINT>(sizeof(VertexIndex_t) * m_indexCount), D3D11_BIND_INDEX_BUFFER);
        DX::ThrowIfFailed(
            spDevice->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                m_indexBuffer.ReleaseAndGetAddressOf()
            )
        );
        m_resetBuffers = false;
    }

}

void ClearTool::Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper)
{
    const UINT stride = sizeof(VertexPosition);
    const UINT offset = 0;
    pRenderingHelper->SetConstantBuffer(&m_modelConstantBufferData);
    pDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    pDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DX::GetDxgiFormat(), 0);
    pDeviceContext->DrawIndexedInstanced(m_indexCount, 2, 0, 0, 0);
}
