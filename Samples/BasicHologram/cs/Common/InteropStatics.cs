using System;
using System.Runtime.InteropServices;
using Windows.Graphics.DirectX.Direct3D11;

namespace BasicHologram.Common
{
    public static class InteropStatics
    {
        public static Guid IInspectable   = new Guid("AF86E2E0-B12D-4c6a-9C5A-D7AA65101E90");
        public static Guid ID3D11Resource = new Guid("dc8e63f3-d12b-4952-b47b-5e45026a862d");
        public static Guid IDXGIAdapter3  = new Guid("645967A4-1392-4310-A798-8053CE3E93FD");
        
        [DllImport(
            "d3d11.dll",
            EntryPoint          = "CreateDirect3D11DeviceFromDXGIDevice",
            SetLastError        = true,
            CharSet             = CharSet.Unicode,
            ExactSpelling       = true,
            CallingConvention   = CallingConvention.StdCall
            )]
        public static extern UInt32 CreateDirect3D11DeviceFromDXGIDevice(IntPtr dxgiDevice, out IntPtr graphicsDevice);

        [DllImport(
            "d3d11.dll",
            EntryPoint = "CreateDirect3D11SurfaceFromDXGISurface",
            SetLastError = true,
            CharSet = CharSet.Unicode,
            ExactSpelling = true,
            CallingConvention = CallingConvention.StdCall
            )]
        public static extern UInt32 CreateDirect3D11SurfaceFromDXGISurface(IntPtr dxgiSurface, out IntPtr direct3DSurface);

        [ComImport]
        [Guid("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1")]
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        [ComVisible(true)]
        public interface IDirect3DDxgiInterfaceAccess : IDisposable
        {
            IntPtr GetInterface([In] ref Guid iid);
        };

        public static IDirect3DSurface CreateDirect3DSurface(IntPtr dxgiSurface)
        {
            IntPtr inspectableSurface;
            uint hr = CreateDirect3D11SurfaceFromDXGISurface(dxgiSurface, out inspectableSurface);

            IDirect3DSurface depthD3DSurface = null;
            if (hr == 0)
            {
                depthD3DSurface = (IDirect3DSurface)Marshal.GetObjectForIUnknown(inspectableSurface);
                Marshal.Release(inspectableSurface);
            }

            return depthD3DSurface;
        }
    }    
}
