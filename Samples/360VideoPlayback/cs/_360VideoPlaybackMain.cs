//
// Comment out this preprocessor definition to disable all of the
// sample content.
//
// To remove the content after disabling it:
//     * Remove the unused code from this file.
//     * Delete the Content folder provided with this template.
//
#define DRAW_SAMPLE_CONTENT

using System;
using System.Numerics;
using System.Diagnostics;
using Windows.Graphics.Holographic;
using Windows.Perception.Spatial;
using Windows.UI.Input.Spatial;

using _360VideoPlayback.Common;
using System.Threading.Tasks;
using Windows.Foundation;
using System.Collections.Generic;

#if DRAW_SAMPLE_CONTENT
using _360VideoPlayback.Content;
#endif

namespace _360VideoPlayback
{
    /// <summary>
    /// Updates, renders, and presents holographic content using Direct3D.
    /// </summary>
    internal class _360VideoPlaybackMain : IDisposable
    {

#if DRAW_SAMPLE_CONTENT
        // Renders a SPherical Video Playback . This sample content
        // is used to demonstrate world-locked rendering.
        private VideoRenderer videoRenderer;

        private SpatialInputHandler spatialInputHandler;
#endif

        // Cached reference to device resources.
        private DeviceResources deviceResources;

        // Render loop timer.
        private StepTimer timer = new StepTimer();

        // Represents the holographic space around the user.
        HolographicSpace holographicSpace;

        // SpatialLocator that is attached to the primary camera.
        SpatialLocator locator;

        // A reference frame attached to the holographic camera.
        SpatialLocatorAttachedFrameOfReference attachreferenceFrame;

        /// <summary>
        /// Loads and initializes application assets when the application is loaded.
        /// </summary>
        /// <param name="deviceResources"></param>
        public _360VideoPlaybackMain(DeviceResources deviceResources)
        {
            this.deviceResources = deviceResources;

            // Register to be notified if the Direct3D device is lost.
            this.deviceResources.DeviceLost += this.OnDeviceLost;
            this.deviceResources.DeviceRestored += this.OnDeviceRestored;
        }

        public void SetHolographicSpace(HolographicSpace holographicSpace)
        {
            this.holographicSpace = holographicSpace;

            //
            // TODO: Add code here to initialize your content.
            //

#if DRAW_SAMPLE_CONTENT
            // Initialize the sample hologram.
            videoRenderer = new VideoRenderer(deviceResources);

            spatialInputHandler = new SpatialInputHandler();
#endif

            // Use the default SpatialLocator to track the motion of the device.
            locator = SpatialLocator.GetDefault();

            // Be able to respond to changes in the positional tracking state.
            locator.LocatabilityChanged += this.OnLocatabilityChanged;

            // Respond to camera added events by creating any resources that are specific
            // to that camera, such as the back buffer render target view.
            // When we add an event handler for CameraAdded, the API layer will avoid putting
            // the new camera in new HolographicFrames until we complete the deferral we created
            // for that handler, or return from the handler without creating a deferral. This
            // allows the app to take more than one frame to finish creating resources and
            // loading assets for the new holographic camera.
            // This function should be registered before the app creates any HolographicFrames.
            holographicSpace.CameraAdded += this.OnCameraAdded;

            // Respond to camera removed events by releasing resources that were created for that
            // camera.
            // When the app receives a CameraRemoved event, it releases all references to the back
            // buffer right away. This includes render target views, Direct2D target bitmaps, and so on.
            // The app must also ensure that the back buffer is not attached as a render target, as
            // shown in DeviceResources.ReleaseResourcesForBackBuffer.
            holographicSpace.CameraRemoved += this.OnCameraRemoved;

            // The simplest way to render video Frame is to create a stationary reference frame
            // when the app is launched. This is roughly analogous to creating a "world" coordinate system
            // with the origin placed at the device's position as the app is launched.
            attachreferenceFrame = locator.CreateAttachedFrameOfReferenceAtCurrentHeading();

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

        public void Dispose()
        {
#if DRAW_SAMPLE_CONTENT
            if (videoRenderer != null)
            {
                videoRenderer.ReleaseDeviceDependentResources();
                videoRenderer.Dispose();
                videoRenderer = null;
            }
#endif
        }

        /// <summary>
        /// Updates the application state once per frame.
        /// </summary>
        public HolographicFrame Update()
        {
            // Before doing the timer update, there is some work to do per-frame
            // to maintain holographic rendering. First, we will get information
            // about the current frame.

            // The HolographicFrame has information that the app needs in order
            // to update and render the current frame. The app begins each new
            // frame by calling CreateNextFrame.
            HolographicFrame holographicFrame = holographicSpace.CreateNextFrame();

            // Get a prediction of where holographic cameras will be when this frame
            // is presented.
            HolographicFramePrediction prediction = holographicFrame.CurrentPrediction;

            // Back buffers can change from frame to frame. Validate each buffer, and recreate
            // resource views and depth buffers as needed.
            deviceResources.EnsureCameraResources(holographicFrame, prediction);

            // Next, we get a coordinate system from the attached frame of reference that is
            // associated with the current frame. Later, this coordinate system is used for
            // for creating the stereo view matrices when rendering the sample content.
            SpatialCoordinateSystem currentCoordinateSystem = attachreferenceFrame.GetStationaryCoordinateSystemAtTimestamp(prediction.Timestamp);//referenceFrame.CoordinateSystem;

            timer.Tick(() =>
            {
                //
                // TODO: Update scene objects.
                //
                // Put time-based updates here. By default this code will run once per frame,
                // but if you change the StepTimer to use a fixed time step this code will
                // run as many times as needed to get to the current step.
                //

#if DRAW_SAMPLE_CONTENT
                videoRenderer.Update(timer);
#endif
            });

            // We complete the frame update by using information about our content positioning
            // to set the focus point.
            foreach (var cameraPose in prediction.CameraPoses)
            {
#if DRAW_SAMPLE_CONTENT
                // The HolographicCameraRenderingParameters class provides access to set
                // the image stabilization parameters.
                HolographicCameraRenderingParameters renderingParameters = holographicFrame.GetRenderingParameters(cameraPose);

                // SetFocusPoint informs the system about a specific point in your scene to
                // prioritize for image stabilization. The focus point is set independently
                // for each holographic camera.
                // You should set the focus point near the content that the user is looking at.
                // In this example, we put the focus point at the center of the sample hologram,
                // since that is the only hologram available for the user to focus on.
                // You can also set the relative velocity and facing of that content; the sample
                // hologram is at a fixed point so we only need to indicate its position.
                Vector3 position = new Vector3(0.0f, 0.0f, 0.0f);
                renderingParameters.SetFocusPoint(
                currentCoordinateSystem, position
                );
#endif
            }

            // The holographic frame will be used to get up-to-date view and projection matrices and
            // to present the swap chain.
            return holographicFrame;
        }

        /// <summary>
        /// Renders the current frame to each holographic display, according to the
        /// current application and spatial positioning state. Returns true if the
        /// frame was rendered to at least one display.
        /// </summary>
        public bool Render(ref HolographicFrame holographicFrame)
        {
            // Don't try to render anything before the first Update.
            if (timer.FrameCount == 0)
            {
                return false;
            }

            // Up-to-date frame predictions enhance the effectiveness of image stablization and
            // allow more accurate positioning of holograms.
            holographicFrame.UpdateCurrentPrediction();
            HolographicFramePrediction prediction = holographicFrame.CurrentPrediction;

            // Lock the set of holographic camera resources, then draw to each camera
            // in this frame.
            return deviceResources.UseHolographicCameraResources(
                (Dictionary<uint, CameraResources> cameraResourceDictionary) =>
                {
                    bool atLeastOneCameraRendered = false;

                    foreach (var cameraPose in prediction.CameraPoses)
                    {
                        // This represents the device-based resources for a HolographicCamera.
                        CameraResources cameraResources = cameraResourceDictionary[cameraPose.HolographicCamera.Id];

                        // Get the device context.
                        var context = deviceResources.D3DDeviceContext;
                        var renderTargetView = cameraResources.BackBufferRenderTargetView;
                        var depthStencilView = cameraResources.DepthStencilView;

                        // Set render targets to the current holographic camera.
                        context.OutputMerger.SetRenderTargets(depthStencilView, renderTargetView);

                        // Clear the back buffer and depth stencil view.
                        SharpDX.Mathematics.Interop.RawColor4 transparent = new SharpDX.Mathematics.Interop.RawColor4(0.0f, 0.0f, 0.0f, 0.0f);
                        context.ClearRenderTargetView(renderTargetView, transparent);
                        context.ClearDepthStencilView(
                            depthStencilView,
                            SharpDX.Direct3D11.DepthStencilClearFlags.Depth | SharpDX.Direct3D11.DepthStencilClearFlags.Stencil,
                            1.0f,
                            0);

                        //
                        // TODO: Replace the sample content with your own content.
                        //
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
                        cameraResources.UpdateViewProjectionBuffer(deviceResources, cameraPose, attachreferenceFrame.GetStationaryCoordinateSystemAtTimestamp(prediction.Timestamp)/*referenceFrame.CoordinateSystem*/);

                        // Attach the view/projection constant buffer for this camera to the graphics pipeline.
                        bool cameraActive = cameraResources.AttachViewProjectionBuffer(deviceResources);

#if DRAW_SAMPLE_CONTENT
                        // Only render world-locked content when positional tracking is active.
                        if (cameraActive)
                        {
                            // Draw the sample hologram.
                            videoRenderer.Render();
                        }
#endif
                        atLeastOneCameraRendered = true;
                    }

                    return atLeastOneCameraRendered;
                });
        }

        public void SaveAppState()
        {
            //
            // TODO: Insert code here to save your app state.
            //       This method is called when the app is about to suspend.
            //
            //       For example, store information in the SpatialAnchorStore.
            //
        }

        public void LoadAppState()
        {
            //
            // TODO: Insert code here to load your app state.
            //       This method is called when the app resumes.
            //
            //       For example, load information from the SpatialAnchorStore.
            //
        }


        /// <summary>
        /// Notifies renderers that device resources need to be released.
        /// </summary>
        public void OnDeviceLost(Object sender, EventArgs e)
        {

#if DRAW_SAMPLE_CONTENT
            videoRenderer.ReleaseDeviceDependentResources();
#endif

        }

        /// <summary>
        /// Notifies renderers that device resources may now be recreated.
        /// </summary>
        public void OnDeviceRestored(Object sender, EventArgs e)
        {
#if DRAW_SAMPLE_CONTENT
            videoRenderer.CreateDeviceDependentResourcesAsync();
#endif
        }

        void OnLocatabilityChanged(SpatialLocator sender, Object args)
        {
            switch (sender.Locatability)
            {
                case SpatialLocatability.Unavailable:
                    // Holograms cannot be rendered.
                    {
                        String message = "Warning! Positional tracking is " + sender.Locatability + ".";
                        Debug.WriteLine(message);
                    }
                    break;

                // In the following three cases, it is still possible to place holograms using a
                // SpatialLocatorAttachedFrameOfReference.
                case SpatialLocatability.PositionalTrackingActivating:
                // The system is preparing to use positional tracking.

                case SpatialLocatability.OrientationOnly:
                // Positional tracking has not been activated.

                case SpatialLocatability.PositionalTrackingInhibited:
                    // Positional tracking is temporarily inhibited. User action may be required
                    // in order to restore positional tracking.
                    break;

                case SpatialLocatability.PositionalTrackingActive:
                    // Positional tracking is active. World-locked content can be rendered.
                    break;
            }
        }

        public void OnCameraAdded(
            HolographicSpace sender,
            HolographicSpaceCameraAddedEventArgs args
            )
        {
            Deferral deferral = args.GetDeferral();
            HolographicCamera holographicCamera = args.Camera;

            Task task1 = new Task(() =>
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
                deviceResources.AddHolographicCamera(holographicCamera);

                // Holographic frame predictions will not include any information about this camera until
                // the deferral is completed.
                deferral.Complete();
            });
            task1.Start();
        }

        public void OnCameraRemoved(
            HolographicSpace sender,
            HolographicSpaceCameraRemovedEventArgs args
            )
        {
            Task task2 = new Task(() =>
            {
                //
                // TODO: Asynchronously unload or deactivate content resources (not back buffer
                //       resources) that are specific only to the camera that was removed.
                //
            });
            task2.Start();

            // Before letting this callback return, ensure that all references to the back buffer
            // are released.
            // Since this function may be called at any time, the RemoveHolographicCamera function
            // waits until it can get a lock on the set of holographic camera resources before
            // deallocating resources for this camera. At 60 frames per second this wait should
            // not take long.
            deviceResources.RemoveHolographicCamera(args.Camera);
        }
        public void CreateVideoShaders()
        {
            videoRenderer.CreateDeviceDependentResourcesAsync();
        }
    }
}
