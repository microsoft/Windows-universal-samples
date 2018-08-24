using Windows.Graphics.Holographic;
using Windows.Foundation;
using Windows.Perception.Spatial;
using SharpDX.Mathematics.Interop;
using SharpDX.Direct3D11;
using System.Numerics;
using Windows.Graphics.DirectX.Direct3D11;
using System.Runtime.InteropServices;
using System;

namespace BasicHologram.Common
{
    /// <summary>
    /// Constant buffer used to send hologram position transform to the shader pipeline.
    /// </summary>
    internal struct ViewProjectionConstantBuffer
    {
        public Matrix4x4 viewProjectionLeft;
        public Matrix4x4 viewProjectionRight;
    }

    // Manages DirectX device resources that are specific to a holographic camera, such as the
    // back buffer, ViewProjection constant buffer, and viewport.
    class CameraResources : Disposer
    {        
        // Direct3D rendering objects. Required for 3D.
        RenderTargetView                d3dRenderTargetView;
        DepthStencilView                d3dDepthStencilView;
        Texture2D                       d3dDepthBuffer;
        Texture2D                       d3dBackBuffer;

        // Device resource to store view and projection matrices.
        SharpDX.Direct3D11.Buffer       viewProjectionConstantBuffer;

        // Direct3D rendering properties.
        SharpDX.DXGI.Format             dxgiFormat;
        RawViewportF                    d3dViewport;
        Size                            d3dRenderTargetSize;
        
        // Indicates whether the camera supports stereoscopic rendering.
        bool                            isStereo = false;

        // Indicates whether this camera has a pending frame.
        bool                            framePending = false;
        
        // Pointer to the holographic camera these resources are for.
        HolographicCamera               holographicCamera = null;

        public CameraResources(HolographicCamera holographicCamera)
        {
            this.holographicCamera      = holographicCamera;
            isStereo                    = holographicCamera.IsStereo;
            d3dRenderTargetSize         = holographicCamera.RenderTargetSize;

            d3dViewport.Height          = (float)d3dRenderTargetSize.Height;
            d3dViewport.Width           = (float)d3dRenderTargetSize.Width;
            d3dViewport.X               = 0;
            d3dViewport.Y               = 0;
            d3dViewport.MinDepth        = 0;
            d3dViewport.MaxDepth        = 1;
        }

        /// <summary>
        /// Updates resources associated with a holographic camera's swap chain.
        /// The app does not access the swap chain directly, but it does create
        /// resource views for the back buffer.
        /// </summary>
        public void CreateResourcesForBackBuffer(
            DeviceResources deviceResources,
            ref HolographicCameraRenderingParameters cameraParameters
            )
        {
            var device = deviceResources.D3DDevice;
            
            // Get the WinRT object representing the holographic camera's back buffer.
            IDirect3DSurface surface = cameraParameters.Direct3D11BackBuffer;

            // Get a DXGI interface for the holographic camera's back buffer.
            // Holographic cameras do not provide the DXGI swap chain, which is owned
            // by the system. The Direct3D back buffer resource is provided using WinRT
            // interop APIs.
            InteropStatics.IDirect3DDxgiInterfaceAccess surfaceDxgiInterfaceAccess = surface as InteropStatics.IDirect3DDxgiInterfaceAccess;
            IntPtr pResource = surfaceDxgiInterfaceAccess.GetInterface(InteropStatics.ID3D11Resource);

            // Determine if the back buffer has changed. If so, ensure that the render target view
            // is for the current back buffer.
            if ((null == d3dBackBuffer) || (d3dBackBuffer.NativePointer != pResource))
            {
                // Clean up references to previous resources.
                this.RemoveAndDispose(ref d3dBackBuffer);
                this.RemoveAndDispose(ref d3dRenderTargetView);

                // This can change every frame as the system moves to the next buffer in the
                // swap chain. This mode of operation will occur when certain rendering modes
                // are activated.
                d3dBackBuffer = this.ToDispose(new SharpDX.Direct3D11.Texture2D(pResource));

                // Create a render target view of the back buffer.
                // Creating this resource is inexpensive, and is better than keeping track of
                // the back buffers in order to pre-allocate render target views for each one.
                d3dRenderTargetView = this.ToDispose(new RenderTargetView(device, d3dBackBuffer));

                // Get the DXGI format for the back buffer.
                // This information can be accessed by the app using CameraResources::GetBackBufferDXGIFormat().
                Texture2DDescription backBufferDesc = BackBufferTexture2D.Description;
                dxgiFormat = backBufferDesc.Format;

                // Check for render target size changes.
                Size currentSize = holographicCamera.RenderTargetSize;
                if (d3dRenderTargetSize != currentSize)
                {
                    // Set render target size.
                    d3dRenderTargetSize = HolographicCamera.RenderTargetSize;

                    // A new depth stencil view is also needed.
                    this.RemoveAndDispose(ref d3dDepthStencilView);
                }
            }

            // Refresh depth stencil resources, if needed.
            if (null == DepthStencilView)
            {
                // Create a depth stencil view for use with 3D rendering if needed.
                var depthStencilDesc = new Texture2DDescription
                {
                    Format    = SharpDX.DXGI.Format.R16_Typeless,
                    Width     = (int)RenderTargetSize.Width,
                    Height    = (int)RenderTargetSize.Height,
                    ArraySize = IsRenderingStereoscopic ? 2 : 1, // Create two textures when rendering in stereo.
                    MipLevels = 1, // Use a single mipmap level.
                    BindFlags = BindFlags.DepthStencil | BindFlags.ShaderResource,
                    SampleDescription = new SharpDX.DXGI.SampleDescription(1, 0)
                };

                d3dDepthBuffer = new Texture2D(device, depthStencilDesc);
                if (d3dDepthBuffer != null)
                {
                    var depthStencilViewDesc = new DepthStencilViewDescription();
                    depthStencilViewDesc.Dimension = IsRenderingStereoscopic ? DepthStencilViewDimension.Texture2DArray : DepthStencilViewDimension.Texture2D;
                    depthStencilViewDesc.Texture2DArray.ArraySize = IsRenderingStereoscopic ? 2 : 0;
                    depthStencilViewDesc.Format = SharpDX.DXGI.Format.D16_UNorm;
                    d3dDepthStencilView = this.ToDispose(new DepthStencilView(device, d3dDepthBuffer, depthStencilViewDesc));
                }
            }

            // Create the constant buffer, if needed.
            if (null == viewProjectionConstantBuffer)
            {
                // Create a constant buffer to store view and projection matrices for the camera.
                ViewProjectionConstantBuffer viewProjectionConstantBufferData = new ViewProjectionConstantBuffer();
                viewProjectionConstantBuffer = this.ToDispose(SharpDX.Direct3D11.Buffer.Create(
                    device,
                    BindFlags.ConstantBuffer,
                    ref viewProjectionConstantBufferData));
            }
        }

        /// <summary>
        /// Releases resources associated with a holographic display back buffer.
        /// </summary>
        public void ReleaseResourcesForBackBuffer(DeviceResources deviceResources)
        {
            var context = deviceResources.D3DDeviceContext;
            
            this.RemoveAndDispose(ref d3dBackBuffer);
            this.RemoveAndDispose(ref d3dDepthBuffer);
            this.RemoveAndDispose(ref d3dRenderTargetView);
            this.RemoveAndDispose(ref d3dDepthStencilView);

            // Ensure system references to the back buffer are released by clearing the render
            // target from the graphics pipeline state, and then flushing the Direct3D context.
            context.OutputMerger.ResetTargets();
            context.Flush();
        }

        public void ReleaseAllDeviceResources(DeviceResources deviceResources)
        {
            ReleaseResourcesForBackBuffer(deviceResources);
            this.RemoveAndDispose(ref viewProjectionConstantBuffer);
        }

        /// <summary>
        /// Updates the constant buffer for the display with view and projection 
        /// matrices for the current frame.
        /// </summary>
        public void UpdateViewProjectionBuffer(
            DeviceResources         deviceResources,
            HolographicCameraPose   cameraPose,
            SpatialCoordinateSystem coordinateSystem
            )
        {
            // The system changes the viewport on a per-frame basis for system optimizations.
            d3dViewport.X           = (float)cameraPose.Viewport.Left;
            d3dViewport.Y           = (float)cameraPose.Viewport.Top;
            d3dViewport.Width       = (float)cameraPose.Viewport.Width;
            d3dViewport.Height      = (float)cameraPose.Viewport.Height;
            d3dViewport.MinDepth    = 0;
            d3dViewport.MaxDepth    = 1;

            // The projection transform for each frame is provided by the HolographicCameraPose.
            HolographicStereoTransform cameraProjectionTransform = cameraPose.ProjectionTransform;

            // Get a container object with the view and projection matrices for the given
            // pose in the given coordinate system.
            HolographicStereoTransform? viewTransformContainer = cameraPose.TryGetViewTransform(coordinateSystem);
            
            // If TryGetViewTransform returns null, that means the pose and coordinate system
            // cannot be understood relative to one another; content cannot be rendered in this 
            // coordinate system for the duration of the current frame.
            // This usually means that positional tracking is not active for the current frame, in
            // which case it is possible to use a SpatialLocatorAttachedFrameOfReference to render
            // content that is not world-locked instead.
            ViewProjectionConstantBuffer viewProjectionConstantBufferData = new ViewProjectionConstantBuffer();
            bool viewTransformAcquired = viewTransformContainer.HasValue;
            if (viewTransformAcquired)
            {
                // Otherwise, the set of view transforms can be retrieved.
                HolographicStereoTransform viewCoordinateSystemTransform = viewTransformContainer.Value;

                // Update the view matrices. Holographic cameras (such as Microsoft HoloLens) are
                // constantly moving relative to the world. The view matrices need to be updated
                // every frame.
                viewProjectionConstantBufferData.viewProjectionLeft  = Matrix4x4.Transpose(
                    viewCoordinateSystemTransform.Left * cameraProjectionTransform.Left
                    );
                viewProjectionConstantBufferData.viewProjectionRight = Matrix4x4.Transpose(
                    viewCoordinateSystemTransform.Right * cameraProjectionTransform.Right
                    );
            }

            // Use the D3D device context to update Direct3D device-based resources.
            var context = deviceResources.D3DDeviceContext;

            // Loading is asynchronous. Resources must be created before they can be updated.
            if (context == null || viewProjectionConstantBuffer == null || !viewTransformAcquired)
            {
                framePending = false;
            }
            else
            {
                // Update the view and projection matrices.
                context.UpdateSubresource(ref viewProjectionConstantBufferData, viewProjectionConstantBuffer);

                framePending = true;
            }
        }

        /// <summary>
        /// Gets the view-projection constant buffer for the display, and attaches it 
        /// to the shader pipeline.
        /// </summary>        
        public bool AttachViewProjectionBuffer(DeviceResources deviceResources)
        {
            // This method uses Direct3D device-based resources.
            var context = deviceResources.D3DDeviceContext;

            // Loading is asynchronous. Resources must be created before they can be updated.
            // Cameras can also be added asynchronously, in which case they must be initialized
            // before they can be used.
            if (context == null || viewProjectionConstantBuffer == null || !framePending)
            {
                return false;
            }

            // Set the viewport for this camera.
            context.Rasterizer.SetViewport(Viewport);

            // Send the constant buffer to the vertex shader.
            context.VertexShader.SetConstantBuffers(1, viewProjectionConstantBuffer);

            // The template includes a pass-through geometry shader that is used by
            // default on systems that don't support the D3D11_FEATURE_D3D11_OPTIONS3::
            // VPAndRTArrayIndexFromAnyShaderFeedingRasterizer extension. The shader 
            // will be enabled at run-time on systems that require it.
            // If your app will also use the geometry shader for other tasks and those
            // tasks require the view/projection matrix, uncomment the following line 
            // of code to send the constant buffer to the geometry shader as well.
            //context.GeometryShader.SetConstantBuffers(1, viewProjectionConstantBuffer);
            
            framePending = false;

            return true;
        }

        // Direct3D device resources.
        public RenderTargetView BackBufferRenderTargetView
        {
            get { return d3dRenderTargetView;  }
        }
        public DepthStencilView DepthStencilView
        {
            get { return d3dDepthStencilView;  }
        }
        public Texture2D BackBufferTexture2D
        {
            get { return d3dBackBuffer;        }
        }
        public Texture2D DepthBufferTexture2D
        {
            get { return d3dDepthBuffer;       }
        }

        // Render target properties.
        public RawViewportF Viewport
        {
            get { return d3dViewport;          }
        }
        public SharpDX.DXGI.Format BackBufferDxgiFormat
        {
            get { return dxgiFormat; }
        }
        public Size RenderTargetSize
        {
            get { return d3dRenderTargetSize;  }
        }
        public bool IsRenderingStereoscopic
        {
            get { return isStereo;             }
        }

        // Associated objects.
        public HolographicCamera HolographicCamera
        {
            get { return holographicCamera;    }
        }
    }
}
