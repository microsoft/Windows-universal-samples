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
#include "DirectXPage.xaml.h"
#include "DirectXHelper.h"

using namespace D2DAdvancedColorImages;

using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::System;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

DirectXPage::DirectXPage() :
    m_isWindowVisible(true),
    m_imageInfo{},
    m_isImageValid(false),
    m_imageMaxCLL(-1.0f)
{
    InitializeComponent();

    // Register event handlers for page lifecycle.
    CoreWindow^ window = Window::Current->CoreWindow;

    window->KeyUp += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &DirectXPage::OnKeyUp);

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &DirectXPage::OnVisibilityChanged);

    window->ResizeCompleted +=
        ref new TypedEventHandler<CoreWindow ^, Object ^>(this, &DirectXPage::OnResizeCompleted);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnOrientationChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDisplayContentsInvalidated);

    currentDisplayInformation->AdvancedColorInfoChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnAdvancedColorInfoChanged);

    swapChainPanel->CompositionScaleChanged +=
        ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &DirectXPage::OnCompositionScaleChanged);

    swapChainPanel->SizeChanged +=
        ref new SizeChangedEventHandler(this, &DirectXPage::OnSwapChainPanelSizeChanged);

    // Pointer and manipulation events handle image pan and zoom.
    swapChainPanel->PointerPressed += ref new PointerEventHandler(this, &DirectXPage::OnPointerPressed);
    swapChainPanel->PointerMoved += ref new PointerEventHandler(this, &DirectXPage::OnPointerMoved);
    swapChainPanel->PointerReleased += ref new PointerEventHandler(this, &DirectXPage::OnPointerReleased);
    swapChainPanel->PointerCanceled += ref new PointerEventHandler(this, &DirectXPage::OnPointerCanceled);
    swapChainPanel->PointerWheelChanged += ref new PointerEventHandler(this, &DirectXPage::OnPointerWheelChanged);

    m_gestureRecognizer = ref new GestureRecognizer();
    m_gestureRecognizer->GestureSettings =
        GestureSettings::ManipulationTranslateX |
        GestureSettings::ManipulationTranslateY |
        GestureSettings::ManipulationScale;

    m_gestureRecognizer->ManipulationStarted +=
        ref new TypedEventHandler<GestureRecognizer^, ManipulationStartedEventArgs^>(this, &DirectXPage::OnManipulationStarted);

    m_gestureRecognizer->ManipulationUpdated +=
        ref new TypedEventHandler<GestureRecognizer^, ManipulationUpdatedEventArgs^>(this, &DirectXPage::OnManipulationUpdated);

    m_gestureRecognizer->ManipulationCompleted +=
        ref new TypedEventHandler<GestureRecognizer^, ManipulationCompletedEventArgs^>(this, &DirectXPage::OnManipulationCompleted);

    m_renderOptionsViewModel = ref new RenderOptionsViewModel();

    // At this point we have access to the device and
    // can create the device-dependent resources.
    m_deviceResources = std::make_shared<DX::DeviceResources>();
    m_deviceResources->SetSwapChainPanel(swapChainPanel);

    m_renderer = std::unique_ptr<D2DAdvancedColorImagesRenderer>(new D2DAdvancedColorImagesRenderer(m_deviceResources));

    UpdateDisplayACState(currentDisplayInformation->GetAdvancedColorInfo());
}

DirectXPage::~DirectXPage()
{
}

void DirectXPage::LoadDefaultImage()
{
    Uri^ uri = ref new Uri("https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/image-scrgb-icc.jxr");
    create_task(StorageFile::CreateStreamedFileFromUriAsync(L"image-scRGB-ICC.jxr", uri, nullptr)).then([=](StorageFile^ imageFile)
    {
        LoadImage(imageFile);
    });
}

void DirectXPage::LoadImage(_In_ StorageFile^ imageFile)
{
    create_task(imageFile->OpenAsync(FileAccessMode::Read)
        ).then([=](IRandomAccessStream^ ras) {
            // If file opening fails, fall through to error handler at the end of task chain.

            ComPtr<IStream> iStream;
            DX::ThrowIfFailed(CreateStreamOverRandomAccessStream(ras, IID_PPV_ARGS(&iStream)));
            return m_renderer->LoadImageFromWic(iStream.Get());
    }).then([=](ImageInfo info) {
        m_imageInfo = info;

        m_renderer->CreateImageDependentResources();
        m_imageMaxCLL = m_renderer->FitImageToWindow();

        ApplicationView::GetForCurrentView()->Title = imageFile->Name;
        ImageACKind->Text = L"Kind: " + ConvertACKindToString(m_imageInfo.imageKind);
        ImageHasColorProfile->Text = L"Color profile: " + (m_imageInfo.numProfiles > 0 ? L"Yes" : L"No");
        ImageBitDepth->Text = L"Bit depth: " + ref new String(std::to_wstring(m_imageInfo.bitsPerChannel).c_str());
        ImageIsFloat->Text = L"Floating point: " + (m_imageInfo.isFloat ? L"Yes" : L"No");

        std::wstringstream cllStr;
        cllStr << L"Estimated MaxCLL: ";
        if (m_imageMaxCLL < 0.0f)
        {
            cllStr << L"N/A";
        }
        else
        {
            cllStr << std::to_wstring(static_cast<int>(m_imageMaxCLL)) << L" nits";
        }

        ImageMaxCLL->Text = ref new String(cllStr.str().c_str());

        // Image loading is done at this point.
        m_isImageValid = true;
        UpdateDefaultRenderOptions();

        // Ensure the preceding continuation runs on the UI thread.
    }, task_continuation_context::use_current()).then([=](task<void> previousTask) {
        try
        {
            previousTask.get();
        }
        catch (...)
        {
            // Errors resulting from failure to load/decode image are ignored.
            return;
        }
    });
}

void DirectXPage::UpdateDisplayACState(_In_ AdvancedColorInfo^ info)
{
    // Render options are meaningless when this method is first called, as no image has been loaded yet.
    // Therefore it doesn't matter what value we use for oldDispKind in this case.
    auto oldDispKind = m_dispInfo ? m_dispInfo->CurrentAdvancedColorKind : AdvancedColorKind::StandardDynamicRange;
    auto newDispKind = info->CurrentAdvancedColorKind;
    m_dispInfo = info;

    DisplayACState->Text = L"Kind: " + ConvertACKindToString(newDispKind);

    unsigned int maxcll = static_cast<unsigned int>(info->MaxLuminanceInNits);

    if (maxcll == 0)
    {
        // Luminance value of 0 means that no valid data was provided by the display.
        DisplayPeakLuminance->Text = L"Peak luminance: Unknown";
    }
    else
    {
        DisplayPeakLuminance->Text = L"Peak luminance: " + ref new String(std::to_wstring(maxcll).c_str()) + L" nits";
    }

    if (oldDispKind == newDispKind)
    {
        // Some changes, such as peak luminance or SDR white level, don't need to reset rendering options.
        UpdateRenderOptions();
    }
    else
    {
        // If display has changed kind between SDR/HDR/WCG, we must reset all rendering options.
        UpdateDefaultRenderOptions();
    }

}

// UI element event handlers.

void DirectXPage::LoadImageButtonClick(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->SuggestedStartLocation = PickerLocationId::Desktop;
    picker->FileTypeFilter->Append(L".jxr");
    picker->FileTypeFilter->Append(L".jpg");
    picker->FileTypeFilter->Append(L".png");
    picker->FileTypeFilter->Append(L".tif");

    create_task(picker->PickSingleFileAsync()).then([=](StorageFile^ pickedFile) {
        if (pickedFile != nullptr)
        {
            LoadImage(pickedFile);
        }
    });
}

void DirectXPage::ResetButtonClick(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    LoadDefaultImage();
}

// Saves the current state of the app for suspend and terminate events.
void DirectXPage::SaveInternalState(_In_ IPropertySet^ state)
{
    m_deviceResources->Trim();
}

// Loads the current state of the app for resume events.
void DirectXPage::LoadInternalState(_In_ IPropertySet^ state)
{
}

// Based on image and display parameters, choose the best rendering options.
void DirectXPage::UpdateDefaultRenderOptions()
{
    if (!m_isImageValid)
    {
        // Render options are only meaningful if an image is already loaded.
        return;
    }

    switch (m_imageInfo.imageKind)
    {
    case AdvancedColorKind::StandardDynamicRange:
    case AdvancedColorKind::WideColorGamut:
    default:
        // SDR and WCG images don't need to be tonemapped.
        RenderEffectCombo->SelectedIndex = 2; // See RenderOptions.h for which value this indicates.

        // Manual brightness adjustment is only useful for HDR content.
        // SDR and WCG content is adjusted by the OS-provided AdvancedColorInfo::SdrWhiteLevel parameter.
        BrightnessAdjustSlider->Value = 1.0f;
        BrightnessAdjustPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        break;

    case AdvancedColorKind::HighDynamicRange:

        switch (m_dispInfo->CurrentAdvancedColorKind)
        {
        case AdvancedColorKind::StandardDynamicRange:
        case AdvancedColorKind::WideColorGamut:
        default:
            // HDR content + non-HDR display: HDR tonemapping is needed for correct rendering.
            RenderEffectCombo->SelectedIndex = 0; // See RenderOptions.h for which value this indicates.
            break;

        case AdvancedColorKind::HighDynamicRange:
            // HDR content + HDR display: HDR tonemapping is needed for best results, but this sample's
            // tonemappers are very simple and not suitable, so just disable it.
            RenderEffectCombo->SelectedIndex = 2; // See RenderOptions.h for which value this indicates.
            break;
        }

        // Manual brightness adjustment is useful for any HDR content.
        BrightnessAdjustPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
        break;
    }

    UpdateRenderOptions();
}

// Common method for updating options on the renderer.
void DirectXPage::UpdateRenderOptions()
{
    if ((m_renderer != nullptr) && RenderEffectCombo->SelectedItem)
    {
        auto tm = static_cast<EffectOption^>(RenderEffectCombo->SelectedItem);

        m_renderer->SetRenderOptions(
            tm->Kind,
            static_cast<float>(BrightnessAdjustSlider->Value),
            m_dispInfo
            );
    }
}

// Window event handlers.

void DirectXPage::OnVisibilityChanged(_In_ CoreWindow^ sender, _In_ VisibilityChangedEventArgs^ args)
{
    m_isWindowVisible = args->Visible;
    if (m_isWindowVisible)
    {
        m_renderer->Draw();
    }
}

// DisplayInformation event handlers.

void DirectXPage::OnDpiChanged(_In_ DisplayInformation^ sender, _In_ Object^ args)
{
    m_deviceResources->SetDpi(sender->LogicalDpi);
    m_renderer->CreateWindowSizeDependentResources();
    m_renderer->Draw();
}

void DirectXPage::OnOrientationChanged(_In_ DisplayInformation^ sender, _In_ Object^ args)
{
    m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
    m_renderer->CreateWindowSizeDependentResources();
    m_renderer->Draw();
}

void DirectXPage::OnDisplayContentsInvalidated(_In_ DisplayInformation^ sender, _In_ Object^ args)
{
    m_deviceResources->ValidateDevice();
    m_renderer->CreateWindowSizeDependentResources();
    m_renderer->Draw();
}

void DirectXPage::OnAdvancedColorInfoChanged(_In_ DisplayInformation ^sender, _In_ Object ^args)
{
    UpdateDisplayACState(sender->GetAdvancedColorInfo());
}


// Other event handlers.

void DirectXPage::OnCompositionScaleChanged(_In_ SwapChainPanel^ sender, _In_ Object^ args)
{
    m_deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
    m_renderer->CreateWindowSizeDependentResources();
    m_renderer->Draw();
}

void DirectXPage::OnSwapChainPanelSizeChanged(_In_ Object^ sender, _In_ SizeChangedEventArgs^ e)
{
    m_deviceResources->SetLogicalSize(e->NewSize);
    m_renderer->CreateWindowSizeDependentResources();
    m_renderer->Draw();
}

String^ DirectXPage::ConvertACKindToString(AdvancedColorKind kind)
{
    String^ displayString;
    switch (kind)
    {
    case AdvancedColorKind::WideColorGamut:
        displayString = L"Wide Color Gamut";
        break;

    case AdvancedColorKind::HighDynamicRange:
        displayString = L"High Dynamic Range";
        break;

    case AdvancedColorKind::StandardDynamicRange:
    default:
        displayString = L"Standard Dynamic Range";
        break;
    }

    return displayString;
}

void DirectXPage::OnKeyUp(_In_ CoreWindow ^sender, _In_ KeyEventArgs ^args)
{
    if (VirtualKey::H == args->VirtualKey)
    {
        if (Windows::UI::Xaml::Visibility::Collapsed == ControlsPanel->Visibility)
        {
            OverlayPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
            ControlsPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
        }
        else
        {
            OverlayPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            ControlsPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        }
    }
    else if (VirtualKey::F == args->VirtualKey)
    {
        if (ApplicationView::GetForCurrentView()->IsFullScreenMode)
        {
            ApplicationView::GetForCurrentView()->ExitFullScreenMode();
        }
        else
        {
            ApplicationView::GetForCurrentView()->TryEnterFullScreenMode();
        }
    }
}

void DirectXPage::SliderChanged(_In_ Object^ sender, _In_ RangeBaseValueChangedEventArgs^ e)
{
    UpdateRenderOptions();
}


void DirectXPage::CheckBoxChanged(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    UpdateRenderOptions();
}

// ResizeCompleted is used to detect when the window has been moved between different displays.
void DirectXPage::OnResizeCompleted(_In_ CoreWindow^ sender, _In_  Object^ args)
{
    UpdateRenderOptions();
}

void DirectXPage::ComboChanged(_In_ Object^ sender, _In_ SelectionChangedEventArgs^ e)
{
    UpdateRenderOptions();
}

// Send low level pointer events to GestureRecognizer to be interpreted.
void DirectXPage::OnPointerPressed(_In_ Object^ sender, _In_ PointerRoutedEventArgs^ e)
{
    swapChainPanel->CapturePointer(e->Pointer);
    m_gestureRecognizer->ProcessDownEvent(e->GetCurrentPoint(swapChainPanel));
}

void DirectXPage::OnPointerMoved(_In_ Object^ sender, _In_ PointerRoutedEventArgs^ e)
{
    m_gestureRecognizer->ProcessMoveEvents(e->GetIntermediatePoints(swapChainPanel));
}

void DirectXPage::OnPointerReleased(_In_ Object ^sender, _In_ PointerRoutedEventArgs ^e)
{
    m_gestureRecognizer->ProcessUpEvent(e->GetCurrentPoint(swapChainPanel));
    swapChainPanel->ReleasePointerCapture(e->Pointer);
}

void DirectXPage::OnPointerCanceled(_In_ Object ^sender, _In_ PointerRoutedEventArgs ^e)
{
    m_gestureRecognizer->CompleteGesture();
    swapChainPanel->ReleasePointerCapture(e->Pointer);
}

void DirectXPage::OnPointerWheelChanged(_In_ Object ^sender, _In_ PointerRoutedEventArgs ^e)
{
    // Passing isControlKeyDown = true causes the wheel delta to be treated as scrolling.
    m_gestureRecognizer->ProcessMouseWheelEvent(e->GetCurrentPoint(swapChainPanel), false, true);
}

// GestureRecognizer triggers events on user manipulation of the image content.
void DirectXPage::OnManipulationStarted(_In_ GestureRecognizer ^sender, _In_ ManipulationStartedEventArgs ^args)
{
}

void DirectXPage::OnManipulationUpdated(_In_ GestureRecognizer ^sender, _In_ ManipulationUpdatedEventArgs ^args)
{
    m_renderer->UpdateManipulationState(args);
}

void DirectXPage::OnManipulationCompleted(_In_ GestureRecognizer ^sender, _In_ ManipulationCompletedEventArgs ^args)
{
}