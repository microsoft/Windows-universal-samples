#include "pch.h"
#include "BaseControl.h"
#include "DirectXHelper.h"
#include "DirectXCollision.h"
#include "..\360VideoPlaybackMain.h"

using namespace _360VideoPlayback;
using namespace Concurrency;
using namespace DirectX;
using namespace DirectX::TriangleTests;
using namespace D2D1;
using namespace DX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

const float4 defaultGlassColor = { 0.0f, 0.0f, 0.0f, 0.5f };
const float4 constantFocusPointColor = { 1.0f, 1.0f, 1.0f, 1.0f };
const float constantFocusPointRadius = 0.1f;
const float constantFocusPointIntensity = 1.0f;

BaseControl::BaseControl(const std::shared_ptr<MeshGeometry> geometry)
{
    m_deviceResources = _360VideoPlaybackMain::GetDeviceResources();
    m_isVisible = false;
    m_isInitialized = false;
    m_isFocused = false;
    m_textureWidth = DEFAULT_TEXTURE_SIZE;
    m_textureHeight = DEFAULT_TEXTURE_SIZE;
    m_geometry = geometry;
    m_scale = float3(1.0f, 1.0f, 1.0f);
    m_position = float3(0, 0, 0);
    m_isVisible = true;
    m_transform = float4x4::identity();
    m_parentTransform = float4x4::identity();
    m_relativeIntersectionPoint = float2(0.0f);
}

void BaseControl::Initialize()
{
    const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(_360VideoPlayback::ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

    DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(
        &constantBufferDesc,
        nullptr,
        &m_modelConstantBuffer));

    CalculateInitialSize();

    AllocateTexture();

    InitializeFocusPoint();

    m_isInitialized = true;
}

void BaseControl::AllocateTexture()
{
    // Create the texture to render the glyph for this control as well as the glyph for the control.
    // NOTE: If we want visual fidelity that better matches the distance the user is from the button we should
    // have different sizes of textures (and glyphs) depending on how close the user is to the control.
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    auto d3ddevice = m_deviceResources->GetD3DDevice();
    ComPtr<IDXGISurface> cubeTextureSurface;
    CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    D3D11_SAMPLER_DESC samplerDescription;
    CD3D11_TEXTURE2D_DESC textureDesc;
    FLOAT dpiX;
    FLOAT dpiY;

    // Create texture for rendering d2d onto
    textureDesc = CD3D11_TEXTURE2D_DESC(
        DXGI_FORMAT_B8G8R8A8_UNORM,
        m_textureWidth,         // Width
        m_textureHeight,        // Height
        1,                      // MipLevels
        1,                      // ArraySize
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
    );

    DX::ThrowIfFailed(d3ddevice->CreateTexture2D(
        &textureDesc,
        nullptr,
        &m_texture));
    shaderResourceViewDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(
        m_texture.Get(),
        D3D11_SRV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(d3ddevice->CreateShaderResourceView(
        m_texture.Get(),
        &shaderResourceViewDesc,
        &m_textureShaderResourceView));

    d2dContext->GetDpi(&dpiX, &dpiY);
    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpiX,
            dpiY);
    m_texture.As(&cubeTextureSurface);

    DX::ThrowIfFailed(d2dContext->CreateBitmapFromDxgiSurface(
        cubeTextureSurface.Get(),
        &bitmapProperties,
        &m_textureTarget));

    // create the sampler
    ZeroMemory(&samplerDescription, sizeof(D3D11_SAMPLER_DESC));
    samplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDescription.MipLODBias = 0.0f;
    samplerDescription.MaxAnisotropy = 4;
    samplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDescription.BorderColor[0] = defaultGlassColor.x;
    samplerDescription.BorderColor[1] = defaultGlassColor.y;
    samplerDescription.BorderColor[2] = defaultGlassColor.z;
    samplerDescription.BorderColor[3] = defaultGlassColor.w;
    // allow use of all mip levels
    samplerDescription.MinLOD = 0;
    samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
    DX::ThrowIfFailed(d3ddevice->CreateSamplerState(
        &samplerDescription,
        &m_sampler));
    DX::ThrowIfFailed(m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock));
}

void BaseControl::Update(const StepTimer& timer, SpatialPointerPose^ cameraPose)
{
    auto scale = m_scale;
    const XMMATRIX modelScale = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
    const XMMATRIX modelTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_position));
    const XMMATRIX localTransform = XMMatrixMultiply(modelScale, modelTranslation);
    const XMMATRIX parentTransform = XMLoadFloat4x4(&m_parentTransform);
    const XMMATRIX modelTransform = XMMatrixMultiply(localTransform, parentTransform);

    XMStoreFloat4x4(&m_transform, modelTransform);
    // Update local copy of Constant buffer
    XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(XMLoadFloat4x4(&m_transform)));

    if (m_isVisible && cameraPose != nullptr)
    {
        m_isFocused = IsFocusOnControl(XMLoadFloat4x4(&m_transform), cameraPose);
    }

    // Update spot light parameters
    m_focusPointConstantBufferData.focusPointOrigin = { cameraPose->Head->Position.x, cameraPose->Head->Position.y, cameraPose->Head->Position.z, 1.0f };
    m_focusPointConstantBufferData.focusPointDirection = { cameraPose->Head->ForwardDirection.x, cameraPose->Head->ForwardDirection.y, cameraPose->Head->ForwardDirection.z, 1.0f };
    m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(
        m_focusPointConstantBuffer.Get(),
        0,
        nullptr,
        &m_focusPointConstantBufferData,
        0,
        0);
    m_renderItem->SetParentControlInformation(m_isFocused, m_relativeIntersectionPoint, m_textureWidth, m_textureHeight);
    m_renderItem->Update(timer, cameraPose);
    if (m_renderItem->GetIsRenderPassNeeded())
    {
        m_isUITextureRenderNeeded = true;
    }
}

void BaseControl::Render()
{
    if (m_isVisible)
    {
        UploadGeometryAndShaders();
        ApplyPixelShader();
        // At this point everything should be set, so Draw the objects.
        m_deviceResources->GetD3DDeviceContext()->DrawIndexedInstanced(
            m_geometry->IndexCount,  // Index count per instance.
            2,                       // Instance count.
            0,                       // Start index location.
            0,                       // Base vertex location.
            0);                      // Start instance location.
    }
}

void BaseControl::UploadGeometryAndShaders()
{
    const auto context = m_deviceResources->GetD3DDeviceContext();

    // Update the model transform buffer
    context->UpdateSubresource(
        m_modelConstantBuffer.Get(),
        0,
        nullptr,
        &m_modelConstantBufferData,
        0,
        0);

    const UINT stride = m_geometry->VertexStride;
    const UINT offset = 0;
    context->IASetVertexBuffers(
        0,
        1,
        m_geometry->VertexBuffer.GetAddressOf(),
        &stride,
        &offset);
    context->IASetIndexBuffer(
        m_geometry->IndexBuffer.Get(),
        DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
        0);
    context->IASetPrimitiveTopology(m_geometry->Topology);
    context->IASetInputLayout(m_inputLayout.Get());

    // Attach the vertex shader.
    context->VSSetShader(
        m_vertexShader.Get(),
        nullptr, 0);
    // Apply the model constant buffer to the vertex shader.
    context->VSSetConstantBuffers(
        0,
        1,
        m_modelConstantBuffer.GetAddressOf());
    m_deviceResources->GetD3DDeviceContext()->GSSetShader(
        m_geometryShader.Get(),
        nullptr,
        0);
}

void BaseControl::ApplyPixelShader()
{
    m_deviceResources->GetD3DDeviceContext()->PSSetShader(
        m_pixelShader.Get(),
        nullptr,
        0);

    m_deviceResources->GetD3DDeviceContext()->PSSetConstantBuffers(
        1, /*StartSlot*/
        1, /*NumBuffers*/
        m_focusPointConstantBuffer.GetAddressOf()
    );

    if (m_isUITextureRenderNeeded)
    {
        auto d2dContext = m_deviceResources->GetD2DDeviceContext();
        d2dContext->SaveDrawingState(m_stateBlock.Get());
        d2dContext->GetTarget(m_d2dContextTargetToRestore.GetAddressOf());
        // Set target to current texture
        d2dContext->SetTarget(m_textureTarget.Get());
        d2dContext->BeginDraw();
        ColorF clearColor = D2D1::ColorF(defaultGlassColor.x, defaultGlassColor.y, defaultGlassColor.z, defaultGlassColor.w);
        d2dContext->Clear(clearColor);

        m_renderItem->TryRender();

        // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
        // is lost. It will be handled during the next call to Present.
        auto endDrawHR = d2dContext->EndDraw();
        if (endDrawHR == D2DERR_RECREATE_TARGET)
        {
            return;
        }
        // We need to restore the previously set d2dContext target so 
        // future draw calls draw to the correct place.
        d2dContext->SetTarget(m_d2dContextTargetToRestore.Get());
        d2dContext->RestoreDrawingState(m_stateBlock.Get());
        m_isUITextureRenderNeeded = false;
    }

    // Tell the shader to use the texture
    m_deviceResources->GetD3DDeviceContext()->PSSetShaderResources(
        0,
        1,
        m_textureShaderResourceView.GetAddressOf());

    m_deviceResources->GetD3DDeviceContext()->PSSetSamplers(
        0,
        1,
        m_sampler.GetAddressOf());
}

void BaseControl::SetParentTransform(float4x4 transform)
{
    m_parentTransform = transform;
}

void BaseControl::SetScale(float3 value)
{
    m_scale = value;
}

void BaseControl::SetPosition(float3 value)
{
    m_position = value;
}


void BaseControl::InitializeFocusPoint()
{
    const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(FocusPointConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &constantBufferDesc,
            nullptr,
            &m_focusPointConstantBuffer));

    m_focusPointConstantBufferData.focusPointIntensity = { constantFocusPointIntensity,  constantFocusPointIntensity,  constantFocusPointIntensity, constantFocusPointIntensity };
    m_focusPointConstantBufferData.focusPointColor = { constantFocusPointColor.x, constantFocusPointColor.y, constantFocusPointColor.z, constantFocusPointColor.w };
    m_focusPointConstantBufferData.focusPointRadius = { constantFocusPointRadius,  constantFocusPointRadius,  constantFocusPointRadius, constantFocusPointRadius };
    m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(
        m_focusPointConstantBuffer.Get(),
        0,
        nullptr,
        &m_focusPointConstantBufferData,
        0,
        0
    );
}

void BaseControl::SetIsVisible(bool value)
{
    m_isVisible = value;
    if (!m_isVisible) // clear focus if you are turned off
    {
        m_isFocused = false;
    }
}

void BaseControl::PerformPressedAction()
{
    if (m_renderItem->IsFocused())
    {
        m_renderItem->PerformAction();
    }
}

void BaseControl::ApplyShaders()
{
    task<std::vector<byte>> loadVSTask = DX::ReadDataAsync(L"ms-appx:///GlassVertexShader.cso");
    task<void> createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> vertexPositionTextureDesc =
        { {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                } };
        DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(
            fileData.data(),
            fileData.size(),
            nullptr,
            &m_vertexShader));

        DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(
            vertexPositionTextureDesc.data(),
            static_cast<UINT>(vertexPositionTextureDesc.size()),
            fileData.data(),
            fileData.size(),
            &m_inputLayout));
    });

    task<std::vector<byte>> loadPSTask = DX::ReadDataAsync(L"ms-appx:///GlassPixelShader.cso");
    task<void> createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
    {
        DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(
            fileData.data(),
            fileData.size(),
            nullptr,
            &m_pixelShader));
    });

    task<std::vector<byte>> loadGSTask = DX::ReadDataAsync(L"ms-appx:///GlassGeometryShader.cso");

    task<void> createGSTask = loadGSTask.then([this](const std::vector<byte>& fileData)
    {
        DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateGeometryShader(
            fileData.data(),
            fileData.size(),
            nullptr,
            &m_geometryShader));
    });

    task<void> shaderTaskGroup = (createPSTask && createVSTask && createGSTask);
    task<void> createSphereTask = shaderTaskGroup.then([this]()
    {
    });
}

void BaseControl::SnapInitialSizeToRootElement()
{
    m_snapSizeToRootElement = true;
}

void BaseControl::CalculateInitialSize()
{
    // Calculate the geometry scale based on the texture dimensions
    if (m_snapSizeToRootElement)
    {
        m_textureWidth = static_cast<UINT>(m_renderItem->GetElementSize().x);
        if (m_textureWidth == 0)
        {
            m_textureWidth = DEFAULT_TEXTURE_SIZE;
        }
        m_textureHeight = static_cast<UINT>(m_renderItem->GetElementSize().y);
        if (m_textureHeight == 0)
        {
            m_textureHeight = DEFAULT_TEXTURE_SIZE;
        }

        // Scale the geometry so we fit 128 texture pixels per geometry unit.
        float geometryWidth = m_textureWidth / static_cast<float>(DEFAULT_TEXTURE_SIZE);
        float geomeryHeight = m_textureHeight / static_cast<float>(DEFAULT_TEXTURE_SIZE);

        m_scale.x *= geometryWidth;
        m_scale.y *= geomeryHeight;
    }
    else
    {
        // Scale the geometry so we fit 128 texture pixels per geometry unit.
        float geometryWidth = m_textureWidth / static_cast<float>(DEFAULT_TEXTURE_SIZE);
        float geomeryHeight = m_textureHeight / static_cast<float>(DEFAULT_TEXTURE_SIZE);

        m_scale.x *= geometryWidth;
        m_scale.y *= geomeryHeight;
    }
}

void BaseControl::SetRenderElement(std::shared_ptr<FocusableElement> renderItem)
{
    m_renderItem = renderItem;
}

bool BaseControl::IsFocusOnControl(const XMMATRIX& transform, SpatialPointerPose^ cameraPose)
{
    // Compute Base Plane Cube Bounds
    float4 topLeft = float4(-0.5f, 0.5f, 0.5f, 1.0f);
    float4 topRight = float4(0.5f, 0.5f, 0.5f, 1.0f);
    float4 bottomRight = float4(0.5f, -0.5f, 0.5f, 1.0f);
    float4 bottomLeft = float4(-0.5f, -0.5f, 0.5f, 1.0f);

    XMVECTOR transformedVerts[4];
    transformedVerts[0] = XMVector4Transform(XMLoadFloat4(&topLeft), transform);
    transformedVerts[1] = XMVector4Transform(XMLoadFloat4(&topRight), transform);
    transformedVerts[2] = XMVector4Transform(XMLoadFloat4(&bottomRight), transform);
    transformedVerts[3] = XMVector4Transform(XMLoadFloat4(&bottomLeft), transform);

    // After applying trasformation, W component of the transformed 4 value vectors to be 1    
#ifdef _XM_SSE_INTRINSICS_
    _ASSERTE(transformedVerts[0].m128_f32[3] == 1);
#endif // _XM_SSE_INTRINSICS_
#ifdef _XM_ARM_NEON_INTRINSICS_
    _ASSERTE(transformedVerts[0].n128_f32[3] == 1);
#endif // _XM_ARM_NEON_INTRINSICS_

    // This is fine to do Vector3 
    XMVECTOR normal = XMVector3Cross(XMVectorSubtract(transformedVerts[3], transformedVerts[0]), XMVectorSubtract(transformedVerts[1], transformedVerts[0]));

    float3 rayPosition = cameraPose->Head->Position;
    float3 rayDirection = cameraPose->Head->ForwardDirection;
    // If the Dot product between two vectors is less than 0, then the two normals are pointing the same direction
    // The normal computed above points in the direction that the front face is facing, so if another vector points the same direction as it, then it is looking at the back side
    // When using XMVector3Dot(), the scalar result of the Dot product is copied to all members of the returned XMVECTOR type. To get the result you just need to
    // access a member of the XMVECTOR
#ifdef _XM_SSE_INTRINSICS_
    if (XMVector3Dot(XMLoadFloat3(&rayDirection), normal).m128_f32[0] < 0.0f) // only perform intersection if we're looking at the front of the plane
#endif // _XM_SSE_INTRINSICS_
#ifdef _XM_ARM_NEON_INTRINSICS_
        if (XMVector3Dot(XMLoadFloat3(&rayDirection), normal).n128_f32[0] < 0.0f) // only perform intersection if we're looking at the front of the plane
#endif // _XM_ARM_NEON_INTRINSICS_
        {
            float t1IntersectDistance;
            float t2IntersectDistance;
            float focusedDistance;
            bool t1Intersects = Intersects(
                XMLoadFloat3(&rayPosition),
                XMLoadFloat3(&rayDirection),
                transformedVerts[0],
                transformedVerts[1],
                transformedVerts[3],
                OUT t1IntersectDistance);
            bool t2Intersects = Intersects(
                XMLoadFloat3(&rayPosition),
                XMLoadFloat3(&rayDirection),
                transformedVerts[3],
                transformedVerts[2],
                transformedVerts[1],
                OUT t2IntersectDistance);
            if (t1Intersects || t2Intersects)
            {
                if (t1Intersects && t2Intersects)
                {
                    focusedDistance = t1IntersectDistance < t2IntersectDistance ? t1IntersectDistance : t2IntersectDistance;
                }
                else
                {
                    focusedDistance = t1Intersects ? t1IntersectDistance : t2IntersectDistance;
                }
                float3 intersectionPosition = rayPosition + (focusedDistance * rayDirection);
                XMVECTOR modelSpaceIntersectionPoint = XMVector3Transform(XMLoadFloat3(&intersectionPosition), XMMatrixInverse(nullptr, transform));

#ifdef _XM_SSE_INTRINSICS_
                m_relativeIntersectionPoint = float2(abs(modelSpaceIntersectionPoint.m128_f32[0] - (-0.5f)), abs(modelSpaceIntersectionPoint.m128_f32[1] - 0.5f));
#endif // _XM_SSE_INTRINSICS_
#ifdef _XM_ARM_NEON_INTRINSICS_
                m_relativeIntersectionPoint = float2(abs(modelSpaceIntersectionPoint.n128_f32[0] - m_minXY.x), abs(modelSpaceIntersectionPoint.n128_f32[1] - m_minXY.y));
#endif // _XM_ARM_NEON_INTRINSICS_

                return true;
            }
        }
    return false;
}
