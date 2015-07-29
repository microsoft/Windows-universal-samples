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
#include "GameRenderer.h"
#include "ConstantBuffers.h"
#include "TargetTexture.h"
#include "BasicLoader.h"
#include "CylinderMesh.h"
#include "FaceMesh.h"
#include "SphereMesh.h"
#include "WorldMesh.h"
#include "Face.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "windows.ui.xaml.media.dxinterop.h"

using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;

//----------------------------------------------------------------------

GameRenderer::GameRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    m_initialized(false),
    m_gameResourcesLoaded(false),
    m_levelResourcesLoaded(false)
{
    m_gameHud = ref new GameHud(
        deviceResources,
        "Windows platform samples",
        "DirectX/XAML first-person game sample",
        "License state unknown"
        );

    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

//----------------------------------------------------------------------

void GameRenderer::ReleaseDeviceDependentResources()
{
    // On device lost all the device resources are invalid.
    // Set the state of the renderer to not have a pointer to the
    // Simple3DGame object.  It will be reset as a part of the
    // game devices resources being recreated.
    m_game = nullptr;
    m_gameHud->ReleaseDeviceDependentResources();
}

//----------------------------------------------------------------------

void GameRenderer::CreateDeviceDependentResources()
{
    m_gameResourcesLoaded = false;
    m_levelResourcesLoaded = false;

    m_gameHud->CreateDeviceDependentResources();
}

//----------------------------------------------------------------------

void GameRenderer::CreateWindowSizeDependentResources()
{
    m_gameHud->CreateWindowSizeDependentResources();

    auto d3dContext = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetSize = m_deviceResources->GetRenderTargetSize();

    if (m_game != nullptr)
    {
        // Update the Projection Matrix and the associated Constant Buffer.
        m_game->GameCamera()->SetProjParams(
            XM_PI / 2, renderTargetSize.Width / renderTargetSize.Height,
            0.01f,
            100.0f
            );

        XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

        ConstantBufferChangeOnResize changesOnResize;
        XMStoreFloat4x4(
            &changesOnResize.projection,
            XMMatrixMultiply(
                XMMatrixTranspose(m_game->GameCamera()->Projection()),
                XMMatrixTranspose(XMLoadFloat4x4(&orientation))
                )
            );

        d3dContext->UpdateSubresource(
            m_constantBufferChangeOnResize.Get(),
            0,
            nullptr,
            &changesOnResize,
            0,
            0
            );
    }
}

//----------------------------------------------------------------------

task<void> GameRenderer::CreateGameDeviceResourcesAsync(_In_ Simple3DGame^ game)
{
    // Create the device dependent game resources.
    // Only the d3dDevice is used in this method.  It is expected
    // to not run on the same thread as the GameRenderer was created.
    // Create methods on the d3dDevice are free-threaded and are safe while any methods
    // in the d3dContext should only be used on a single thread and handled
    // in the FinalizeCreateGameDeviceResources method.
    m_game = game;

    auto d3dDevice = m_deviceResources->GetD3DDevice();

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    // Create the constant buffers.
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.ByteWidth = (sizeof(ConstantBufferNeverChanges) + 15) / 16 * 16;
    DX::ThrowIfFailed(
        d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferNeverChanges)
        );

    bd.ByteWidth = (sizeof(ConstantBufferChangeOnResize) + 15) / 16 * 16;
    DX::ThrowIfFailed(
        d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferChangeOnResize)
        );

    bd.ByteWidth = (sizeof(ConstantBufferChangesEveryFrame) + 15) / 16 * 16;
    DX::ThrowIfFailed(
        d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferChangesEveryFrame)
        );

    bd.ByteWidth = (sizeof(ConstantBufferChangesEveryPrim) + 15) / 16 * 16;
    DX::ThrowIfFailed(
        d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferChangesEveryPrim)
        );

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));

    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = FLT_MAX;
    DX::ThrowIfFailed(
        d3dDevice->CreateSamplerState(&sampDesc, &m_samplerLinear)
        );

    // Start the async tasks to load the shaders and textures.
    BasicLoader^ loader = ref new BasicLoader(d3dDevice);

    std::vector<task<void>> tasks;

    uint32 numElements = ARRAYSIZE(PNTVertexLayout);
    tasks.push_back(loader->LoadShaderAsync("VertexShader.cso", PNTVertexLayout, numElements, &m_vertexShader, &m_vertexLayout));
    tasks.push_back(loader->LoadShaderAsync("VertexShaderFlat.cso", nullptr, numElements, &m_vertexShaderFlat, nullptr));
    tasks.push_back(loader->LoadShaderAsync("PixelShader.cso", &m_pixelShader));
    tasks.push_back(loader->LoadShaderAsync("PixelShaderFlat.cso", &m_pixelShaderFlat));

    // Make sure the previous versions if any of the textures are released.
    m_sphereTexture = nullptr;
    m_cylinderTexture = nullptr;
    for (uint32 i = 0; i < GameConstants::MaxBackgroundTextures; i++)
    {
        m_ceilingTexture[i] = nullptr;
        m_floorTexture[i] = nullptr;
        m_wallsTexture[i] = nullptr;
    }

    // Load Game specific textures.
    tasks.push_back(loader->LoadTextureAsync("Assets\\seafloor.dds", nullptr, &m_sphereTexture));
    tasks.push_back(loader->LoadTextureAsync("Assets\\metal_texture.dds", nullptr, &m_cylinderTexture));
    tasks.push_back(loader->LoadTextureAsync("Assets\\cellceiling.dds", nullptr, &m_ceilingTexture[0]));
    tasks.push_back(loader->LoadTextureAsync("Assets\\cellfloor.dds", nullptr, &m_floorTexture[0]));
    tasks.push_back(loader->LoadTextureAsync("Assets\\cellwall.dds", nullptr, &m_wallsTexture[0]));
    tasks.push_back(loader->LoadTextureAsync("Assets\\nightwall.dds", nullptr, &m_ceilingTexture[1]));
    tasks.push_back(loader->LoadTextureAsync("Assets\\nightfloor.dds", nullptr, &m_floorTexture[1]));
    tasks.push_back(loader->LoadTextureAsync("Assets\\nightwall.dds", nullptr, &m_wallsTexture[1]));
    tasks.push_back(loader->LoadTextureAsync("Assets\\daywall.dds", nullptr, &m_ceilingTexture[2]));
    tasks.push_back(loader->LoadTextureAsync("Assets\\dayfloor.dds", nullptr, &m_floorTexture[2]));
    tasks.push_back(loader->LoadTextureAsync("Assets\\daywall.dds", nullptr, &m_wallsTexture[2]));
    tasks.push_back(create_task([]()
    {
        // Simulate loading additional resources by introducing a delay.
        wait(GameConstants::InitialLoadingDelay);
    }));

    // Return the task group of all the async tasks for loading the shader and texture assets.
    return when_all(tasks.begin(), tasks.end());
}

//----------------------------------------------------------------------

void GameRenderer::FinalizeCreateGameDeviceResources()
{
    // All asynchronously loaded resources have completed loading.
    // Now associate all the resources with the appropriate game objects.
    // This method is expected to run in the same thread as the GameRenderer
    // was created. All work will happen behind the "Loading ..." screen after the
    // main loop has been entered.

    // Initialize the Constant buffer with the light positions
    // These are handled here to ensure that the d3dContext is only
    // used in one thread.

    auto d3dDevice = m_deviceResources->GetD3DDevice();

    ConstantBufferNeverChanges constantBufferNeverChanges;
    constantBufferNeverChanges.lightPosition[0] = XMFLOAT4( 3.5f, 2.5f,  5.5f, 1.0f);
    constantBufferNeverChanges.lightPosition[1] = XMFLOAT4( 3.5f, 2.5f, -5.5f, 1.0f);
    constantBufferNeverChanges.lightPosition[2] = XMFLOAT4(-3.5f, 2.5f, -5.5f, 1.0f);
    constantBufferNeverChanges.lightPosition[3] = XMFLOAT4( 3.5f, 2.5f,  5.5f, 1.0f);
    constantBufferNeverChanges.lightColor = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
    m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(
        m_constantBufferNeverChanges.Get(),
        0,
        nullptr,
        &constantBufferNeverChanges,
        0,
        0
        );

    // For the targets, there are two unique generated textures.
    // Each texture image includes the number of the texture.
    // Make sure the 2D rendering is occurring on the same thread
    // as the main rendering.

    TargetTexture^ textureGenerator = ref new TargetTexture(
        d3dDevice,
        m_deviceResources->GetD2DFactory(),
        m_deviceResources->GetDWriteFactory(),
        m_deviceResources->GetD2DDeviceContext()
        );

    MeshObject^ cylinderMesh = ref new CylinderMesh(d3dDevice, 26);
    MeshObject^ targetMesh = ref new FaceMesh(d3dDevice);
    MeshObject^ sphereMesh = ref new SphereMesh(d3dDevice, 26);

    Material^ cylinderMaterial = ref new Material(
        XMFLOAT4(0.8f, 0.8f, 0.8f, .5f),
        XMFLOAT4(0.8f, 0.8f, 0.8f, .5f),
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        15.0f,
        m_cylinderTexture.Get(),
        m_vertexShader.Get(),
        m_pixelShader.Get()
        );

    Material^ sphereMaterial = ref new Material(
        XMFLOAT4(0.8f, 0.4f, 0.0f, 1.0f),
        XMFLOAT4(0.8f, 0.4f, 0.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        50.0f,
        m_sphereTexture.Get(),
        m_vertexShader.Get(),
        m_pixelShader.Get()
        );

    auto objects = m_game->RenderObjects();

    // Attach the textures to the appropriate game objects.
    for (auto object = objects.begin(); object != objects.end(); object++)
    {
        if ((*object)->TargetId() == GameConstants::WorldFloorId)
        {
            (*object)->NormalMaterial(
                ref new Material(
                    XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
                    XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
                    XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
                    150.0f,
                    m_floorTexture[0].Get(),
                    m_vertexShaderFlat.Get(),
                    m_pixelShaderFlat.Get()
                    )
                );
            (*object)->Mesh(ref new WorldFloorMesh(d3dDevice));
        }
        else if ((*object)->TargetId() == GameConstants::WorldCeilingId)
        {
            (*object)->NormalMaterial(
                ref new Material(
                    XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
                    XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
                    XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
                    150.0f,
                    m_ceilingTexture[0].Get(),
                    m_vertexShaderFlat.Get(),
                    m_pixelShaderFlat.Get()
                    )
                );
            (*object)->Mesh(ref new WorldCeilingMesh(d3dDevice));
        }
        else if ((*object)->TargetId() == GameConstants::WorldWallsId)
        {
            (*object)->NormalMaterial(
                ref new Material(
                    XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
                    XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
                    XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
                    150.0f,
                    m_wallsTexture[0].Get(),
                    m_vertexShaderFlat.Get(),
                    m_pixelShaderFlat.Get()
                    )
                );
            (*object)->Mesh(ref new WorldWallsMesh(d3dDevice));
        }
        else if (Cylinder^ cylinder = dynamic_cast<Cylinder^>(*object))
        {
            cylinder->Mesh(cylinderMesh);
            cylinder->NormalMaterial(cylinderMaterial);
        }
        else if (Face^ target = dynamic_cast<Face^>(*object))
        {
            const int bufferLength = 16;
            char16 str[bufferLength];
            int len = swprintf_s(str, bufferLength, L"%d", target->TargetId());
            Platform::String^ string = ref new Platform::String(str, len);

            ComPtr<ID3D11ShaderResourceView> texture;
            textureGenerator->CreateTextureResourceView(string, &texture);
            target->NormalMaterial(
                ref new Material(
                    XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f),
                    XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f),
                    XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
                    5.0f,
                    texture.Get(),
                    m_vertexShader.Get(),
                    m_pixelShader.Get()
                    )
                );

            textureGenerator->CreateHitTextureResourceView(string, &texture);
            target->HitMaterial(
                ref new Material(
                    XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f),
                    XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f),
                    XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
                    5.0f,
                    texture.Get(),
                    m_vertexShader.Get(),
                    m_pixelShader.Get()
                    )
                );

            target->Mesh(targetMesh);
        }
        else if (Sphere^ sphere = dynamic_cast<Sphere^>(*object))
        {
            sphere->Mesh(sphereMesh);
            sphere->NormalMaterial(sphereMaterial);
        }
    }

    // Ensure that the camera has been initialized with the right Projection
    // matrix.  The camera is not created at the time the first window resize event
    // occurs.
    auto renderTargetSize = m_deviceResources->GetRenderTargetSize();
    m_game->GameCamera()->SetProjParams(
        XM_PI / 2,
        renderTargetSize.Width / renderTargetSize.Height,
        0.01f,
        100.0f
        );

    // Make sure that Projection matrix has been set in the constant buffer
    // now that all the resources are loaded.
    // We are handling screen rotations directly to eliminate an unaligned
    // fullscreen copy.  As a result, it is necessary to post multiply the rotationTransform3D
    // matrix to the camera projection matrix.
    // The matrices are transposed due to the shader code expecting the matrices in the opposite
    // row/column order from the DirectX math library.

    auto orientation = m_deviceResources->GetOrientationTransform3D();

    ConstantBufferChangeOnResize changesOnResize;
    XMStoreFloat4x4(
        &changesOnResize.projection,
        XMMatrixMultiply(
            XMMatrixTranspose(m_game->GameCamera()->Projection()),
            XMMatrixTranspose(XMLoadFloat4x4(&orientation))
            )
        );

    m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(
        m_constantBufferChangeOnResize.Get(),
        0,
        nullptr,
        &changesOnResize,
        0,
        0
        );

    m_gameResourcesLoaded = true;
}

//----------------------------------------------------------------------

task<void> GameRenderer::LoadLevelResourcesAsync()
{
    m_levelResourcesLoaded = false;

    return create_task([this]()
    {
        // This is where additional async loading of level specific resources
        // would be done.  Since there aren't any to load, just simulate
        // by delaying for some time.
        wait(GameConstants::LevelLoadingDelay);
    });
}

//----------------------------------------------------------------------

void GameRenderer::FinalizeLoadLevelResources()
{
    // Once the level specific resources had been loaded, this method is
    // where D3D context specific actions would be handled.  This method
    // runs in the same thread context as the GameRenderer was created.

    m_levelResourcesLoaded = true;
}

//----------------------------------------------------------------------

void GameRenderer::Render()
{
    bool stereoEnabled = m_deviceResources->GetStereoState();

    auto d3dContext = m_deviceResources->GetD3DDeviceContext();
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    int renderingPasses = 1;
    if (stereoEnabled)
    {
        renderingPasses = 2;
    }

    for (int i = 0; i < renderingPasses; i++)
    {
        // Iterate through the number of rendering passes to be completed.
        if (i > 0)
        {
            // Doing the Right Eye View.
            ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetViewRight() };

            d3dContext->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());
            d3dContext->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
            d2dContext->SetTarget(m_deviceResources->GetD2DTargetBitmapRight());
        }
        else
        {
            // Doing the Mono or Left Eye View.
            ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };

            d3dContext->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());
            d3dContext->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
            d2dContext->SetTarget(m_deviceResources->GetD2DTargetBitmap());
        }

        if (m_game != nullptr && m_gameResourcesLoaded && m_levelResourcesLoaded)
        {
            // This section is only used after the game state has been initialized and all device
            // resources needed for the game have been created and associated with the game objects.
            if (stereoEnabled)
            {
                // When doing stereo, it is necessary to update the projection matrix once per rendering pass.

                auto orientation = m_deviceResources->GetOrientationTransform3D();

                ConstantBufferChangeOnResize changesOnResize;
                XMStoreFloat4x4(
                    &changesOnResize.projection,
                    XMMatrixMultiply(
                        XMMatrixTranspose(
                            i == 0 ?
                            m_game->GameCamera()->LeftEyeProjection() :
                            m_game->GameCamera()->RightEyeProjection()
                            ),
                        XMMatrixTranspose(XMLoadFloat4x4(&orientation))
                        )
                    );

                d3dContext->UpdateSubresource(
                    m_constantBufferChangeOnResize.Get(),
                    0,
                    nullptr,
                    &changesOnResize,
                    0,
                    0
                    );
            }
            // Update variables that change once per frame.

            ConstantBufferChangesEveryFrame constantBufferChangesEveryFrame;
            XMStoreFloat4x4(
                &constantBufferChangesEveryFrame.view,
                XMMatrixTranspose(m_game->GameCamera()->View())
                );
            d3dContext->UpdateSubresource(
                m_constantBufferChangesEveryFrame.Get(),
                0,
                nullptr,
                &constantBufferChangesEveryFrame,
                0,
                0
                );

            // Setup the graphics pipeline. This sample uses the same InputLayout and set of
            // constant buffers for all shaders, so they only need to be set once per frame.

            d3dContext->IASetInputLayout(m_vertexLayout.Get());
            d3dContext->VSSetConstantBuffers(0, 1, m_constantBufferNeverChanges.GetAddressOf());
            d3dContext->VSSetConstantBuffers(1, 1, m_constantBufferChangeOnResize.GetAddressOf());
            d3dContext->VSSetConstantBuffers(2, 1, m_constantBufferChangesEveryFrame.GetAddressOf());
            d3dContext->VSSetConstantBuffers(3, 1, m_constantBufferChangesEveryPrim.GetAddressOf());

            d3dContext->PSSetConstantBuffers(2, 1, m_constantBufferChangesEveryFrame.GetAddressOf());
            d3dContext->PSSetConstantBuffers(3, 1, m_constantBufferChangesEveryPrim.GetAddressOf());
            d3dContext->PSSetSamplers(0, 1, m_samplerLinear.GetAddressOf());

            auto objects = m_game->RenderObjects();
            for (auto object = objects.begin(); object != objects.end(); object++)
            {
                (*object)->Render(d3dContext, m_constantBufferChangesEveryPrim.Get());
            }
        }
        else
        {
            const float ClearColor[4] = { 0.5f, 0.5f, 0.8f, 1.0f };

            // Only need to clear the background when not rendering the full 3D scene since
            // the 3D world is a fully enclosed box and the dynamics prevents the camera from
            // moving outside this space.
            if (i > 0)
            {
                // Doing the Right Eye View.
                d3dContext->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetViewRight(), ClearColor);
            }
            else
            {
                // Doing the Mono or Left Eye View.
                d3dContext->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), ClearColor);
            }
        }

        d2dContext->BeginDraw();

        // To handle the swapchain being pre-rotated, set the D2D transformation to include it.
        d2dContext->SetTransform(m_deviceResources->GetOrientationTransform2D());

        if (m_game != nullptr && m_gameResourcesLoaded)
        {
            // This is only used after the game state has been initialized.
            m_gameHud->Render(m_game);
        }

        // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
        // is lost. It will be handled during the next call to Present.
        HRESULT hr = d2dContext->EndDraw();
        if (hr != D2DERR_RECREATE_TARGET)
        {
            DX::ThrowIfFailed(hr);
        }
    }
}

//----------------------------------------------------------------------

#if defined(_DEBUG)
void GameRenderer::ReportLiveDeviceObjects()
{
    // If the debug layer isn't present on the system then this QI will fail.  In that case
    // the routine just exits.   This is a debugging aid to see the active D3D objects and
    // is not critical to functional operation of the sample.

    ComPtr<ID3D11Debug> debugLayer;
    ComPtr<ID3D11Device2>d3dDevice = m_deviceResources->GetD3DDevice();
    HRESULT hr = d3dDevice.As(&debugLayer);
    if (FAILED(hr))
    {
        return;
    }
    DX::ThrowIfFailed(
        debugLayer->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL)
        );
}

//----------------------------------------------------------------------

#endif

void GameRenderer::SetBackground(uint32 background)
{
    if (background < GameConstants::MaxBackgroundTextures)
    {
        if (m_ceilingTexture[background] != nullptr &&
            m_floorTexture[background] != nullptr &&
            m_wallsTexture[background] != nullptr)
        {
            auto objects = m_game->RenderObjects();

            // Attach the textures to the appropriate game objects.
            for (auto object = objects.begin(); object != objects.end(); object++)
            {
                if ((*object)->TargetId() == GameConstants::WorldFloorId)
                {
                    (*object)->NormalMaterial()->SetTexture(m_floorTexture[background].Get());
                }
                else if ((*object)->TargetId() == GameConstants::WorldCeilingId)
                {
                    (*object)->NormalMaterial()->SetTexture(m_ceilingTexture[background].Get());
                }
                else if ((*object)->TargetId() == GameConstants::WorldWallsId)
                {
                    (*object)->NormalMaterial()->SetTexture(m_wallsTexture[background].Get());
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------


