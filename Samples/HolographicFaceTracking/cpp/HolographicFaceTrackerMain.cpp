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
#include "HolographicFaceTrackerMain.h"
#include "Common\DirectXHelper.h"

#include "Content\VideoFrameProcessor.h"
#include "Content\FaceTrackerProcessor.h"

#include "Content\SpinningCubeRenderer.h"
#include "Content\QuadRenderer.h"
#include "Content\TextRenderer.h"
#include "Content\NV12VideoTexture.h"

using namespace HolographicFaceTracker;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::Media::Devices::Core;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;

using namespace std::placeholders;

using namespace Microsoft::WRL::Wrappers;
using namespace Microsoft::WRL;

using namespace DirectX;

// Loads and initializes application assets when the application is loaded.
HolographicFaceTrackerMain::HolographicFaceTrackerMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    // Register to be notified if the device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);
}

void HolographicFaceTrackerMain::SetHolographicSpace(HolographicSpace^ holographicSpace)
{
    UnregisterHolographicEventHandlers();

    m_holographicSpace = holographicSpace;

    // Use the default SpatialLocator to track the motion of the device.
    m_locator = SpatialLocator::GetDefault();

    // Be able to respond to changes in the positional tracking state.
    m_locatabilityChangedToken =
        m_locator->LocatabilityChanged +=
            ref new Windows::Foundation::TypedEventHandler<SpatialLocator^, Object^>(
                std::bind(&HolographicFaceTrackerMain::OnLocatabilityChanged, this, _1, _2)
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
                std::bind(&HolographicFaceTrackerMain::OnCameraAdded, this, _1, _2)
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
                std::bind(&HolographicFaceTrackerMain::OnCameraRemoved, this, _1, _2)
                );

    // The simplest way to render world-locked holograms is to create a stationary reference frame
    // when the app is launched. This is roughly analogous to creating a "world" coordinate system
    // with the origin placed at the device's position as the app is launched.
    m_referenceFrame = m_locator->CreateAttachedFrameOfReferenceAtCurrentHeading();

    // First we initialize our MediaCapture and FaceTracking objects
    task<void> videoInitTask = VideoFrameProcessor::CreateAsync()
        .then([this](std::shared_ptr<VideoFrameProcessor> videoProcessor)
    {
        m_videoFrameProcessor = std::move(videoProcessor);

        return FaceTrackerProcessor::CreateAsync(m_videoFrameProcessor)
            .then([this](std::shared_ptr<FaceTrackerProcessor> faceProcessor)
        {
            m_faceTrackerProcessor = std::move(faceProcessor);
        });
    });

    // Then we can initialize our device dependent resources, which depend on the VideoFrameProcessor.
    videoInitTask.then([this]
    {
        std::vector<task<void>> deviceInitTasks = {
            DX::CreateAndInitializeAsync(m_quadRenderer, m_deviceResources),
            DX::CreateAndInitializeAsync(m_spinningCubeRenderer, m_deviceResources),
            DX::CreateAndInitializeAsync(m_textRenderer, m_deviceResources, 512u, 512u),
        };

        if (m_videoFrameProcessor)
        {
            VideoMediaFrameFormat^ videoFormat = m_videoFrameProcessor->GetCurrentFormat();
            deviceInitTasks.push_back(DX::CreateAndInitializeAsync(m_videoTexture, m_deviceResources, videoFormat->Width, videoFormat->Height));
        }

        when_all(deviceInitTasks.begin(), deviceInitTasks.end()).then([this]
        {
            // If we weren't able to create the VideoFrameProcessor, then we don't have any cameras we can use.
            // Set our status message to inform the user. Typically this should only happen on the emulator.
            if (m_videoFrameProcessor == nullptr)
            {
                m_textRenderer->RenderTextOffscreen(L"No camera available");
            }
            // Everything is good to go, so we can set our status message to inform the user when we don't detect
            // any faces.
            else
            {
                m_textRenderer->RenderTextOffscreen(L"No faces detected");
            }

            m_isReadyToRender = true;
        });
    });
}

void HolographicFaceTrackerMain::UnregisterHolographicEventHandlers()
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

HolographicFaceTrackerMain::~HolographicFaceTrackerMain()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);

    UnregisterHolographicEventHandlers();
}

void HolographicFaceTrackerMain::ProcessFaces(std::vector<BitmapBounds> const& faces, Windows::Media::Capture::Frames::MediaFrameReference^ frame, SpatialCoordinateSystem^ worldCoordSystem)
{
    VideoMediaFrameFormat^ videoFormat = frame->VideoMediaFrame->VideoFormat;
    SpatialCoordinateSystem^ cameraCoordinateSystem = frame->CoordinateSystem;
    CameraIntrinsics^ cameraIntrinsics = frame->VideoMediaFrame->CameraIntrinsics;

    IBox<float4x4>^ cameraToWorld = cameraCoordinateSystem->TryGetTransformTo(worldCoordSystem);

    // If we can't locate the world, this transform will be null.
    if (cameraToWorld == nullptr)
    {
        return;
    }

    float const textureWidthInv = 1.0f / static_cast<float>(videoFormat->Width);
    float const textureHeightInv = 1.0f / static_cast<float>(videoFormat->Height);

    // The face analysis returns very "tight fitting" rectangles.
    // We add some padding to make the visuals more appealing.
    constexpr uint32_t paddingForFaceRect = 24u;
    constexpr float averageFaceWidthInMeters = 0.15f;

    float const pixelsPerMeterAlongX = cameraIntrinsics->FocalLength.x;
    float const averagePixelsForFaceAt1Meter = pixelsPerMeterAlongX * averageFaceWidthInMeters;

    // Place the cube 25cm above the center of the face.
    float3 const cubeOffsetInWorldSpace = float3{ 0.0f, 0.25f, 0.0f };

    BitmapBounds bestRect = {};
    float3 bestRectPositionInCameraSpace = float3::zero();
    float bestDotProduct = -1.0f;

    for (BitmapBounds const& faceRect : faces)
    {
        Point const faceRectCenterPoint = {
            static_cast<float>(faceRect.X + faceRect.Width / 2u),
            static_cast<float>(faceRect.Y + faceRect.Height / 2u),
        };

        // Calculate the vector towards the face at 1 meter.
        float2 const centerOfFace = cameraIntrinsics->UnprojectAtUnitDepth(faceRectCenterPoint);

        // Add the Z component and normalize.
        float3 const vectorTowardsFace = normalize(float3{ centerOfFace.x, centerOfFace.y, -1.0f });

        // Estimate depth using the ratio of the current faceRect width with the average faceRect width at 1 meter.
        float const estimatedFaceDepth = averagePixelsForFaceAt1Meter / static_cast<float>(faceRect.Width);

        // Get the dot product between the vector towards the face and the gaze vector.
        // The closer the dot product is to 1.0, the closer the face is to the middle of the video image.
        float const dotFaceWithGaze = dot(vectorTowardsFace, -float3::unit_z());

        // Scale the vector towards the face by the depth, and add an offset for the cube.
        float3 const targetPositionInCameraSpace = vectorTowardsFace * estimatedFaceDepth;

        // Pick the faceRect that best matches the users gaze.
        if (dotFaceWithGaze > bestDotProduct)
        {
            bestDotProduct = dotFaceWithGaze;
            bestRect = faceRect;
            bestRectPositionInCameraSpace = targetPositionInCameraSpace;
        }
    }

    // Transform the cube from Camera space to World space.
    float3 const bestRectPositionInWorldspace = transform(bestRectPositionInCameraSpace, cameraToWorld->Value);

    m_spinningCubeRenderer->SetTargetPosition(bestRectPositionInWorldspace + cubeOffsetInWorldSpace);

    // Texture Coordinates are [0,1], but our FaceRect is [0,Width] and [0,Height], so we need to normalize these coordinates
    // We also add padding for the faceRects to make it more visually appealing.
    float const normalizedWidth  = static_cast<float>(bestRect.Width  + paddingForFaceRect * 2u) * textureWidthInv;
    float const normalizedHeight = static_cast<float>(bestRect.Height + paddingForFaceRect * 2u) * textureHeightInv;
    float const normalizedX      = static_cast<float>(bestRect.X      - paddingForFaceRect     ) * textureWidthInv;
    float const normalizedY      = static_cast<float>(bestRect.Y      - paddingForFaceRect     ) * textureHeightInv;

    m_quadRenderer->SetTexCoordScaleAndOffset({ normalizedWidth, normalizedHeight }, { normalizedX, normalizedY });
}

// Updates the application state once per frame.
HolographicFrame^ HolographicFaceTrackerMain::Update()
{
    if (!m_isReadyToRender)
    {
        return nullptr;
    }

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

    SpatialCoordinateSystem^ currentCoordinateSystem = m_referenceFrame->GetStationaryCoordinateSystemAtTimestamp(prediction->Timestamp);

    if (m_videoFrameProcessor && m_faceTrackerProcessor)
    {
        m_trackingFaces = m_faceTrackerProcessor->IsTrackingFaces();

        if (m_trackingFaces)
        {
            if (MediaFrameReference^ frame = m_videoFrameProcessor->GetLatestFrame())
            {
                ProcessFaces(m_faceTrackerProcessor->GetLatestFaces(), frame, currentCoordinateSystem);

                int64_t const currentTimeStamp = frame->SystemRelativeTime->Value.Duration;

                // Copy only new frames to our DirectX texture.
                if (currentTimeStamp > m_previousFrameTimestamp)
                {
                    m_videoTexture->CopyFromVideoMediaFrame(frame->VideoMediaFrame);
                    m_previousFrameTimestamp = currentTimeStamp;
                }
            }
        }
    }

    SpatialPointerPose^ pointerPose = SpatialPointerPose::TryGetAtTimestamp(currentCoordinateSystem, prediction->Timestamp);

    m_timer.Tick([&] {
        m_spinningCubeRenderer->Update(m_timer);

        // If we're tracking faces, then put the quad to the left side of the viewport, 2 meters out.
        if (m_trackingFaces)
        {
            m_quadRenderer->Update(pointerPose, float3{ -0.45f, 0.0f, -2.0f }, m_timer);
        }
        // Otherwise, put the quad centered in the viewport, 2 meters out.
        else
        {
            m_quadRenderer->ResetTexCoordScaleAndOffset();
            m_quadRenderer->Update(pointerPose, float3{ 0.0f, -0.15f, -2.0f }, m_timer);
        }
    });

    // We complete the frame update by using information about our content positioning
    // to set the focus point.
    // Next, we get a coordinate system from the attached frame of reference that is
    // associated with the current frame. Later, this coordinate system is used for
    // for creating the stereo view matrices when rendering the sample content.

    for (auto cameraPose : prediction->CameraPoses)
    {
        // The HolographicCameraRenderingParameters class provides access to set
        // the image stabilization parameters.
        HolographicCameraRenderingParameters^ renderingParameters = holographicFrame->GetRenderingParameters(cameraPose);

        // If we're tracking faces, then put the focus point on the cube
        if (m_trackingFaces)
        {
            renderingParameters->SetFocusPoint(
                currentCoordinateSystem,
                m_spinningCubeRenderer->GetPosition()
            );
        }
        // Otherwise put the focus point on status message quad.
        else
        {
            renderingParameters->SetFocusPoint(
                currentCoordinateSystem,
                m_quadRenderer->GetPosition(),
                m_quadRenderer->GetNormal(),
                m_quadRenderer->GetVelocity()
            );
        }
    }

    // The holographic frame will be used to get up-to-date view and projection matrices and
    // to present the swap chain.
    return holographicFrame;
}

// Renders the current frame to each holographic camera, according to the
// current application and spatial positioning state. Returns true if the
// frame was rendered to at least one camera.
bool HolographicFaceTrackerMain::Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame)
{
    if (!m_isReadyToRender)
    {
        return false;
    }

    SpatialCoordinateSystem^ currentCoordinateSystem = m_referenceFrame->GetStationaryCoordinateSystemAtTimestamp(holographicFrame->CurrentPrediction->Timestamp);

    // Lock the set of holographic camera resources, then draw to each camera
    // in this frame.
    return m_deviceResources->UseHolographicCameraResources<bool>(
        [this, holographicFrame, currentCoordinateSystem](std::map<UINT32, std::unique_ptr<DX::CameraResources>>& cameraResourceMap)
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
            const auto depthStencilView = pCameraResources->GetDepthStencilView();

            // Set render targets to the current holographic camera.
            ID3D11RenderTargetView *const targets[1] = { pCameraResources->GetBackBufferRenderTargetView() };
            context->OMSetRenderTargets(1, targets, depthStencilView);

            // Clear the back buffer and depth stencil view.
            context->ClearRenderTargetView(targets[0], DirectX::Colors::Transparent);
            context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            // The view and projection matrices for each holographic camera will change
            // every frame. This function refreshes the data in the constant buffer for
            // the holographic camera indicated by cameraPose.
            pCameraResources->UpdateViewProjectionBuffer(m_deviceResources, cameraPose, currentCoordinateSystem);

            // Attach the view/projection constant buffer for this camera to the graphics pipeline.
            bool cameraActive = pCameraResources->AttachViewProjectionBuffer(m_deviceResources);

            // Render world-locked content only when positional tracking is active.
            if (cameraActive)
            {
                // If we are tracking any faces, then we render the cube over their head, and the video image on the quad.
                if (m_trackingFaces)
                {
                    m_spinningCubeRenderer->Render();
                    m_quadRenderer->RenderNV12(m_videoTexture->GetLuminanceTexture(), m_videoTexture->GetChrominanceTexture());
                }
                // Otherwise we render the status message on the quad.
                else
                {
                    m_quadRenderer->RenderRGB(m_textRenderer->GetTexture());
                }
            }

            atLeastOneCameraRendered = true;
        }

        return atLeastOneCameraRendered;
    });
}

void HolographicFaceTrackerMain::SaveAppState()
{
    // This sample does not save any state on suspend.
}

void HolographicFaceTrackerMain::LoadAppState()
{
    // This sample has no state to restore on suspend.
}

// Notifies classes that use Direct3D device resources that the device resources
// need to be released before this method returns.
void HolographicFaceTrackerMain::OnDeviceLost()
{
    m_isReadyToRender = false;

    m_quadRenderer->ReleaseDeviceDependentResources();
    m_spinningCubeRenderer->ReleaseDeviceDependentResources();
    m_textRenderer->ReleaseDeviceDependentResources();
    m_videoTexture->ReleaseDeviceDependentResources();
}

// Notifies classes that use Direct3D device resources that the device resources
// may now be recreated.
void HolographicFaceTrackerMain::OnDeviceRestored()
{
    auto initTasks = {
        m_quadRenderer->CreateDeviceDependentResourcesAsync(),
        m_spinningCubeRenderer->CreateDeviceDependentResourcesAsync(),
        m_textRenderer->CreateDeviceDependentResourcesAsync(),
        m_videoTexture->CreateDeviceDependentResourcesAsync(),
    };

    when_all(initTasks.begin(), initTasks.end()).then([this] {
        m_isReadyToRender = true;
    });
}

void HolographicFaceTrackerMain::OnLocatabilityChanged(SpatialLocator^ sender, Object^ args)
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
        break;

    case SpatialLocatability::PositionalTrackingInhibited:
        // Positional tracking is temporarily inhibited. User action may be required
        // in order to restore positional tracking.
        break;

    case SpatialLocatability::PositionalTrackingActive:
        // Positional tracking is active. World-locked content can be rendered.
        break;
    }
}

void HolographicFaceTrackerMain::OnCameraAdded(
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

void HolographicFaceTrackerMain::OnCameraRemoved(
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
