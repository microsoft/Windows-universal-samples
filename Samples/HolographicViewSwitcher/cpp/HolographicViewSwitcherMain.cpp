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
#include "HolographicViewSwitcherMain.h"
#include "Common\DirectXHelper.h"

#include <windows.graphics.directx.direct3d11.interop.h>
#include <Collection.h>

using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input::Spatial;
using namespace Windows::UI::ViewManagement;
using namespace concurrency;
using namespace std::placeholders;

namespace SDKTemplate
{
  // Loads and initializes application assets when the application is loaded.
  HolographicViewSwitcherMain::HolographicViewSwitcherMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
  {
    // Register to be notified if the device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);
  }

  void HolographicViewSwitcherMain::SetHolographicSpace(HolographicSpace^ holographicSpace)
  {
    UnregisterHolographicEventHandlers();

    m_holographicSpace = holographicSpace;

#ifdef DRAW_SAMPLE_CONTENT
    // Initialize the sample hologram.
    m_quadRenderer = std::make_unique<QuadRenderer>(m_deviceResources);

    // Initialize the input handler.
    m_spatialInputHandler = std::make_unique<SpatialInputHandler>();
#endif

    // Use the default SpatialLocator to track the motion of the device.
    m_locator = SpatialLocator::GetDefault();

    // Be able to respond to changes in the positional tracking state.
    m_locatabilityChangedToken =
      m_locator->LocatabilityChanged +=
        ref new Windows::Foundation::TypedEventHandler<SpatialLocator^, Object^>(
          std::bind(&HolographicViewSwitcherMain::OnLocatabilityChanged, this, _1, _2)
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
          std::bind(&HolographicViewSwitcherMain::OnCameraAdded, this, _1, _2)
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
          std::bind(&HolographicViewSwitcherMain::OnCameraRemoved, this, _1, _2)
        );

    // In this example, we create a reference frame attached to the device.
    m_attachedReferenceFrame = m_locator->CreateAttachedFrameOfReferenceAtCurrentHeading();

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

  void HolographicViewSwitcherMain::UnregisterHolographicEventHandlers()
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
      // Clear previous event registrations.

      if (m_locatabilityChangedToken.Value != 0)
      {
        m_locator->LocatabilityChanged -= m_locatabilityChangedToken;
        m_locatabilityChangedToken.Value = 0;
      }
    }
  }

  HolographicViewSwitcherMain::~HolographicViewSwitcherMain()
  {
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);

    UnregisterHolographicEventHandlers();
  }

  // Updates the application state once per frame.
  HolographicFrame^ HolographicViewSwitcherMain::Update()
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
    SpatialCoordinateSystem^ currentCoordinateSystem =
      m_attachedReferenceFrame->GetStationaryCoordinateSystemAtTimestamp(prediction->Timestamp);

    m_timer.Tick([&]()
    {
      // Check for new input state since the last frame.
      SpatialInteractionSourceState^ pointerState = m_spatialInputHandler->CheckForInput();
      if (pointerState != nullptr)
      {
        // When a Pressed gesture is detected, the application asynchronously switches back to the main (XAML) view
        auto mainView = CoreApplication::MainView;

        // We must launch the switch from the main view's UI thread so we can access its view ID
        mainView->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
          auto viewId = ApplicationView::GetForCurrentView()->Id;

          CoreApplication::MainView->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([viewId]()
          {
            auto asyncAction = ApplicationViewSwitcher::SwitchAsync(viewId, ApplicationView::GetForCurrentView()->Id);
          }));
        }));
      }

#ifdef DRAW_SAMPLE_CONTENT
      SpatialPointerPose^ pose = SpatialPointerPose::TryGetAtTimestamp(currentCoordinateSystem, prediction->Timestamp);
      m_quadRenderer->UpdateHologramPosition(pose, m_timer);

      m_quadRenderer->Update(m_timer);
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
      // In this example, we set position, normal, and velocity for a tag-along quad.
      float3 const& focusPointPosition = m_quadRenderer->GetPosition();
      float3        focusPointNormal = focusPointPosition == float3(0.f) ? float3(0.f, 0.f, 1.f) : -normalize(focusPointPosition);
      float3 const& focusPointVelocity = m_quadRenderer->GetVelocity();

      // Set the focus point.
      SpatialCoordinateSystem^ coordinateSystemToUse = currentCoordinateSystem;
      renderingParameters->SetFocusPoint(
        coordinateSystemToUse,
        focusPointPosition,
        focusPointNormal,
        focusPointVelocity
      );
#endif
    }

    // The holographic frame will be used to get up-to-date view and projection matrices and
    // to present the swap chain.
    return holographicFrame;
  }

  // Renders the current frame to each holographic camera, according to the
  // current application and spatial positioning state. Returns true if the
  // frame was rendered to at least one camera.
  bool HolographicViewSwitcherMain::Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame)
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
      // Up-to-date frame predictions enhance the effectiveness of image stabilization and
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
        ID3D11RenderTargetView* const targets[1] = { pCameraResources->GetBackBufferRenderTargetView() };
        context->OMSetRenderTargets(1, targets, depthStencilView);

        // Clear the back buffer and depth stencil view.
        context->ClearRenderTargetView(targets[0], DirectX::Colors::Transparent);
        context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // The view and projection matrices for each holographic camera will change
        // every frame. This function refreshes the data in the constant buffer for
        // the holographic camera indicated by cameraPose.
        pCameraResources->UpdateViewProjectionBuffer(
          m_deviceResources,
          cameraPose,
          m_attachedReferenceFrame->GetStationaryCoordinateSystemAtTimestamp(prediction->Timestamp)
        );

        // Attach the view/projection constant buffer for this camera to the graphics pipeline.
        bool cameraActive = pCameraResources->AttachViewProjectionBuffer(m_deviceResources);

#ifdef DRAW_SAMPLE_CONTENT
        // This example content can always be rendered, as long as the content and Direct3D
        // device-based resources are done loading.
        if (cameraActive)
        {
          // Draw the sample hologram.
          m_quadRenderer->Render();
        }
#endif
        atLeastOneCameraRendered = true;
      }

      return atLeastOneCameraRendered;
    });
  }

  // Consumes any waiting input event, and discards it
  void HolographicViewSwitcherMain::ResetInput()
  {
    // This "consumes" the click event
    SpatialInteractionSourceState^ pointerState = m_spatialInputHandler->CheckForInput();
  }

  void HolographicViewSwitcherMain::SaveAppState()
  {
    // This code sample does not persist any app state between sessions.
  }

  void HolographicViewSwitcherMain::LoadAppState()
  {
    // This code sample does not persist any app state between sessions.
  }

  // Notifies classes that use Direct3D device resources that the device resources
  // need to be released before this method returns.
  void HolographicViewSwitcherMain::OnDeviceLost()
  {
#ifdef DRAW_SAMPLE_CONTENT
    m_quadRenderer->ReleaseDeviceDependentResources();
#endif
  }

  // Notifies classes that use Direct3D device resources that the device resources
  // may now be recreated.
  void HolographicViewSwitcherMain::OnDeviceRestored()
  {
#ifdef DRAW_SAMPLE_CONTENT
    m_quadRenderer->CreateDeviceDependentResources();
#endif
  }

  void HolographicViewSwitcherMain::OnLocatabilityChanged(SpatialLocator^ sender, Object^ args)
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

  void HolographicViewSwitcherMain::OnCameraAdded(
    HolographicSpace^ sender,
    HolographicSpaceCameraAddedEventArgs^ args
  )
  {
    Deferral^ deferral = args->GetDeferral();
    HolographicCamera^ holographicCamera = args->Camera;
    create_task([this, deferral, holographicCamera]()
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

  void HolographicViewSwitcherMain::OnCameraRemoved(
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
}