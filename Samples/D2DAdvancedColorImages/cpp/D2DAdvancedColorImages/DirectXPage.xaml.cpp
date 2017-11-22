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
    m_dispInfo{}
{
    InitializeComponent();

    // Register event handlers for page lifecycle.
    CoreWindow^ window = Window::Current->CoreWindow;

    window->KeyUp += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &DirectXPage::OnKeyUp);

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &DirectXPage::OnVisibilityChanged);

    window->ResizeCompleted +=
        ref new TypedEventHandler<CoreWindow ^, Platform::Object ^>(this, &DirectXPage::OnResizeCompleted);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnOrientationChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDisplayContentsInvalidated);

    swapChainPanel->CompositionScaleChanged +=
        ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &DirectXPage::OnCompositionScaleChanged);

    swapChainPanel->SizeChanged +=
        ref new SizeChangedEventHandler(this, &DirectXPage::OnSwapChainPanelSizeChanged);

    m_renderOptionsViewModel = ref new RenderOptionsViewModel();

    // At this point we have access to the device and
    // can create the device-dependent resources.
    m_deviceResources = std::make_shared<DX::DeviceResources>();
    m_deviceResources->SetSwapChainPanel(swapChainPanel);

    m_renderer = std::unique_ptr<D2DAdvancedColorImagesRenderer>(new D2DAdvancedColorImagesRenderer(m_deviceResources, this));
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
    create_task(imageFile->OpenAsync(FileAccessMode::Read)).then([=](task<IRandomAccessStream^> previousTask) {
        IRandomAccessStream^ ras;
        try
        {
            ras = previousTask.get();
        }
        catch (...)
        {
            // If the file cannot be opened, then do nothing.
            return;
        }

        ComPtr<IStream> iStream;
        DX::ThrowIfFailed(CreateStreamOverRandomAccessStream(ras, IID_PPV_ARGS(&iStream)));

        m_imageInfo = m_renderer->LoadImage(iStream.Get());

        ApplicationView::GetForCurrentView()->Title = imageFile->Name;
        ImageHasColorProfile->Text = L"Color profile: " + (m_imageInfo.numProfiles > 0 ? L"Yes" : L"No");
        ImageBitDepth->Text = L"Bits per pixel: " + ref new String(std::to_wstring(m_imageInfo.bitsPerPixel).c_str());
        ImageIsFloat->Text = L"Floating point: " + (m_imageInfo.isFloat ? L"Yes" : L"No");

        m_renderer->CreateImageDependentResources();
        m_renderer->FitImageToWindow();

        // After image is loaded, reset rendering options to defaults.
        ScalingCheckBox->IsChecked = false;
        TonemappersCombo->SelectedIndex = 0; // See RenderOptions.h for which value this indicates.
        ColorspacesCombo->SelectedIndex = 0; // See RenderOptions.h for which value this indicates.
        WhiteLevelSlider->Value = 1.0f;

        m_renderer->Draw();
    });
}

void DirectXPage::OnDisplayACStateChanged(float maxLuminance, unsigned int bitDepth, DisplayACKind displayKind)
{
    String^ displayString;
    switch (displayKind)
    {
    case DisplayACKind::AdvancedColor_LowDynamicRange:
        displayString = L"Advanced color, LDR";
        break;

    case DisplayACKind::AdvancedColor_HighDynamicRange:
        displayString = L"Advanced color, HDR";
        break;

    case DisplayACKind::NotAdvancedColor:
    default:
        displayString = L"Not advanced color";
        break;
    }

    DisplayACState->Text = L"Kind: " + displayString;
    DisplayPeakLuminance->Text = L"Peak luminance: " + 
        ref new String(std::to_wstring(static_cast<unsigned int>(maxLuminance)).c_str()) + L" nits";

    DisplayBitDepth->Text = L"Bits per channel: " + ref new String(std::to_wstring(bitDepth).c_str());

    // White level scaling should only be enabled where overbright/high dynamic range is available.
    // Tonemapping should only be enabled in the converse situation (low dynamic range); they don't benefit
    // from adjusting white level since 1.0 white is simply mapped to the maximum luminance of the display.
    if (displayKind == DisplayACKind::AdvancedColor_HighDynamicRange)
    {
        WhiteLevelPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
        TonemapperPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    else
    {
        WhiteLevelSlider->Value = 1.0f;
        WhiteLevelPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        TonemapperPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
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

// Common method for updating options on the renderer.
void DirectXPage::UpdateRenderOptions()
{
    if ((m_renderer != nullptr) &&
        ColorspacesCombo->SelectedItem &&
        TonemappersCombo->SelectedItem
        )
    {
        auto cs = static_cast<ColorspaceOption^>(ColorspacesCombo->SelectedItem);
        auto tm = static_cast<TonemapperOption^>(TonemappersCombo->SelectedItem);

        m_renderer->SetRenderOptions(
            ScalingCheckBox->IsChecked->Value,
            tm->Tonemapper,
            static_cast<float>(WhiteLevelSlider->Value),
            static_cast<DXGI_COLOR_SPACE_TYPE>(cs->Colorspace)
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
}

void DirectXPage::SliderChanged(Object ^ sender, RangeBaseValueChangedEventArgs ^ e)
{
    UpdateRenderOptions();
}


void DirectXPage::CheckBoxChanged(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    UpdateRenderOptions();
}

// ResizeCompleted is used to detect when the window has been moved between different displays.
void DirectXPage::OnResizeCompleted(CoreWindow ^sender, Object ^args)
{
    UpdateRenderOptions();
}

void DirectXPage::ComboChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    UpdateRenderOptions();
}
