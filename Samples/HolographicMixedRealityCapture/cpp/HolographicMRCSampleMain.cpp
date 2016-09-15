#include "pch.h"
#include "HolographicMRCSampleMain.h"
#include "Common\DirectXHelper.h"

#include <windows.graphics.directx.direct3d11.interop.h>
#include <Collection.h>


using namespace HolographicMRCSample;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;
using namespace std::placeholders;

// Loads and initializes application assets when the application is loaded.
HolographicMRCSampleMain::HolographicMRCSampleMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    m_recording(false)
{
    // Register to be notified if the device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);
}

void HolographicMRCSampleMain::SetHolographicSpace(HolographicSpace^ holographicSpace)
{
    UnregisterHolographicEventHandlers();

    m_holographicSpace = holographicSpace;

#ifdef DRAW_SAMPLE_CONTENT
    m_cursor = std::make_unique<Cursor>(0.01f, 0.01f);
    m_cursor->Initialize(m_deviceResources);

    m_mainPanel = std::make_unique<Panel>(0.20f, 0.60f, 0.01f,      // Size
                                          0.5f, 0.5f, 0.5f, 1.0f);  // Color
    m_mainPanel->Initialize(m_deviceResources);
    m_mainPanel->SetPosition(Windows::Foundation::Numerics::float3(-0.25f, 0.0f, -2.0f));

    // Init button
    m_initButton.reset(new Button(Windows::Foundation::Numerics::float3(0.14f, 0.07f, 0.01f),           // Size
                                  Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.5f, 1.0f),        // Color
                                  Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.9f, 1.0f),        // Focused Color
                                  Windows::Foundation::Numerics::float4(0.0f, 0.0f, 0.5f, 1.0f),        // Turned on Color
                                  DX::Texture_Init));
    m_initButton->Initialize(m_deviceResources);
    m_initButton->SetPosition(Windows::Foundation::Numerics::float3(0.0f, 0.25f, 0.01f));
    m_initButton->SetOnAirTapCallback(std::bind(&HolographicMRCSampleMain::OnButtonInitTapped, this));
    m_initButton->SetEnabled(true);
    m_mainPanel->AddChild(m_initButton);

    // Photo button
    m_photoButton.reset(new Button(Windows::Foundation::Numerics::float3(0.14f, 0.07f, 0.01f),          // Size
                                   Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.5f, 1.0f),       // Color
                                   Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.9f, 1.0f),       // Focused Color
                                   Windows::Foundation::Numerics::float4(0.0f, 0.0f, 0.5f, 1.0f),       // Turned on Color
                                   DX::Texture_Photo));
    m_photoButton->Initialize(m_deviceResources);
    m_photoButton->SetPosition(Windows::Foundation::Numerics::float3(0.0f, 0.15f, 0.01f));
    m_photoButton->SetOnAirTapCallback(std::bind(&HolographicMRCSampleMain::OnButtonPhotoTapped, this));
    m_photoButton->SetEnabled(false);
    m_mainPanel->AddChild(m_photoButton);

    // Video button
    m_videoButton.reset(new Button(Windows::Foundation::Numerics::float3(0.14f, 0.07f, 0.01f),          // Size
                                   Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.5f, 1.0f),       // Color
                                   Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.9f, 1.0f),       // Focused Color
                                   Windows::Foundation::Numerics::float4(0.0f, 0.0f, 0.5f, 1.0f),       // Turned on Color
                                   DX::Texture_Video));
    m_videoButton->Initialize(m_deviceResources);
    m_videoButton->SetPosition(Windows::Foundation::Numerics::float3(0.0f, 0.05f, 0.01f));
    m_videoButton->SetOnAirTapCallback(std::bind(&HolographicMRCSampleMain::OnButtonVideoTapped, this));
    m_videoButton->SetEnabled(false);
    m_mainPanel->AddChild(m_videoButton);

    // Hologram button
    m_hologramButton.reset(new Button(Windows::Foundation::Numerics::float3(0.14f, 0.07f, 0.01f),       // Size
                                      Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.5f, 1.0f),    // Color
                                      Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.9f, 1.0f),    // Focused Color
                                      Windows::Foundation::Numerics::float4(0.0f, 0.0f, 0.5f, 1.0f),    // Turned on Color
                                      DX::Texture_Hologram));
    m_hologramButton->Initialize(m_deviceResources);
    m_hologramButton->SetPosition(Windows::Foundation::Numerics::float3(0.0f, -0.05f, 0.01f));
    m_hologramButton->SetOnAirTapCallback(std::bind(&HolographicMRCSampleMain::OnButtonHologramTapped, this));
    m_hologramButton->SetEnabled(true);
    m_mainPanel->AddChild(m_hologramButton);

    // System Audio button
    m_sysAudioButton.reset(new Button(Windows::Foundation::Numerics::float3(0.14f, 0.07f, 0.01f),       // Size
                                      Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.5f, 1.0f),    // Color
                                      Windows::Foundation::Numerics::float4(1.0f, 1.0f, 0.9f, 1.0f),    // Focused Color
                                      Windows::Foundation::Numerics::float4(0.0f, 0.0f, 0.5f, 1.0f),    // Turned on Color
                                      DX::Texture_SysAudio));
    m_sysAudioButton->Initialize(m_deviceResources);
    m_sysAudioButton->SetPosition(Windows::Foundation::Numerics::float3(0.0f, -0.15f, 0.01f));
    m_sysAudioButton->SetOnAirTapCallback(std::bind(&HolographicMRCSampleMain::OnButtonSysAudioTapped, this));
    m_sysAudioButton->SetEnabled(true);
    m_mainPanel->AddChild(m_sysAudioButton);

    // Spinning Cubes
    m_cube1 = std::make_unique<SpinningCubeRenderer>(m_deviceResources);
    m_cube1->SetPosition(Windows::Foundation::Numerics::float3(0.0f, 0.0f, -2.0f)); // Front

    m_cube2 = std::make_unique<SpinningCubeRenderer>(m_deviceResources);
    m_cube2->SetPosition(Windows::Foundation::Numerics::float3(0.0f, 0.0f, 2.0f)); // Back

    m_cube3 = std::make_unique<SpinningCubeRenderer>(m_deviceResources);
    m_cube3->SetPosition(Windows::Foundation::Numerics::float3(2.0f, 0.0f, 0.0f)); // Right

    m_cube4 = std::make_unique<SpinningCubeRenderer>(m_deviceResources);
    m_cube4->SetPosition(Windows::Foundation::Numerics::float3(-2.0f, 0.0f, 0.0f)); // Left

    // Spatial Input Handler
    m_spatialInputHandler = std::make_unique<SpatialInputHandler>();

    // Media Capture Manager
    m_mediaCapture = std::make_unique<MediaCaptureManager>();
#endif

    // Use the default SpatialLocator to track the motion of the device.
    m_locator = SpatialLocator::GetDefault();

    // Be able to respond to changes in the positional tracking state.
    m_locatabilityChangedToken =
        m_locator->LocatabilityChanged +=
            ref new Windows::Foundation::TypedEventHandler<SpatialLocator^, Object^>(
                std::bind(&HolographicMRCSampleMain::OnLocatabilityChanged, this, _1, _2)
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
                std::bind(&HolographicMRCSampleMain::OnCameraAdded, this, _1, _2)
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
                std::bind(&HolographicMRCSampleMain::OnCameraRemoved, this, _1, _2)
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

void HolographicMRCSampleMain::UnregisterHolographicEventHandlers()
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

HolographicMRCSampleMain::~HolographicMRCSampleMain()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);

    UnregisterHolographicEventHandlers();
}

// Updates the application state once per frame.
HolographicFrame^ HolographicMRCSampleMain::Update()
{
    if (m_holographicSpace)
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
#endif

        m_timer.Tick([&]()
        {
            //
            // Update scene objects.
            //
            // Put time-based updates here. By default this code will run once per frame,
            // but if you change the StepTimer to use a fixed time step this code will
            // run as many times as needed to get to the current step.
            //

#ifdef DRAW_SAMPLE_CONTENT
            m_cube1->Update(m_timer);
            m_cube2->Update(m_timer);
            m_cube3->Update(m_timer);
            m_cube4->Update(m_timer);
            m_mainPanel->Update(m_timer, holographicFrame, currentCoordinateSystem, pointerState);

            DirectX::BoundingOrientedBox panelBoundingBox;
            m_mainPanel->GetBoundingBox(panelBoundingBox);

            auto pointerPose = SpatialPointerPose::TryGetAtTimestamp(currentCoordinateSystem, prediction->Timestamp);
            if (pointerPose != nullptr)
            {
                // Get the gaze direction relative to the given coordinate system.
                const float3 position = pointerPose->Head->Position;
                const float3 direction = pointerPose->Head->ForwardDirection;

                DirectX::XMFLOAT3 headPosition = DirectX::XMFLOAT3(position.x, position.y, position.z);
                DirectX::XMFLOAT3 headDirection = DirectX::XMFLOAT3(direction.x, direction.y, direction.z);
                float distance = 0.0f;

                if (panelBoundingBox.Intersects(XMLoadFloat3(&headPosition), XMLoadFloat3(&headDirection), distance))
                {
                    m_cursor->SetDistance(distance - 0.05f);
                }
                else
                {
                    m_cursor->SetDistance(2.0f);
                }
            }
            else
            {
                m_cursor->SetDistance(2.0f);
            }

            m_cursor->Update(m_timer, holographicFrame, currentCoordinateSystem, nullptr);
#endif
        });

        // We complete the frame update by using information about our content positioning
        // to set the focus point.

        for (auto cameraPose : prediction->CameraPoses)
        {
#ifdef DRAW_SAMPLE_CONTENT
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
            renderingParameters->SetFocusPoint(
                currentCoordinateSystem,
                m_mainPanel->GetPosition()
            );
#endif
        }

        // The holographic frame will be used to get up-to-date view and projection matrices and
        // to present the swap chain.
        return holographicFrame;
    }

    return nullptr;
}

// Renders the current frame to each holographic camera, according to the
// current application and spatial positioning state. Returns true if the
// frame was rendered to at least one camera.
bool HolographicMRCSampleMain::Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame)
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return false;
    }

    //
    // Take care of any tasks that are not specific to an individual holographic
    // camera. This includes anything that doesn't need the final view or projection
    // matrix, such as lighting maps.
    //

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
            context->OMSetBlendState(blendState, NULL, 0xffffffff);

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
                m_cube1->Render();
                m_cube2->Render();
                m_cube3->Render();
                m_cube4->Render();
                m_mainPanel->Render();
                m_cursor->Render();
            }
#endif
            atLeastOneCameraRendered = true;
        }

        return atLeastOneCameraRendered;
    });
}

void HolographicMRCSampleMain::SaveAppState()
{
}

void HolographicMRCSampleMain::LoadAppState()
{
}

// Notifies classes that use Direct3D device resources that the device resources
// need to be released before this method returns.
void HolographicMRCSampleMain::OnDeviceLost()
{
#ifdef DRAW_SAMPLE_CONTENT
    m_cube1->ReleaseDeviceDependentResources();
    m_cube2->ReleaseDeviceDependentResources();
    m_cube3->ReleaseDeviceDependentResources();
    m_cube4->ReleaseDeviceDependentResources();

    m_cursor->ReleaseDeviceDependentResources();
    m_mainPanel->ReleaseDeviceDependentResources();
#endif
}

// Notifies classes that use Direct3D device resources that the device resources
// may now be recreated.
void HolographicMRCSampleMain::OnDeviceRestored()
{
#ifdef DRAW_SAMPLE_CONTENT
    m_cube1->CreateDeviceDependentResources();
    m_cube2->CreateDeviceDependentResources();
    m_cube3->CreateDeviceDependentResources();
    m_cube4->CreateDeviceDependentResources();

    m_cursor->CreateDeviceDependentResources();
    m_mainPanel->CreateDeviceDependentResources();
#endif
}

void HolographicMRCSampleMain::OnLocatabilityChanged(SpatialLocator^ sender, Object^ args)
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

void HolographicMRCSampleMain::OnCameraAdded(
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

void HolographicMRCSampleMain::OnCameraRemoved(
    HolographicSpace^ sender,
    HolographicSpaceCameraRemovedEventArgs^ args
    )
{
    create_task([this]()
    {
    });

    // Before letting this callback return, ensure that all references to the back buffer 
    // are released.
    // Since this function may be called at any time, the RemoveHolographicCamera function
    // waits until it can get a lock on the set of holographic camera resources before
    // deallocating resources for this camera. At 60 frames per second this wait should
    // not take long.
    m_deviceResources->RemoveHolographicCamera(args->Camera);
}

void HolographicMRCSampleMain::OnButtonInitTapped()
{
    DisableAllButtons();

    try
    {
        Concurrency::create_task(m_mediaCapture->InitializeAsync()).then([this]()
        {
            m_photoButton->SetEnabled(true);
            m_videoButton->SetEnabled(true);
            m_hologramButton->SetEnabled(true);
            m_sysAudioButton->SetEnabled(true);
        });
    }
    catch (Platform::Exception ^e)
    {
        OutputDebugString(e->Message->Data());
    }
}

void HolographicMRCSampleMain::OnButtonVideoTapped()
{
    DisableAllButtons();

    if (m_recording)
    {
        try
        {
            // Stop Recording
            m_mediaCapture->StopRecordingAsync().then([this]()
            {
                m_recording = false;

                m_initButton->SetEnabled(false);
                m_photoButton->SetEnabled(m_mediaCapture->CanTakePhoto());
                m_videoButton->SetEnabled(true);
                m_hologramButton->SetEnabled(true);
                m_sysAudioButton->SetEnabled(true);
            });
        }
        catch (Platform::Exception ^e)
        {
            OutputDebugString(e->Message->Data());
        }
    }
    else
    {
        try
        {
            // Start Recording
            m_mediaCapture->StartRecordingAsync().then([this]()
            {
                m_recording = true;

                m_initButton->SetEnabled(false);
                m_photoButton->SetEnabled(m_mediaCapture->CanTakePhoto());
                m_videoButton->SetEnabled(true);
                m_hologramButton->SetEnabled(false);
                m_sysAudioButton->SetEnabled(false);
            });
        }
        catch (Platform::Exception ^e)
        {
            OutputDebugString(e->Message->Data());
        }
    }
}

void HolographicMRCSampleMain::OnButtonPhotoTapped()
{
    DisableAllButtons();

    try
    {
        m_mediaCapture->TakePhotoAsync().then([this]()
        {
            m_initButton->SetEnabled(false);
            m_photoButton->SetEnabled(m_mediaCapture->CanTakePhoto());
            m_videoButton->SetEnabled(true);
            m_hologramButton->SetEnabled(true);
            m_sysAudioButton->SetEnabled(true);
        });
    }
    catch (Platform::Exception ^e)
    {
        OutputDebugString(e->Message->Data());
    }
}

void HolographicMRCSampleMain::OnButtonHologramTapped()
{
    // Toggle button switch state
    m_hologramButton->SetSwitch(!m_hologramButton->GetSwitch());

    m_mediaCapture->SetHologramEnabled(m_hologramButton->GetSwitch());
}

void HolographicMRCSampleMain::OnButtonSysAudioTapped()
{
    // Toggle button switch state
    m_sysAudioButton->SetSwitch(!m_sysAudioButton->GetSwitch());

    m_mediaCapture->SetSystemAudioEnabled(m_sysAudioButton->GetSwitch());
}

void HolographicMRCSampleMain::DisableAllButtons()
{
    m_initButton->SetEnabled(false);
    m_photoButton->SetEnabled(false);
    m_videoButton->SetEnabled(false);
    m_hologramButton->SetEnabled(false);
    m_sysAudioButton->SetEnabled(false);
}
