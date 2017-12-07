#include "pch.h"
#include "Whiteboard.h"
#include "..\Common\DirectXHelper.h"

using namespace HolographicPaint;
using namespace DirectX;
using namespace Windows::UI;
using namespace Windows::Foundation::Numerics;

const float WhiteboardDepth = .005f;

Whiteboard::Whiteboard(float width, float height, float3 position)
    : m_width(width), m_height(height), m_position(position)
{
    const XMMATRIX modelRotation = XMMatrixIdentity();

    const XMMATRIX modelScale = XMMatrixScaling(width, height, WhiteboardDepth);

    const XMMATRIX modelTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&position));

    // Multiply to get the transform matrix.
    // Note that this transform does not enforce a particular coordinate system. The calling
    // class is responsible for rendering this content in a consistent manner.
    const XMMATRIX modelTransform = modelScale * modelRotation * modelTranslation;

    // The view and projection matrices are provided by the system; they are associated
    // with holographic cameras, and updated on a per-camera basis.
    // Here, we provide the model transform for the sample hologram. The model transform
    // matrix is transposed to prepare it for the shader.
    XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(modelTransform));

    m_modelConstantBufferData.color = DX::GetDXColor(ColorHelper::FromArgb(0x40, 0xFf, 0xFF, 0xFF));


    XMFLOAT3 boundingPosition(position.x, position.y, position.z);
    XMFLOAT3 boundingExtents(width / 2.0f, height / 2.0f, WhiteboardDepth / 2.0f);
    m_boundingBox = BoundingBox(boundingPosition, boundingExtents);
}

void Whiteboard::Update(ID3D11DeviceContext* pDeviceContext)
{
    // Whiteboard do not move and hence update do not have to do anything special
}

void Whiteboard::Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper)
{
    pRenderingHelper->SetConstantBuffer(&m_modelConstantBufferData);
    pRenderingHelper->DrawCube();
}

bool Whiteboard::Intersects(const float3& origin, const float3& direction, float* distance)
{
    XMVECTOR originVector = XMLoadFloat3(&origin);
    XMVECTOR directionVector = XMLoadFloat3(&direction);
    return m_boundingBox.Intersects(originVector, directionVector, *distance);
}

bool Whiteboard::IsValidPosition(const Windows::Foundation::Numerics::float3& position)
{
    // As the whiteboard is axis-aligned and we do not take z into count, the test is simple
    return fabs(position.x - m_position.x) <= m_width / 2.0f && fabs(position.y - m_position.y) <= m_height / 2.0f;
}
