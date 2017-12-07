#include "pch.h"
#include "HolographicPaintMain.h"
#include "Common\DirectXHelper.h"

#include <windows.graphics.directx.direct3d11.interop.h>
#include <Collection.h>

using namespace HolographicPaint;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception;
using namespace Windows::Perception::People;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;
using namespace std::placeholders;
using namespace DirectX;

// Loads and initializes application assets when the application is loaded.
HolographicPaintMain::HolographicPaintMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    // Register to be notified if the device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    // Create a 6m x 2m whiteboard in front of the (initial position of the) user
    m_whiteboard = std::make_shared<Whiteboard>(6.0f, 2.0f, float3(0.0f, 0.0f, -3.0f));

}

void HolographicPaintMain::SetHolographicSpace(HolographicSpace^ holographicSpace)
{
    UnregisterHolographicEventHandlers();

    m_holographicSpace = holographicSpace;

    // Initialize the rendering helper
    m_renderingHelper = std::make_unique<RenderingHelper>(m_deviceResources);

    m_spatialInteractionManager = SpatialInteractionManager::GetForCurrentView();
    // Handle source detectionand loss by adding wrappers which will transform low level interactions into application-level commands
    m_sourceDetectedToken = 
        m_spatialInteractionManager->SourceDetected += 
            ref new TypedEventHandler<SpatialInteractionManager^, SpatialInteractionSourceEventArgs^>(
                std::bind(&HolographicPaintMain::OnSourceDetected, this, _1, _2));
    m_sourceLostToken =
        m_spatialInteractionManager->SourceLost +=
            ref new TypedEventHandler<SpatialInteractionManager^, SpatialInteractionSourceEventArgs^>(
                std::bind(&HolographicPaintMain::OnSourceLost, this, _1, _2));

    // SourcePressed is raised when a button is pressed (or in case of Air Tap for Hand) and we will transmit this event to the corresponding source wrapper
    // to take actions such as opening the tool selection carousel or validating a tool
    m_sourcePressedToken =
        m_spatialInteractionManager->SourcePressed +=
            ref new TypedEventHandler<SpatialInteractionManager^, SpatialInteractionSourceEventArgs^>(
                std::bind(&HolographicPaintMain::OnSourcePressed, this, _1, _2));

    // Source Updated is raised when the input state or the location of a source changes. The main usage is to display the too associated to a controller
    // at the right position
    m_sourceUpdatedToken =
        m_spatialInteractionManager->SourceUpdated +=
            ref new TypedEventHandler<SpatialInteractionManager^, SpatialInteractionSourceEventArgs^>(
                std::bind(&HolographicPaintMain::OnSourceUpdated, this, _1, _2));

    // Interaction detected is raised when the select button is pressed which will allow to delegate gesture recognition to a SpatialGestureRecognizer
    m_interactionDetectedToken =
        m_spatialInteractionManager->InteractionDetected +=
            ref new TypedEventHandler<SpatialInteractionManager^, SpatialInteractionDetectedEventArgs^>(
            std::bind(&HolographicPaintMain::OnInteractionDetected, this, _1, _2));

    // Use the default SpatialLocator to track the motion of the device.
    m_locator = SpatialLocator::GetDefault();

    // Be able to respond to changes in the positional tracking state.
    m_locatabilityChangedToken =
        m_locator->LocatabilityChanged +=
            ref new Windows::Foundation::TypedEventHandler<SpatialLocator^, Object^>(
                std::bind(&HolographicPaintMain::OnLocatabilityChanged, this, _1, _2));

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
                std::bind(&HolographicPaintMain::OnCameraAdded, this, _1, _2));

    // Respond to camera removed events by releasing resources that were created for that
    // camera.
    // When the app receives a CameraRemoved event, it releases all references to the back
    // buffer right away. This includes render target views, Direct2D target bitmaps, and so on.
    // The app must also ensure that the back buffer is not attached as a render target, as
    // shown in DeviceResources::ReleaseResourcesForBackBuffer.
    m_cameraRemovedToken =
        m_holographicSpace->CameraRemoved +=
            ref new Windows::Foundation::TypedEventHandler<HolographicSpace^, HolographicSpaceCameraRemovedEventArgs^>(
                std::bind(&HolographicPaintMain::OnCameraRemoved, this, _1, _2));

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

    // Add a few brushes variations (color and size) as well as a special tool allowing to clear the drawing
    m_tools.push_back(std::make_shared<Tool>(Windows::UI::ColorHelper::FromArgb(0xC0, 0x00, 0x00, 0xFF), .03f));
    m_tools.push_back(std::make_shared<Tool>(Windows::UI::ColorHelper::FromArgb(0xC0, 0x00, 0x00, 0xFF), .01f));
    m_tools.push_back(std::make_shared<Tool>(Windows::UI::ColorHelper::FromArgb(0xC0, 0x00, 0xFF, 0x00), .03f));
    m_tools.push_back(std::make_shared<Tool>(Windows::UI::ColorHelper::FromArgb(0xC0, 0x00, 0xFF, 0x00), .01f));
    m_tools.push_back(std::make_shared<Tool>(Windows::UI::ColorHelper::FromArgb(0xC0, 0xFF, 0x00, 0x00), .03f));
    m_tools.push_back(std::make_shared<Tool>(Windows::UI::ColorHelper::FromArgb(0xC0, 0xFF, 0x00, 0x00), .01f));

    auto clearTool = std::make_shared<ClearTool>(Windows::UI::Colors::White, .06f);
    m_clearToolIndex = m_tools.size();
    m_tools.push_back(clearTool);

}

void HolographicPaintMain::UnregisterHolographicEventHandlers()
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

    if (m_spatialInteractionManager != nullptr)
    {
        m_spatialInteractionManager->SourceDetected -= m_sourceDetectedToken;
        m_spatialInteractionManager->SourceLost -= m_sourceLostToken;
        m_spatialInteractionManager->SourcePressed -= m_sourcePressedToken;
        m_spatialInteractionManager->SourceUpdated -= m_sourceUpdatedToken;
        m_spatialInteractionManager->InteractionDetected -= m_interactionDetectedToken;
    }
}

HolographicPaintMain::~HolographicPaintMain()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);

    UnregisterHolographicEventHandlers();
}

// Updates the application state once per frame.
HolographicFrame^ HolographicPaintMain::Update()
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

    m_timer.Tick([&]()
    {
        if (m_renderingHelper->IsLoadingComplete())
        {
            const auto context = m_deviceResources->GetD3DDeviceContext();
            switch (m_currentMode)
            {
            case ApplicationMode::ChooseTool:
                m_toolsCarousel.Update(m_tools, m_currentToolIndex, m_timer);
                for (auto& tool : m_tools)
                {
                    tool->Update(context);
                }
                break;
            case ApplicationMode::Drawing:
            {

                for (auto& source : m_sources)
                {
                    source->Update(context);

                }
                m_whiteboard->Update(context);
                for (auto& stroke : m_strokes)
                {
                    stroke->Update(context);
                }
            }
            break;
            }
        }
    });

    // We complete the frame update by using information about our content positioning
    // to set the focus point.

    for (auto cameraPose : prediction->CameraPoses)
    {
        // The HolographicCameraRenderingParameters class provides access to set
        // the image stabilization parameters.
        HolographicCameraRenderingParameters^ renderingParameters = holographicFrame->GetRenderingParameters(cameraPose);

        // SetFocusPoint informs the system about a specific point in your scene to
        // prioritize for image stabilization. The focus point is set independently
        // for each holographic camera.
        // You should set the focus point near the content that the user is looking at.
        // In this example, we put the focus point at the center of the sample hologram,
        // since that is the only hologram available for the user to focus on.
        // You can also set the relative velocity and facing of that content; the sample
        // hologram is at a fixed point so we only need to indicate its position.
        renderingParameters->SetFocusPoint(currentCoordinateSystem, m_focusPoint);
    }

    // The holographic frame will be used to get up-to-date view and projection matrices and
    // to present the swap chain.
    return holographicFrame;
}

// Renders the current frame to each holographic camera, according to the
// current application and spatial positioning state. Returns true if the
// frame was rendered to at least one camera.
bool HolographicPaintMain::Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame)
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
            const auto depthStencilView = pCameraResources->GetDepthStencilView();
            const auto blendState = pCameraResources->GetBlendState();

            // Set render targets to the current holographic camera.
            ID3D11RenderTargetView *const targets[1] = { pCameraResources->GetBackBufferRenderTargetView() };
            context->OMSetRenderTargets(1, targets, depthStencilView);

            // Set blend state
            context->OMSetBlendState(blendState, nullptr, 0xffffffff);

            // Clear the back buffer and depth stencil view.
            context->ClearRenderTargetView(targets[0], DirectX::Colors::Transparent);
            context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            // Notes regarding holographic content:
            //    * For drawing, remember that you have the potential to fill twice as many pixels
            //      in a stereoscopic render target as compared to a non-stereoscopic render target
            //      of the same resolution. Avoid unnecessary or repeated writes to the same pixel,
            //      and only draw holograms that the user can see.
            //    * To help occlude hologram geometry, you can create a depth map using geometry
            //      data obtained via the surface mapping APIs. You can use this depth map to avoid
            //      rendering holograms that are intended to be hidden behind tables, walls,
            //      monitors, and so on.
            //    * Black pixels will appear transparent to the user wearing the device, but you
            //      should still use alpha blending to draw semitransparent holograms. You should
            //      also clear the screen to Transparent as shown above.
            //


            // The view and projection matrices for each holographic camera will change
            // every frame. This function refreshes the data in the constant buffer for
            // the holographic camera indicated by cameraPose.
            pCameraResources->UpdateViewProjectionBuffer(m_deviceResources, cameraPose, m_referenceFrame->CoordinateSystem);

            // Attach the view/projection constant buffer for this camera to the graphics pipeline.
            bool cameraActive = pCameraResources->AttachViewProjectionBuffer(m_deviceResources);

            // Only render world-locked content when positional tracking is active.
            if (cameraActive && m_renderingHelper->PrepareRendering())
            {
                switch (m_currentMode)
                {
                case ApplicationMode::Drawing:
                    m_whiteboard->Render(context, m_renderingHelper.get());

                    for (auto& source : m_sources)
                    {
                        source->Render(context, m_renderingHelper.get());
                    }
                    for (auto& stroke : m_strokes)
                    {
                        stroke->Render(context, m_renderingHelper.get());
                    }
                    break;
                case ApplicationMode::ChooseTool:
                    for (auto& tool : m_tools)
                    {
                        tool->Render(context, m_renderingHelper.get());
                    }
                    break;
                }
            }
            atLeastOneCameraRendered = true;
        }

        return atLeastOneCameraRendered;
    });
}

// Notifies classes that use Direct3D device resources that the device resources
// need to be released before this method returns.
void HolographicPaintMain::OnDeviceLost()
{
    m_renderingHelper->ReleaseDeviceDependentResources();
}

// Notifies classes that use Direct3D device resources that the device resources
// may now be recreated.
void HolographicPaintMain::OnDeviceRestored()
{
    m_renderingHelper->CreateDeviceDependentResources();
}

void HolographicPaintMain::OnLocatabilityChanged(SpatialLocator^ sender, Object^ args)
{
    switch (sender->Locatability)
    {
    case SpatialLocatability::Unavailable:
        // Holograms cannot be rendered.
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

void HolographicPaintMain::OnCameraAdded(
    HolographicSpace^ sender,
    HolographicSpaceCameraAddedEventArgs^ args
)
{
    Deferral^ deferral = args->GetDeferral();
    HolographicCamera^ holographicCamera = args->Camera;
    create_task([this, deferral, holographicCamera]()
    {
        //
        // TODO: Allocate resources for the new camera and load any content specific to
        //       that camera. Note that the render target size (in pixels) is a property
        //       of the HolographicCamera object, and can be used to create off-screen
        //       render targets that match the resolution of the HolographicCamera.
        //

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

void HolographicPaintMain::OnCameraRemoved(
    HolographicSpace^ sender,
    HolographicSpaceCameraRemovedEventArgs^ args
)
{
    create_task([this]()
    {
        //
        // TODO: Asynchronously unload or deactivate content resources (not back buffer 
        //       resources) that are specific only to the camera that was removed.
        //
    });

    // Before letting this callback return, ensure that all references to the back buffer 
    // are released.
    // Since this function may be called at any time, the RemoveHolographicCamera function
    // waits until it can get a lock on the set of holographic camera resources before
    // deallocating resources for this camera. At 60 frames per second this wait should
    // not take long.
    m_deviceResources->RemoveHolographicCamera(args->Camera);
}

void HolographicPaintMain::OnSourceDetected(SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
{
    SpatialInteractionSourceState^ state = args->State;
    SpatialInteractionSource^ source = state->Source;

    // We are only interested in sources which can give position:
    //   Hands on HoloLens
    //   6DOF Controllers
    if ((source->Kind == SpatialInteractionSourceKind::Hand)
        || ((source->Kind == SpatialInteractionSourceKind::Controller) && source->IsPointingSupported))
    {
        std::shared_ptr<SourceHandler> sourceHandler;
        auto it = m_sourceMap.find(source->Id);
        if (it == m_sourceMap.end())
        {
            if (SourceHandler::Make(&sourceHandler, source, this, m_whiteboard))
            {
                sourceHandler->SetCurrentTool(m_tools[0]);
                sourceHandler->SetApplicationMode(m_currentMode);
                sourceHandler->OnSourceUpdated(state, m_referenceFrame->CoordinateSystem);
                m_sourceMap[source->Id] = sourceHandler;
                m_sources.push_back(sourceHandler);
            }
        }
    }
}

void HolographicPaintMain::OnSourceLost(SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
{
    SpatialInteractionSourceState^ state = args->State;
    SpatialInteractionSource^ source = state->Source;
    const unsigned int sourceId = source->Id;
    SourceHandler* sourceHandler = nullptr;

    {
        auto it = m_sourceMap.find(sourceId);
        if (it != m_sourceMap.end())
        {
            sourceHandler = it->second.get();
            m_sourceMap.erase(it);
        }
    }

    if (sourceHandler != nullptr)
    {
        auto it = std::find_if(m_sources.begin(), m_sources.end(), [sourceHandler]
        (const std::shared_ptr<SourceHandler>& controller)
        {
            return controller.get() == sourceHandler;
        });
        if (it != m_sources.end())
        {
            m_sources.erase(it);
        }

        if ((m_currentMode == ApplicationMode::ChooseTool) && (m_currentSourceId == sourceId))
        {
            // Hands are lost / retrieved more easily than controllers
            // Therefore we stay in "ChooseTool" mode if the source was Hand
            if (source->Kind != SpatialInteractionSourceKind::Hand)
            {
                OnSelectTool(nullptr);
            }
        }
    }
}


void HolographicPaintMain::OnSourcePressed(SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
{
    SpatialInteractionSourceState^ state = args->State;
    SpatialInteractionSource^ source = state->Source;
    SourceHandler* sourceHandler = GetSourceHandler(source->Id);
    if (sourceHandler != nullptr)
    {
        sourceHandler->OnSourcePressed(args);
    }
}

void HolographicPaintMain::OnSourceUpdated(SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
{
    SpatialInteractionSourceState^ state = args->State;
    SpatialInteractionSource^ source = state->Source;
    SourceHandler* sourceHandler = GetSourceHandler(source->Id);
    if (sourceHandler != nullptr)
    {
        sourceHandler->OnSourceUpdated(state, m_referenceFrame->CoordinateSystem);
    }
}

void HolographicPaintMain::PositionTools(SpatialPointerPose^ pointerPose, HandAssignment side)
{
    // Get the gaze direction relative to the given coordinate system.
    const float3 headPosition = pointerPose->Head->Position;
    const float3 headDirection(pointerPose->Head->ForwardDirection.x, 0.0, pointerPose->Head->ForwardDirection.z);
    const float3 headUp = pointerPose->Head->UpDirection;

    const XMVECTOR headRightVector = XMVector3Cross(XMLoadFloat3(&headDirection), XMLoadFloat3(&headUp));
    float3 headRight;
    XMStoreFloat3(&headRight, headRightVector);

    // If we have recognized which hand is/hold the source, we move the carousel 30 cm
    // to the corresponding side
    float3 sideOffset = float3::zero();
    switch (side)
    {
    case HandAssignment::LeftHand:
        sideOffset = -.3f * headRight;
        break;
    case HandAssignment::RightHand:
        sideOffset = .3f * headRight;
        break;
    }

    XMVECTOR rotationAxisVector = XMLoadFloat3(&headUp);
    // Tilt carousel rotation axis PI/6 towards the user so that all tools are visible
    const XMMATRIX rotationAxisRotation = XMMatrixRotationAxis(headRightVector, static_cast<float>(M_PI / 6.0));
    rotationAxisVector = XMVector3Transform(rotationAxisVector, rotationAxisRotation);
    float3 rotationAxis;
    XMStoreFloat3(&rotationAxis, rotationAxisVector);

    // 50 cm radius for carousel
    const XMMATRIX scaleMatrix = XMMatrixScaling(.5f, .5f, .5f);
    const XMVECTOR headVector = XMVector3Normalize(XMLoadFloat3(&headDirection));
    const XMVECTOR scaledHeadVector = XMVector3Transform(headVector, scaleMatrix);
    float3 radius;
    XMStoreFloat3(&radius, scaledHeadVector);

    constexpr float distanceFromUser = 1.0f; // meters
    const float3 center = headPosition + sideOffset + (distanceFromUser * headDirection);

    m_toolsCarousel.SetPosition(center, radius, rotationAxis);
    m_toolsCarousel.ArrangeTools(m_tools, m_currentToolIndex);

}

size_t HolographicPaintMain::FindToolIndex(unsigned int sourceId)
{
    // Because hands are detected / lost very easily - and in particular during tool detected - they will
    // share the same tool selection
    for (auto& it : m_sources)
    {
        if (it->SourceId() == sourceId)
        {
            Tool* tool = it->GetCurrentTool();
            for (size_t idx = 0; idx < m_tools.size(); ++idx)
            {
                if (m_tools[idx].get() == tool)
                {
                    return idx;
                }
            }
        }
    }
    return static_cast<size_t>(-1);
}

void HolographicPaintMain::SetToolByIndex(unsigned int sourceId, size_t toolIndex)
{
    if (toolIndex < m_tools.size())
    {
        // Because hands are detected / lost very easily - and in particular during tool detected - they will
        // share the same tool selection
        for (auto& it : m_sources)
        {
            if (it->SourceId() == sourceId)
            {
                it->SetCurrentTool(m_tools[toolIndex]);
            }
        }
    }
}

void HolographicPaintMain::OnInteractionDetected(SpatialInteractionManager ^sender, SpatialInteractionDetectedEventArgs ^args)
{
    SpatialInteractionSource^ source = args->InteractionSource;
    SourceHandler* sourceHandler = GetSourceHandler(source->Id);
    if (sourceHandler != nullptr)
    {
        sourceHandler->OnInteractionDetected(args->Interaction, m_referenceFrame->CoordinateSystem);
    }
}


// All events are executed on the main thread, hence we do not need locks
void HolographicPaintMain::OnDPadAction(SourceHandler* sender, DPadAction action)
{
    if (ApplicationMode::ChooseTool == m_currentMode)
    {
        if (!m_toolsCarousel.IsRotating() && (sender->SourceId() == m_currentSourceId))
        {
            switch (action)
            {
            case DPadAction::Right:
                m_toolsCarousel.StartRotation(RotationDirection::RotateRight, m_timer);
                break;
            case DPadAction::Left:
                m_toolsCarousel.StartRotation(RotationDirection::RotateLeft, m_timer);
                break;
            }
        }
    }
}

void HolographicPaintMain::OnStrokeAdded(SourceHandler* sender, std::shared_ptr<IBaseRenderedObject> stroke)
{
    if (ApplicationMode::Drawing == m_currentMode)
    {
        m_strokes.push_back(stroke);
        // And set the focus point to the current position of the source
        m_focusPoint = sender->GetToolPosition();
    }
}

void HolographicPaintMain::OnEnterToolChoice(SourceHandler* sender, PerceptionTimestamp^ timestamp)
{
    if (ApplicationMode::Drawing == m_currentMode)
    {
        const unsigned int sourceId = sender->SourceId();
        SpatialPointerPose^ pointerPose = SpatialPointerPose::TryGetAtTimestamp(m_referenceFrame->CoordinateSystem, timestamp);
        if (pointerPose != nullptr)
        {
            HandAssignment hand = TryGetHandedness(sender, pointerPose);
            m_currentMode = ApplicationMode::ChooseTool;
            m_currentSourceId = sourceId;
            m_currentToolIndex = FindToolIndex(sourceId);
            PositionTools(pointerPose, hand);

            for (auto& it : m_sources) it->SetApplicationMode(m_currentMode);
        }
    }
}

void HolographicPaintMain::OnSelectTool(SourceHandler* sender)
{
    if (ApplicationMode::ChooseTool == m_currentMode)
    {
        if (sender != nullptr)
        {
            const unsigned int sourceId = sender->SourceId();
            if (sourceId != m_currentSourceId) return; // Only accept changes from selected source

            if (m_clearToolIndex == m_currentToolIndex)
            {
                // This is a special tool which can not be selected
                m_strokes.clear();
            }
            else
            {
                SetToolByIndex(m_currentSourceId, m_currentToolIndex);
            }
        }

        m_currentMode = ApplicationMode::Drawing;
        m_currentSourceId = NoSourceId;

        for (auto& it : m_sources) it->SetApplicationMode(m_currentMode);
    }
}

HandAssignment HolographicPaintMain::TryGetHandedness(SourceHandler* sender, SpatialPointerPose^ pointerPose)
{
    constexpr double handednessTestDeadZoneAngularLimit = M_PI / 8.0;
    if (sender->IsPositionCurrent())
    {
        HeadPose^ head = pointerPose->Head;
        const float3 headForward(head->ForwardDirection.x, 0, head->ForwardDirection.z);
        const float3 sourcePos = sender->GetPosition();
        const float3 headToSource(sourcePos.x - head->Position.x, 0, sourcePos.z - head->Position.z);

        // Because we just cleared the Y component of each vector, we do not need to load XMVectors to find the cross product.
        const float headForwardLength = length(headForward);
        if (headForwardLength < std::numeric_limits<float>::epsilon()) return HandAssignment::Unknown;

        const float headToSourceLength = length(headToSource);
        if (headToSourceLength < std::numeric_limits<float>::epsilon()) return HandAssignment::Unknown;

        // Because headForward and HeadToSource have a 0 y, cross product x and z are 0
        const float crossProductY = cross(headForward, headToSource).y;

        // We could already assign left or right but we'll keep a dead-zone in the center
        if (abs(crossProductY) < std::numeric_limits<float>::epsilon()) return HandAssignment::Unknown;
        const float crossProductLength = abs(crossProductY);

        const double angle = asin(crossProductLength / (headForwardLength * headToSourceLength));

        if (angle > handednessTestDeadZoneAngularLimit)
        {
            return crossProductY < 0.0 ? HandAssignment::RightHand : HandAssignment::LeftHand;
        }
        else
        {
            return HandAssignment::Unknown;
        }
    }
    else
    {
        return HandAssignment::Unknown;
    }
}
