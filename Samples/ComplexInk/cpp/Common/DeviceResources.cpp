#include "pch.h"
#include "DeviceResources.h"

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::UI::Xaml::Media::Imaging;

DX::DeviceResources::DeviceResources(Windows::Foundation::Size size, Windows::Foundation::Size sizeViewport) :
    _beginDrawStarted(false),
    _contentZoomFactor(1.0f)
{
    SetContentSize(size);
    SetViewSize(sizeViewport);
    _logicalDPI = Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi;
    _dpiAdjustmentRatio = _logicalDPI / DEFAULT_DPI;
    CreateDeviceResources();
}

// Initialize hardware-dependent resources.
void DX::DeviceResources::CreateDeviceResources()
{
    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    ComPtr<ID3D11DeviceContext> context;
    D3D_FEATURE_LEVEL d3dFeatureLevel;

    // Initialize Direct2D resources.
    D2D1_FACTORY_OPTIONS options;
    ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

    // Initialize the Direct2D Factory.
    DX::ThrowIfFailed(
        D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory2),
        &options,
        &_d2dFactory
        )
        );


    // Create the Direct3D 11 API device object.
    DX::ThrowIfFailed(
        D3D11CreateDevice(
        nullptr,                        // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,                  // Set debug and Direct2D compatibility flags.
        nullptr,                        // List of feature levels this app can support.
        0,
        D3D11_SDK_VERSION,              // Always set this to D3D11_SDK_VERSION for Windows Runtime apps.
        &_d3dDevice,                   // Returns the Direct3D device created.
        &d3dFeatureLevel,
        &context
        )
        );

    // Get the Direct3D 11.1 API device.
    ComPtr<IDXGIDevice> dxgiDevice;
    DX::ThrowIfFailed(
        _d3dDevice.As(&dxgiDevice)
        );

    DX::ThrowIfFailed(
        context.As(&_d3dContext)
        );

    // Create the Direct2D device object and a corresponding context.
    DX::ThrowIfFailed(
        _d2dFactory->CreateDevice(
        dxgiDevice.Get(),
        &_d2dDevice
        )
        );

    DX::ThrowIfFailed(
        _d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &_d2dContext
        )
        );

    // By default, D2D will treat incoming values as DIPs instead of pixels. By setting the
    // logical DPI the application operates on, D2D will use it as a transform factor to scale
    // the content. Since the XAML virtual surface image source uses pixel unit instead of DIP, 
    // such scale will in fact ensure the rendered graphics locate at the right physical pixels 
    // of the VSIS.
    float logicalDPI = _logicalDPI * GetContentZoomFactor();
    _d2dContext->SetDpi(logicalDPI, logicalDPI);

    // When an application performs animation or image composition of graphics content, it is important
    // to use Direct2D grayscale text rendering mode rather than ClearType. The ClearType technique
    // operates on the color channels and not the alpha channel, and therefore unable to correctly perform
    // image composition or sub-pixel animation of text. ClearType is still a method of choice when it
    // comes to direct rendering of text to the destination surface with no subsequent composition required.
    _d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    // Set content zoom as a scale transform on D2D's device context. That will ensure ink will be 
    // rendered will appropriate quality when being zoomed.
    _dpiAdjustmentRatio *= GetContentZoomFactor();

    // Create an image source at the given content size with DPI adjustment. This ensures that we get VSIS image size in physical pixels instead of DIPs.
    int pixelWidth = static_cast<int>(ConvertFromDIPToPixelUnit(_contentSize.Width));
    int pixelHeight = static_cast<int>(ConvertFromDIPToPixelUnit(_contentSize.Height));
    _vsis = ref new VirtualSurfaceImageSource(pixelWidth, pixelHeight);

    ComPtr<IUnknown> unknown(reinterpret_cast<IUnknown*>(_vsis));
    unknown.As(&_vsisNative);

    DX::ThrowIfFailed(
        _vsisNative->SetDevice(dxgiDevice.Get())
        );
}

// Update logical DPI and invalidate VSIS if needed.
void DX::DeviceResources::SetDPI(float dpi)
{
    if (dpi != _logicalDPI)
    {
        _logicalDPI = dpi;
        RefreshScaleFactor();
    }
}

// Update content zoom factor and invalidate VSIS if needed.
void DX::DeviceResources::SetContentZoomFactor(float contentZoomFactor)
{
    if (ShouldApplyContentZoomFactor(contentZoomFactor))
    {
        _contentZoomFactor = contentZoomFactor;
        RefreshScaleFactor();
    }
}

// Call this method when the app suspends. It provides a hint to the driver that the app  
// is entering an idle state and that temporary buffers can be reclaimed for use by other apps. 
void DX::DeviceResources::Trim()
{
    ComPtr<IDXGIDevice3> dxgiDevice;
    _d3dDevice.As(&dxgiDevice);
    dxgiDevice->Trim();
}

void DX::DeviceResources::RefreshScaleFactor()
{
    float logicalDPI = GetDPI() * GetContentZoomFactor();
    _dpiAdjustmentRatio = logicalDPI / DEFAULT_DPI;

    if (_d2dContext != nullptr)
    {
        _d2dContext->SetDpi(logicalDPI, logicalDPI);
        InvalidateAndResizeVSIS();
    }
}

void DX::DeviceResources::GetUpdateRects(_Outptr_ RECT** ppUpdateRects, _Out_ ULONG* pUpdateRectsCount)
{
    assert(_vsisNative != nullptr);
    *pUpdateRectsCount = 0;
    
    DX::ThrowIfFailed(
        _vsisNative->GetUpdateRectCount(pUpdateRectsCount)
        );

    *ppUpdateRects = new RECT[*pUpdateRectsCount];

    if (*ppUpdateRects != nullptr)
    {
        DX::ThrowIfFailed(
            _vsisNative->GetUpdateRects(*ppUpdateRects, *pUpdateRectsCount)
            );
    }
    else
    {
        DX::ThrowIfFailed(E_OUTOFMEMORY);
    }
}

void DX::DeviceResources::InvalidateRect(RECT const& updateRect)
{
    assert(_vsisNative != nullptr);
    DX::ThrowIfFailed(
        _vsisNative->Invalidate(updateRect)
        );
}

void DX::DeviceResources::InvalidateContentRect()
{
    long pixelContentWidth = static_cast<long>(ConvertFromDIPToPixelUnit(_contentSize.Width));
    long pixelContentHeight = static_cast<long>(ConvertFromDIPToPixelUnit(_contentSize.Height));
    RECT bounds = {0, 0, pixelContentWidth, pixelContentHeight};
    InvalidateRect(bounds);
}

// If an update rect is passed in through the input parameter, assume it is 
// in DIP units and we need to internally convert it to physical pixel units.
void  DX::DeviceResources::BeginDraw(_In_opt_ const RECT* pUpdateRect)
{
    assert(_vsisNative != nullptr);
    long pixelViewWidth = static_cast<long>(ConvertFromDIPToPixelUnit(_viewSize.Width));
    long pixelViewHeight = static_cast<long>(ConvertFromDIPToPixelUnit(_viewSize.Height));
    RECT updateRect = { 0, 0, pixelViewWidth, pixelViewHeight };
    RECT emptyRect = { 0, 0, 0, 0 };
    
    if (pUpdateRect == nullptr)
    {
        RECT visibleRect = {};

        DX::ThrowIfFailed(
            _vsisNative->GetVisibleBounds(&visibleRect)
            );

        if (visibleRect != emptyRect) { updateRect = visibleRect; }
    }
    else
    {
        if (*pUpdateRect != emptyRect)
        {
            updateRect = *pUpdateRect;
        }
    }

    POINT offset;
    ComPtr<IDXGISurface> surface;

    // Returns a target surface and an offset to use as the top left origin when drawing.
    DX::ThrowIfFailed(
        _vsisNative->BeginDraw(updateRect, &surface, &offset)
    );

    // Create render target.
    ComPtr<ID2D1Bitmap1> bitmap;
    DX::ThrowIfFailed(
        _d2dContext->CreateBitmapFromDxgiSurface(
        surface.Get(),
        nullptr,
        &bitmap
        )
        );

    _d2dContext->BeginDraw();

    // Set the context transformation based on the offset (in physical pixel units) returned by VSIS's BeginDraw.
    // Since D2D operates on DIP units, we need to do the inverse DPI adjustment to make the coordinates match.
    // Note that we don't round the return value because D2D will scale everything back up by the DPI scale factor
    // and the result should land on the correct pixel as we operate in physical pixels.
    _d2dContext->SetTransform(
        D2D1::Matrix3x2F::Translation(
        ConvertFromPixelToDIPUnit(static_cast<float>(offset.x - updateRect.left), false/*rounded*/),
        ConvertFromPixelToDIPUnit(static_cast<float>(offset.y - updateRect.top), false/*rounded*/)
        )
        );

    _d2dContext->SetTarget(bitmap.Get());

    _beginDrawStarted = true;
}

void DX::DeviceResources::EndDraw()
{
    if (_beginDrawStarted)
    {
        _beginDrawStarted = false;

        _d2dContext->SetTarget(nullptr);

        // End the drawing
        DX::ThrowIfFailed(
            _d2dContext->EndDraw()
            );

        DX::ThrowIfFailed(
            _vsisNative->EndDraw()
            );
    }
}

void DX::DeviceResources::ClearTarget()
{
    // Set the background on which objects will be rendered on top
    Windows::UI::Color color = CanvasBackgroundColor;

    _d2dContext->Clear(D2D1::ColorF(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f));

}

void DX::DeviceResources::SetContentSize(Windows::Foundation::Size size)
{
    _contentSize = size;
    InvalidateAndResizeVSIS();
}

void DX::DeviceResources::InvalidateAndResizeVSIS()
{
    if (_vsisNative != nullptr)
    {
        int pixelWidth = static_cast<int>(ConvertFromDIPToPixelUnit(_contentSize.Width));
        int pixelHeight = static_cast<int>(ConvertFromDIPToPixelUnit(_contentSize.Height));

        DX::ThrowIfFailed(
            _vsisNative->Resize(pixelWidth, pixelHeight)
            );
        RECT rect = { 0, 0, pixelWidth, pixelHeight };
        DX::ThrowIfFailed(
            _vsisNative->Invalidate(rect)
            );
    }
}

void DX::DeviceResources::SetViewSize(Windows::Foundation::Size size)
{
    _viewSize = size;
}

RECT DX::DeviceResources::AdjustUpdateRect(RECT const& updateRect)
{
    RECT newUpdateRect = { 0, 0, 0, 0 };
    // Ensure update rect is within content rect's boundary and do DPI adjustment as needed.
    // We also inflate the rect further by rounding down the top/left position and rounding up
    // the bottom/right position.
    newUpdateRect.left = static_cast<long>(ConvertFromDIPToPixelUnit(updateRect.left < 0 ? 0 : static_cast<float>(updateRect.left), false/*rounded*/));
    newUpdateRect.top = static_cast<long>(ConvertFromDIPToPixelUnit(updateRect.top < 0 ? 0 : static_cast<float>(updateRect.top), false/*rounded*/));
    newUpdateRect.right = static_cast<long>(ConvertFromDIPToPixelUnit(updateRect.right > _contentSize.Width ? _contentSize.Width : static_cast<float>(updateRect.right)));
    newUpdateRect.bottom = static_cast<long>(ConvertFromDIPToPixelUnit(updateRect.bottom > _contentSize.Height ? _contentSize.Height : static_cast<float>(updateRect.bottom)));

    return newUpdateRect;
}