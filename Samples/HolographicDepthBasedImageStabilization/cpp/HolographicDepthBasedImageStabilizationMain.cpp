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
#include "HolographicDepthBasedImageStabilizationMain.h"
#include "Common\DirectXHelper.h"

#include <windows.graphics.directx.direct3d11.interop.h>
#include <Collection.h>

#include <string>

using namespace HolographicDepthBasedImageStabilization;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;
using namespace std::placeholders;

// Loads and initializes application assets when the application is loaded.
HolographicDepthBasedImageStabilizationMain::HolographicDepthBasedImageStabilizationMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    // Register to be notified if the device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);
}

void HolographicDepthBasedImageStabilizationMain::SetHolographicSpace(HolographicSpace^ holographicSpace)
{
    UnregisterHolographicEventHandlers();

    m_holographicSpace = holographicSpace;

    // Initialize the automatic image stabilization plane finder.
    m_depthBufferPlaneFinder.Init(m_deviceResources);

#ifdef DRAW_SAMPLE_CONTENT
    // Initialize the sample hologram.
    m_spinningPlaneRenderer = std::make_unique<SpinningPlaneRenderer>(m_deviceResources);
    
    m_depthPlaneRenderer = std::make_unique<QuadRenderer>(m_deviceResources);

    m_spatialInputHandler = std::make_unique<SpatialInputHandler>();
#endif

    // Use the default SpatialLocator to track the motion of the device.
    m_locator = SpatialLocator::GetDefault();

    // Be able to respond to changes in the positional tracking state.
    m_locatabilityChangedToken =
        m_locator->LocatabilityChanged +=
            ref new Windows::Foundation::TypedEventHandler<SpatialLocator^, Object^>(
                std::bind(&HolographicDepthBasedImageStabilizationMain::OnLocatabilityChanged, this, _1, _2)
                );

    // Respond to camera added events by creating any resources that are specific
    // to that camera, such as the back buffer render target view.
    // When we add an event handler for CameraAdded, the API layer will avoid putting
    // the new camera in new HolographicFrames until we complete the deferral we created
    // for that handler, or return from the handler without creating a deferral. This
    // allows the app to take more than one frame to finish creating resources and
    // loading assets for the new holographic camera.
    // This function should be registered before the app creates any HolographicFrames.
    m_cameraAddedToken =
        m_holographicSpace->CameraAdded +=
            ref new Windows::Foundation::TypedEventHandler<HolographicSpace^, HolographicSpaceCameraAddedEventArgs^>(
                std::bind(&HolographicDepthBasedImageStabilizationMain::OnCameraAdded, this, _1, _2)
                );

    // Respond to camera removed events by releasing resources that were created for that
    // camera.
    // When the app receives a CameraRemoved event, it releases all references to the back
    // buffer right away. This includes render target views, Direct2D target bitmaps, and so on.
    // The app must also ensure that the back buffer is not attached as a render target, as
    // shown in DeviceResources::ReleaseResourcesForBackBuffer.
    m_cameraRemovedToken =
        m_holographicSpace->CameraRemoved +=
            ref new Windows::Foundation::TypedEventHandler<HolographicSpace^, HolographicSpaceCameraRemovedEventArgs^>(
                std::bind(&HolographicDepthBasedImageStabilizationMain::OnCameraRemoved, this, _1, _2)
                );

    // The simplest way to render world-locked holograms is to create a stationary reference frame
    // when the app is launched. This is roughly analogous to creating a "world" coordinate system
    // with the origin placed at the device's position as the app is launched.
    m_referenceFrame = m_locator->CreateStationaryFrameOfReferenceAtCurrentLocation();

    // Notes on spatial tracking APIs:
    // * Stationary reference frames are designed to provide a best-fit position relative to the
    //   overall space. Individual positions within that reference frame are allowed to drift slightly
    //   as the device learns more about the environment.
    // * When precise placement of individual holograms is required, a SpatialAnchor should be used to
    //   anchor the individual hologram to a position in the real world - for example, a point the user
    //   indicates to be of special interest. Anchor positions do not drift, but can be corrected; the
    //   anchor will use the corrected position starting in the next frame after the correction has
    //   occurred.
}

void HolographicDepthBasedImageStabilizationMain::UnregisterHolographicEventHandlers()
{
    if (m_holographicSpace != nullptr)
    {
        // Clear previous event registrations.

        if (m_cameraAddedToken.Value != 0)
        {
            m_holographicSpace->CameraAdded -= m_cameraAddedToken;
            m_cameraAddedToken.Value = 0;
        }

        if (m_cameraRemovedToken.Value != 0)
        {
            m_holographicSpace->CameraRemoved -= m_cameraRemovedToken;
            m_cameraRemovedToken.Value = 0;
        }
    }

    if (m_locator != nullptr)
    {
        m_locator->LocatabilityChanged -= m_locatabilityChangedToken;
    }
}

HolographicDepthBasedImageStabilizationMain::~HolographicDepthBasedImageStabilizationMain()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);

    UnregisterHolographicEventHandlers();
}

// Updates the application state once per frame.
HolographicFrame^ HolographicDepthBasedImageStabilizationMain::Update()
{
    // Before doing the timer update, there is some work to do per-frame
    // to maintain holographic rendering. First, we will get information
    // about the current frame.

    // The HolographicFrame has information that the app needs in order
    // to update and render the current frame. The app begins each new
    // frame by calling CreateNextFrame.
    HolographicFrame^ holographicFrame = m_holographicSpace->CreateNextFrame();

    // Get a prediction of where holographic cameras will be when this frame
    // is presented.
    HolographicFramePrediction^ prediction = holographicFrame->CurrentPrediction;

    // Back buffers can change from frame to frame. Validate each buffer, and recreate
    // resource views and depth buffers as needed.
    m_deviceResources->EnsureCameraResources(holographicFrame, prediction);

    // Next, we get a coordinate system from the attached frame of reference that is
    // associated with the current frame. Later, this coordinate system is used for
    // for creating the stereo view matrices when rendering the sample content.
    SpatialCoordinateSystem^ currentCoordinateSystem = m_referenceFrame->CoordinateSystem;

#ifdef DRAW_SAMPLE_CONTENT
    // Check for new input state since the last frame.
    SpatialInteractionSourceState^ pointerState = m_spatialInputHandler->CheckForInput();
    if (pointerState != nullptr)
    {
        // When a Pressed gesture is detected, the sample hologram will be repositioned
        // two meters in front of the user.
        m_spinningPlaneRenderer->PositionHologram(
            pointerState->TryGetPointerPose(currentCoordinateSystem)
            );
    }
#endif

    m_timer.Tick([&] ()
    {
#ifdef DRAW_SAMPLE_CONTENT
        m_spinningPlaneRenderer->Update(m_timer);
#endif
    });

    // In this example, we will update the stabilization plane using information from the depth buffer.

    // The holographic frame will be used to get up-to-date view and projection matrices and
    // to present the swap chain.
    return holographicFrame;
}

// Renders the current frame to each holographic camera, according to the
// current application and spatial positioning state. Returns true if the
// frame was rendered to at least one camera.
bool HolographicDepthBasedImageStabilizationMain::Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame)
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return false;
    }

    // Lock the set of holographic camera resources, then draw to each camera
    // in this frame.
    return m_deviceResources->UseHolographicCameraResources<bool>(
        [this, holographicFrame](std::map<UINT32, std::unique_ptr<DX::CameraResources>>& cameraResourceMap)
    {
        // Up-to-date frame predictions enhance the effectiveness of image stablization and
        // allow more accurate positioning of holograms.
        holographicFrame->UpdateCurrentPrediction();
        HolographicFramePrediction^ prediction = holographicFrame->CurrentPrediction;

        bool atLeastOneCameraRendered = false;
        for (auto cameraPose : prediction->CameraPoses)
        {
            // This represents the device-based resources for a HolographicCamera.
            DX::CameraResources* pCameraResources = cameraResourceMap[cameraPose->HolographicCamera->Id].get();
            
            // Get the device context.
            const auto context = m_deviceResources->GetD3DDeviceContext();
            const auto device  = m_deviceResources->GetD3DDevice();
            
            // For image stabilization plane estimation, we use view/projection matrices from the previous 
            // frame, since the depth buffer we use is also from the previous frame.
            float4x4 const& viewMatrix        = pCameraResources->GetViewMatrix();
            float4x4 const& inverseViewMatrix = pCameraResources->GetInverseViewMatrix();

            m_depthBufferPlaneFinder.SetProjectionMatrix(pCameraResources->GetProjectionMatrix());

            // We must dispatch the drawing commands first, so that the result can be used for the compute shader in 
            // the image stabilization plane estimation.

            // Set render targets to the current holographic camera.
            ID3D11RenderTargetView *const targets[1] = { pCameraResources->GetBackBufferRenderTargetView() };
            const auto depthStencilView = pCameraResources->GetDepthStencilView();
            context->OMSetRenderTargets(1, targets, depthStencilView);

            // Clear the back buffer and depth stencil view.
            context->ClearRenderTargetView(targets[0], DirectX::Colors::Transparent);
            context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            // The view and projection matrices for each holographic camera will change
            // every frame. This function refreshes the data in the constant buffer for
            // the holographic camera indicated by cameraPose.
            pCameraResources->UpdateViewProjectionBuffer(m_deviceResources, cameraPose, m_referenceFrame->CoordinateSystem);

            // Attach the view/projection constant buffer for this camera to the graphics pipeline.
            bool cameraActive = pCameraResources->AttachViewProjectionBuffer(m_deviceResources);

#ifdef DRAW_SAMPLE_CONTENT
            // Only render world-locked content when positional tracking is active.
            if (cameraActive)
            {
                // Draw the sample content.
                m_spinningPlaneRenderer->Render();
            }
#endif

            atLeastOneCameraRendered = atLeastOneCameraRendered | cameraActive;

            // Do the image stabilization plane estimation.

            // Resolve depth for the current camera.
            float3 planeNormalInCameraSpace = {0, 0, 1.f};
            float distanceToPointInMeters = 1.9f;
            const bool estimationSuccess = m_depthBufferPlaneFinder.TryFindPlaneNormalAndDistance(
                pCameraResources, 
                planeNormalInCameraSpace, 
                distanceToPointInMeters);

//#define ENABLE_DEBUG_OUTPUT
#ifdef ENABLE_DEBUG_OUTPUT
            OutputDebugStringA(("Distance from near plane to point: " + std::to_string(distanceToPointInMeters) + "\n").c_str());
#endif

            if (estimationSuccess)
            {
                // Transform the stabilization point, which is computed in view space, into the "world space" coordinate system.
                float3 pointInViewSpace = { 0.f, 0.f, -distanceToPointInMeters };
                float3 pointInWorldSpace = transform(pointInViewSpace, inverseViewMatrix);

                // Note that the transpose of a rotation matrix is its inverse.
                float4x4 normalRotationInverse = transpose(viewMatrix);
                
                // Transform the LSR plane normal from view space to world space.
                planeNormalInCameraSpace = normalize(planeNormalInCameraSpace);
                float4 normalInCameraSpace = {
                    planeNormalInCameraSpace.x,
                   -planeNormalInCameraSpace.y,
                    planeNormalInCameraSpace.z,
                    Windows::Foundation::Numerics::dot(pointInViewSpace, planeNormalInCameraSpace)
                    };
                float4 planeNormalInWorldSpace4 = transform(normalInCameraSpace, normalRotationInverse);
                float3 planeNormalInWorldSpace = {
                    planeNormalInWorldSpace4.x,
                    planeNormalInWorldSpace4.y,
                    planeNormalInWorldSpace4.z
                };

                // Set the focus point for the current camera.
                HolographicCameraRenderingParameters^ renderingParameters = holographicFrame->GetRenderingParameters(cameraPose);
                renderingParameters->SetFocusPoint(
                    m_referenceFrame->CoordinateSystem,
                    pointInWorldSpace,
                    planeNormalInWorldSpace);

                m_depthPlaneRenderer->UpdateHologramPositionAndOrientation(
                    pointInWorldSpace,
                    planeNormalInWorldSpace);

                // Visualize the SR plane.

                // The depth estimation clears the render target state, so we need to set it again
                // here.
                ID3D11RenderTargetView *const targets[1] = { pCameraResources->GetBackBufferRenderTargetView() };
                const auto depthStencilView = pCameraResources->GetDepthStencilView();
                context->OMSetRenderTargets(1, targets, depthStencilView);

                // We should also ensure the view/projection buffer is attached.
                bool cameraActive = pCameraResources->AttachViewProjectionBuffer(m_deviceResources);

                m_depthPlaneRenderer->Render();
            }
        }

        return atLeastOneCameraRendered;
    });
}

void HolographicDepthBasedImageStabilizationMain::SaveAppState()
{
    // This code sample does not store any app state.
}

void HolographicDepthBasedImageStabilizationMain::LoadAppState()
{
    // This code sample does not load any app state.
}

// Notifies classes that use Direct3D device resources that the device resources
// need to be released before this method returns.
void HolographicDepthBasedImageStabilizationMain::OnDeviceLost()
{
    m_depthBufferPlaneFinder.ReleaseDeviceDependentResources();

#ifdef DRAW_SAMPLE_CONTENT
    m_spinningPlaneRenderer->ReleaseDeviceDependentResources();
    m_depthPlaneRenderer->CreateDeviceDependentResources();
#endif
}

// Notifies classes that use Direct3D device resources that the device resources
// may now be recreated.
void HolographicDepthBasedImageStabilizationMain::OnDeviceRestored()
{
    m_depthBufferPlaneFinder.CreateDeviceDependentResources();

#ifdef DRAW_SAMPLE_CONTENT
    m_spinningPlaneRenderer->CreateDeviceDependentResources();
    m_depthPlaneRenderer->CreateDeviceDependentResources();
#endif
}

void HolographicDepthBasedImageStabilizationMain::OnLocatabilityChanged(SpatialLocator^ sender, Object^ args)
{
    switch (sender->Locatability)
    {
    case SpatialLocatability::Unavailable:
        // Holograms cannot be rendered.
        {
            String^ message = L"Warning! Positional tracking is " +
                                        sender->Locatability.ToString() + L".\n";
            OutputDebugStringW(message->Data());
        }
        break;

    // In the following three cases, it is still possible to place holograms using a
    // SpatialLocatorAttachedFrameOfReference.
    case SpatialLocatability::PositionalTrackingActivating:
        // The system is preparing to use positional tracking.

    case SpatialLocatability::OrientationOnly:
        // Positional tracking has not been activated.

    case SpatialLocatability::PositionalTrackingInhibited:
        // Positional tracking is temporarily inhibited. User action may be required
        // in order to restore positional tracking.
        break;

    case SpatialLocatability::PositionalTrackingActive:
        // Positional tracking is active. World-locked content can be rendered.
        break;
    }
}

void HolographicDepthBasedImageStabilizationMain::OnCameraAdded(
    HolographicSpace^ sender,
    HolographicSpaceCameraAddedEventArgs^ args
    )
{
    Deferral^ deferral = args->GetDeferral();
    HolographicCamera^ holographicCamera = args->Camera;
    create_task([this, deferral, holographicCamera] ()
    {
        // Create device-based resources for the holographic camera and add it to the list of
        // cameras used for updates and rendering. Notes:
        //   * Since this function may be called at any time, the AddHolographicCamera function
        //     waits until it can get a lock on the set of holographic camera resources before
        //     adding the new camera. At 60 frames per second this wait should not take long.
        //   * A subsequent Update will take the back buffer from the RenderingParameters of this
        //     camera's CameraPose and use it to create the ID3D11RenderTargetView for this camera.
        //     Content can then be rendered for the HolographicCamera.
        m_deviceResources->AddHolographicCamera(holographicCamera);

        // Holographic frame predictions will not include any information about this camera until
        // the deferral is completed.
        deferral->Complete();
    });
}

void HolographicDepthBasedImageStabilizationMain::OnCameraRemoved(
    HolographicSpace^ sender,
    HolographicSpaceCameraRemovedEventArgs^ args
    )
{
    // Before letting this callback return, ensure that all references to the back buffer 
    // are released.
    // Since this function may be called at any time, the RemoveHolographicCamera function
    // waits until it can get a lock on the set of holographic camera resources before
    // deallocating resources for this camera. At 60 frames per second this wait should
    // not take long.
    m_deviceResources->RemoveHolographicCamera(args->Camera);
}

float3 HolographicDepthBasedImageStabilizationMain::ProjectGaze(SpatialPointerPose^ pointerPose, float const& distanceFromUser)
{
    // Get the gaze direction relative to the given coordinate system.
    const float3 headPosition = pointerPose->Head->Position;
    const float3 headDirection = pointerPose->Head->ForwardDirection;

    // The hologram is positioned two meters along the user's gaze direction.
    const float3 gazeAtTwoMeters = headPosition + (distanceFromUser * headDirection);

    return gazeAtTwoMeters;
}
