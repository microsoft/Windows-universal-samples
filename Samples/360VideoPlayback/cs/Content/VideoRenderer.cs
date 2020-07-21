using _360VideoPlayback.Common;
using System;
using System.Numerics;
using System.Runtime.InteropServices;
using Windows.Graphics.DirectX.Direct3D11;
using Windows.Media.Playback;

namespace _360VideoPlayback.Content
{
    /// <summary>
    /// This sample renderer instantiates a basic rendering pipeline.
    /// </summary>
    internal class VideoRenderer : Disposer
    {
        // Cached reference to device resources.
        private DeviceResources deviceResources;

        // Direct3D resources for cube geometry.
        private SharpDX.Direct3D11.InputLayout inputLayout;
        private SharpDX.Direct3D11.Buffer vertexBuffer;
        private SharpDX.Direct3D11.Buffer indexBuffer;
        private SharpDX.Direct3D11.VertexShader vertexShader;
        private SharpDX.Direct3D11.GeometryShader geometryShader;
        private SharpDX.Direct3D11.PixelShader pixelShader;
        private SharpDX.Direct3D11.Buffer modelConstantBuffer;

        private SharpDX.Direct3D11.Texture2D m_texture;
        private SharpDX.Direct3D11.ShaderResourceView m_textureView;
        private SharpDX.Direct3D11.SamplerState m_quadTextureSamplerState;
        private IDirect3DSurface d3dInteropSurface;

        // System resources for cube geometry.
        private ModelConstantBuffer modelConstantBufferData;
        private int indexCount = 0;

        // Variables used with the rendering loop.
        private bool loadingComplete = false;

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        private bool usingVprtShaders = false;

        private MediaPlayer m_mediaPlayer;
        private Object thisLock = new Object();

        /// <summary>
        /// Loads vertex and pixel shaders from files and instantiates the cube geometry.
        /// </summary>
        public VideoRenderer(DeviceResources deviceResources)
        {
            this.deviceResources = deviceResources;
            m_mediaPlayer = AppView.mediaPlayer;

        }

        /// <summary>
        /// Called once per frame, rotates the cube and calculates the model and view matrices.
        /// </summary>
        public void Update(StepTimer timer)
        {
            Vector3 scale = new Vector3(10.0f);
            Matrix4x4 modelscale = Matrix4x4.CreateScale(scale);

            // Here, we provide the model transform for the sample hologram. The model transform
            // matrix is transposed to prepare it for the shader.
            this.modelConstantBufferData.model = Matrix4x4.Transpose(modelscale);
            // Loading is asynchronous. Resources must be created before they can be updated.
            if (!loadingComplete)
            {
                return;
            }

            // Use the D3D device context to update Direct3D device-based resources.
            var context = this.deviceResources.D3DDeviceContext;

            // Update the model transform buffer for the hologram.
            context.UpdateSubresource(ref this.modelConstantBufferData, this.modelConstantBuffer);
        }

        /// <summary>
        /// Renders one frame using the vertex and pixel shaders.
        /// On devices that do not support the D3D11_FEATURE_D3D11_OPTIONS3::
        /// VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature,
        /// a pass-through geometry shader is also used to set the render 
        /// target array index.
        /// </summary>
        public void Render()
        {
            // Loading is asynchronous. Resources must be created before drawing can occur.
            if (!this.loadingComplete)
            {
                return;
            }

            var context = this.deviceResources.D3DDeviceContext;

            // Each vertex is one instance of the VertexPositionColor struct.
            int stride = SharpDX.Utilities.SizeOf<VertexPositionTexture>();
            int offset = 0;
            var bufferBinding = new SharpDX.Direct3D11.VertexBufferBinding(this.vertexBuffer, stride, offset);
            context.InputAssembler.SetVertexBuffers(0, bufferBinding);
            context.InputAssembler.SetIndexBuffer(
                this.indexBuffer,
                SharpDX.DXGI.Format.R32_UInt, // Each index is one 16-bit unsigned integer (short).
                0);
            context.InputAssembler.PrimitiveTopology = SharpDX.Direct3D.PrimitiveTopology.TriangleList;
            context.InputAssembler.InputLayout = this.inputLayout;

            // Attach the vertex shader.
            context.VertexShader.SetShader(this.vertexShader, null, 0);
            // Apply the model constant buffer to the vertex shader.
            context.VertexShader.SetConstantBuffers(0, this.modelConstantBuffer);

            if (!this.usingVprtShaders)
            {
                // On devices that do not support the D3D11_FEATURE_D3D11_OPTIONS3::
                // VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature,
                // a pass-through geometry shader is used to set the render target 
                // array index.
                context.GeometryShader.SetShader(this.geometryShader, null, 0);
            }

            // Attach the pixel shader.
            context.PixelShader.SetShader(this.pixelShader, null, 0);

            lock (thisLock)
            {

                context.PixelShader.SetShaderResources(0, 1, m_textureView);

                context.PixelShader.SetSamplers(0, 1, m_quadTextureSamplerState);

                // Draw the objects.
                context.DrawIndexedInstanced(
                    indexCount,     // Index count per instance.
                    2,              // Instance count.
                    0,              // Start index location.
                    0,              // Base vertex location.
                    0               // Start instance location.
                    );
            }
        }

        /// <summary>
        /// Creates device-based resources to store a constant buffer, cube
        /// geometry, and vertex and pixel shaders. In some cases this will also 
        /// store a geometry shader.
        /// </summary>
        public void CreateDeviceDependentResourcesAsync()
        {
            ReleaseDeviceDependentResources();

            // Create a constant buffer to store the model matrix.
            modelConstantBuffer = this.ToDispose(SharpDX.Direct3D11.Buffer.Create(
                deviceResources.D3DDevice,
                SharpDX.Direct3D11.BindFlags.ConstantBuffer,
                ref modelConstantBufferData));

            // Create the Texture, ShaderResource and Sampler state
            lock (thisLock)
            {
                m_texture = this.ToDispose(new SharpDX.Direct3D11.Texture2D(deviceResources.D3DDevice, new SharpDX.Direct3D11.Texture2DDescription()
                {
                    Format = SharpDX.DXGI.Format.R8G8B8A8_UNorm,
                    Width = (int)m_mediaPlayer.PlaybackSession.NaturalVideoWidth,//Width
                    Height = (int)m_mediaPlayer.PlaybackSession.NaturalVideoHeight,//Height
                    MipLevels = 1,
                    ArraySize = 1,
                    BindFlags = SharpDX.Direct3D11.BindFlags.ShaderResource | SharpDX.Direct3D11.BindFlags.RenderTarget,
                    Usage = SharpDX.Direct3D11.ResourceUsage.Default,
                    CpuAccessFlags = SharpDX.Direct3D11.CpuAccessFlags.None,
                    SampleDescription = new SharpDX.DXGI.SampleDescription(1, 0),
                    OptionFlags = SharpDX.Direct3D11.ResourceOptionFlags.None
                }));

                m_textureView = this.ToDispose(new SharpDX.Direct3D11.ShaderResourceView(deviceResources.D3DDevice, m_texture));
            }

            m_quadTextureSamplerState = this.ToDispose(new SharpDX.Direct3D11.SamplerState(deviceResources.D3DDevice, new SharpDX.Direct3D11.SamplerStateDescription()
            {
                Filter = SharpDX.Direct3D11.Filter.Anisotropic,
                AddressU = SharpDX.Direct3D11.TextureAddressMode.Clamp,
                AddressV = SharpDX.Direct3D11.TextureAddressMode.Clamp,
                AddressW = SharpDX.Direct3D11.TextureAddressMode.Clamp,
                MaximumAnisotropy = 3,
                MinimumLod = 0,
                MaximumLod = 3,
                MipLodBias = 0,
                BorderColor = new SharpDX.Mathematics.Interop.RawColor4(0, 0, 0, 0),
                ComparisonFunction = SharpDX.Direct3D11.Comparison.Never
            }));

            CreateD3D11Surface();
            LoadShaders();
        }


        private async void LoadShaders()
        {
            usingVprtShaders = deviceResources.D3DDeviceSupportsVprt;
            // On devices that do support the D3D11_FEATURE_D3D11_OPTIONS3::
            // VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature
            // we can avoid using a pass-through geometry shader to set the render
            // target array index, thus avoiding any overhead that would be 
            // incurred by setting the geometry shader stage.
            var vertexShaderFileName = usingVprtShaders ? "Content\\Shaders\\VPRTVertexShader.cso" : "Content\\Shaders\\VertexShader.cso";
            var folder = Windows.ApplicationModel.Package.Current.InstalledLocation;
            // Load the compiled vertex shader.
            var vertexShaderByteCode = await DirectXHelper.ReadDataAsync(await folder.GetFileAsync(vertexShaderFileName));

            // After the vertex shader file is loaded, create the shader and input layout.
            vertexShader = this.ToDispose(new SharpDX.Direct3D11.VertexShader(
                deviceResources.D3DDevice,
                vertexShaderByteCode));

            SharpDX.Direct3D11.InputElement[] vertexDesc =
            {
                new SharpDX.Direct3D11.InputElement("POSITION", 0, SharpDX.DXGI.Format.R32G32B32_Float,  0, 0, SharpDX.Direct3D11.InputClassification.PerVertexData, 0),
                new SharpDX.Direct3D11.InputElement("TEXCOORD", 1, SharpDX.DXGI.Format.R32G32_Float, 12, 0, SharpDX.Direct3D11.InputClassification.PerVertexData, 0),
            };


            inputLayout = this.ToDispose(new SharpDX.Direct3D11.InputLayout(
                deviceResources.D3DDevice,
                vertexShaderByteCode,
                vertexDesc));


            if (!usingVprtShaders)
            {
                // Load the compiled pass-through geometry shader.
                var geometryShaderByteCode = await DirectXHelper.ReadDataAsync(await folder.GetFileAsync("Content\\Shaders\\GeometryShader.cso"));

                // After the pass-through geometry shader file is loaded, create the shader.
                geometryShader = this.ToDispose(new SharpDX.Direct3D11.GeometryShader(
                    deviceResources.D3DDevice,
                    geometryShaderByteCode));
            }

            // Load the compiled pixel shader.
            var pixelShaderByteCode = await DirectXHelper.ReadDataAsync(await folder.GetFileAsync("Content\\Shaders\\PixelShader.cso"));

            // After the pixel shader file is loaded, create the shader.
            pixelShader = this.ToDispose(new SharpDX.Direct3D11.PixelShader(
                deviceResources.D3DDevice,
                pixelShaderByteCode));

            ComputeSphere(16, true);
            // Once the cube is loaded, the object is ready to be rendered.
            loadingComplete = true;
        }

        /// <summary>
        /// Releases device-based resources.
        /// </summary>
        public void ReleaseDeviceDependentResources()
        {
            loadingComplete = false;
            usingVprtShaders = false;
            this.RemoveAndDispose(ref vertexShader);
            this.RemoveAndDispose(ref inputLayout);
            this.RemoveAndDispose(ref pixelShader);
            this.RemoveAndDispose(ref geometryShader);
            this.RemoveAndDispose(ref modelConstantBuffer);
            this.RemoveAndDispose(ref vertexBuffer);
            this.RemoveAndDispose(ref indexBuffer);
            this.RemoveAndDispose(ref m_texture);
            this.RemoveAndDispose(ref m_textureView);
            this.RemoveAndDispose(ref m_quadTextureSamplerState);
        }

        private void CreateD3D11Surface()
        {
            SharpDX.DXGI.Surface2 surface = m_texture.QueryInterface<SharpDX.DXGI.Surface2>();
            IntPtr pUnknown;
            UInt32 hr = InteropStatics.CreateDirect3D11SurfaceFromDXGISurface(surface.NativePointer, out pUnknown);

            if (hr == 0)
            {
                d3dInteropSurface = (IDirect3DSurface)Marshal.GetObjectForIUnknown(pUnknown);
                Marshal.Release(pUnknown);
            }

            m_mediaPlayer.VideoFrameAvailable += M_mediaPlayer_VideoFrameAvailable;
            m_mediaPlayer.Play();
        }

        private void M_mediaPlayer_VideoFrameAvailable(MediaPlayer sender, object args)
        {
            lock (thisLock)
            {
                m_mediaPlayer.CopyFrameToVideoSurface(d3dInteropSurface);
            }
        }

        private void ComputeSphere(int tessellation, bool invertn)
        {
            if (tessellation < 3)
            {
                new Exception("tesselation parameter out of range");
            }

            int verticalSegments = tessellation;
            int horizontalSegments = tessellation * 2;
            int vertCount = (verticalSegments + 1) * (horizontalSegments + 1);
            int idxCount = (verticalSegments) * (horizontalSegments + 1) * 6;

            float radius = 0.5f; // Diameter of the default Sphere will always be 1 to stay aligned


            VertexPositionTexture[] sphereVertArray = new VertexPositionTexture[vertCount];
            int[] cubeIndices = new int[idxCount];

            int writePos = 0;

            for (int i = 0; i <= verticalSegments; i++)
            {
                float v = 1 - (float)i / verticalSegments;

                float latitude = (float)(i * SharpDX.MathUtil.Pi /
                                           verticalSegments) - SharpDX.MathUtil.PiOverTwo;

                var dy = (float)Math.Sin(latitude);
                var dxz = (float)Math.Cos(latitude);

                // Create a single ring of vertices at this latitude.
                for (ushort j = 0; j <= horizontalSegments; j++)
                {
                    float u = (float)j / horizontalSegments;

                    float longitude = (float)(j * SharpDX.MathUtil.TwoPi / horizontalSegments);
                    float dx = (float)Math.Cos(longitude);
                    float dz = (float)Math.Sin(longitude);
                    dx *= dxz;
                    dz *= dxz;

                    Vector3 vect3 = new Vector3(dx, dy, dz);
                    VertexPositionTexture vert;
                    vert.pos = vect3 * radius;
                    vert.texture = new Vector2(u, v);

                    sphereVertArray[writePos] = vert;
                    writePos++;
                }
            }

            // Fill the index buffer with triangles joining each pair of latitude rings.
            int stride = horizontalSegments + 1;
            writePos = 0;
            for (int i = 0; i < verticalSegments; i++)
            {
                for (int j = 0; j <= horizontalSegments; j++)
                {
                    int nextI = i + 1;
                    int nextJ = (j + 1) % stride;

                    cubeIndices[writePos] = i * stride + j;
                    cubeIndices[++writePos] = nextI * stride + j;
                    cubeIndices[++writePos] = i * stride + nextJ;

                    cubeIndices[++writePos] = i * stride + nextJ;
                    cubeIndices[++writePos] = nextI * stride + j;
                    cubeIndices[++writePos] = nextI * stride + nextJ;
                    writePos++;
                }
            }

            vertexBuffer = this.ToDispose(SharpDX.Direct3D11.Buffer.Create(
               deviceResources.D3DDevice,
               SharpDX.Direct3D11.BindFlags.VertexBuffer,
               sphereVertArray));

            indexCount = idxCount;

            indexBuffer = this.ToDispose(SharpDX.Direct3D11.Buffer.Create(
               deviceResources.D3DDevice,
               SharpDX.Direct3D11.BindFlags.IndexBuffer,
               cubeIndices));
        }

        private void XMScalarSinCos(ref float Sin, ref float Cos, float Value)
        {
            const float XM1div2PI = 0.159154943f;
            // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
            float quotient = XM1div2PI * Value;
            if (Value >= 0.0f)
            {
                quotient = (float)((int)(quotient + 0.5f));
            }
            else
            {
                quotient = (float)((int)(quotient - 0.5f));
            }
            float y = Value - SharpDX.MathUtil.TwoPi * quotient;

            // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
            float sign;
            if (y > SharpDX.MathUtil.PiOverTwo)
            {
                y = SharpDX.MathUtil.Pi - y;
                sign = -1.0f;
            }
            else if (y < -(SharpDX.MathUtil.PiOverTwo))
            {
                y = -(SharpDX.MathUtil.Pi) - y;
                sign = -1.0f;
            }
            else
            {
                sign = +1.0f;
            }

            float y2 = y * y;

            // 11-degree minimax approximation
            Sin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

            // 10-degree minimax approximation
            float p = (float)((float)((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f);

            Cos = sign * p;
        }

    }
}


