#pragma once

#include "pch.h"
#include "DirectXHelper.h"
#include <windows.ui.xaml.media.dxinterop.h>
#include <math.h>

namespace DX
{
    // Controls all the DirectX device resources.
    class DeviceResources
    {
    public:
        DeviceResources(Windows::Foundation::Size sizeContent, Windows::Foundation::Size sizeViewport);
        void BeginDraw(_In_opt_ const RECT* pUpdateRect = nullptr);
        void EndDraw();
        void CreateDeviceResources();
        void ClearTarget();
        void SetContentSize(Windows::Foundation::Size size);
        Windows::Foundation::Size GetContentSize() { return _contentSize; };
        void SetViewSize(Windows::Foundation::Size size);
        Windows::Foundation::Size GetViewSize() { return _viewSize; };
        void SetDPI(float dpi);
        float GetDPI() { return _logicalDPI; };
        void SetContentZoomFactor(float contentZoomFactor);
        float GetContentZoomFactor() { return _contentZoomFactor; };
        void Trim();

        void GetUpdateRects(_Outptr_ RECT** ppUpdateRects, _Out_ ULONG* pUpdateRectsCount);
        void InvalidateRect(RECT const& updateRect);
        void InvalidateContentRect();
        // Ensure update rect is within content rect's boundary
        RECT AdjustUpdateRect(RECT const& updateRect);

        // D3D Accessors.
        ID3D11Device*           GetD3DDevice() const                    { return _d3dDevice.Get(); }
        ID3D11DeviceContext2*   GetD3DDeviceContext() const             { return _d3dContext.Get(); }

        // D2D Accessors.
        ID2D1Device*            GetD2DDevice() const                    { return _d2dDevice.Get(); }
        ID2D1DeviceContext*     GetD2DDeviceContext() const             { return _d2dContext.Get(); }
        ID2D1Factory2*          GetD2DFactory() const                   { return _d2dFactory.Get(); }

        // VirtualSurfaceImageSource Accessors
        Windows::UI::Xaml::Media::Imaging::VirtualSurfaceImageSource^ GetImageSource()const { return _vsis; }
        Microsoft::WRL::ComPtr<IVirtualSurfaceImageSourceNative> GetImageSourceNative()const  { return _vsisNative; }

    private:
        // method
        void InvalidateAndResizeVSIS();
        float ConvertFromDIPToPixelUnit(float val, bool rounded = true) {
            float pixelVal = val * _dpiAdjustmentRatio;
            return rounded? floorf(pixelVal + 0.5f) : pixelVal;
        };
        float ConvertFromPixelToDIPUnit(float val, bool rounded = true) { 
            float dipVal = (_dpiAdjustmentRatio == 0) ? val : val / _dpiAdjustmentRatio;
            return rounded ? floorf(dipVal + 0.5f) : dipVal;
        };
        bool ShouldApplyContentZoomFactor(float contentZoomFactor) { 
            return (contentZoomFactor > 0) && (_contentZoomFactor != contentZoomFactor); 
        };

        void RefreshScaleFactor();

        // member variables
        const float DEFAULT_DPI = 96.f;

        // Direct3D device
        Microsoft::WRL::ComPtr<ID3D11Device>                _d3dDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext2>        _d3dContext;

        // Direct2D objects
        Microsoft::WRL::ComPtr<ID2D1Device>                 _d2dDevice;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext>          _d2dContext;
        Microsoft::WRL::ComPtr<ID2D1Factory2>               _d2dFactory;

        float                                               _logicalDPI;
        float                                               _dpiAdjustmentRatio;
        float                                               _contentZoomFactor;

        Microsoft::WRL::ComPtr<IVirtualSurfaceImageSourceNative>        _vsisNative;
        Windows::UI::Xaml::Media::Imaging::VirtualSurfaceImageSource^   _vsis;

        Windows::Foundation::Size _contentSize;
        Windows::Foundation::Size _viewSize;

        bool _beginDrawStarted;
    };
}