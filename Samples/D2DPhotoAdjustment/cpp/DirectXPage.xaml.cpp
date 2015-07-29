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

using namespace D2DPhotoAdjustment;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

DirectXPage::DirectXPage() :
    m_isWindowVisible(true),
    m_suppressRendererUpdates(false),
    m_properties()
{
    InitializeComponent();

    // Register event handlers for page lifecycle.
    CoreWindow^ window = Window::Current->CoreWindow;

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &DirectXPage::OnVisibilityChanged);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnOrientationChanged);

    currentDisplayInformation->ColorProfileChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnColorProfileChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDisplayContentsInvalidated);

    swapChainPanel->CompositionScaleChanged +=
        ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &DirectXPage::OnCompositionScaleChanged);

    swapChainPanel->SizeChanged +=
        ref new SizeChangedEventHandler(this, &DirectXPage::OnSwapChainPanelSizeChanged);

    // At this point we have access to the device and
    // can create the device-dependent resources.
    m_deviceResources = std::make_shared<DX::DeviceResources>();
    m_deviceResources->SetSwapChainPanel(swapChainPanel);

    CreateRenderer();
}

DirectXPage::~DirectXPage()
{
}

void DirectXPage::CreateRenderer()
{
    m_renderer = std::unique_ptr<PhotoAdjustmentRenderer>(new PhotoAdjustmentRenderer(m_deviceResources));

    // Wait until the renderer has been created to reset the photo pipeline values.
    ResetPhotoAdjustmentValues();
}

// Resets the UI sliders and PhotoAdjustmentRenderer state to "neutral" values.
void DirectXPage::ResetPhotoAdjustmentValues()
{
    // Temporarily suppress renderer updates because we are changing a bunch of
    // properties at once.
    m_suppressRendererUpdates = true;

    // IsJpegIndexingEnabled persists across photo adjustment pipeline resets.
    this->StraightenValue = 0.0f;
    this->TemperatureValue = 0.0f;
    this->TintValue = 0.0f;
    this->SaturationValue = 1.0f; // D2D1_SATURATION_PROP_SATURATION defaults to 0.5.
    this->ContrastValue = 0.0f;
    this->ShadowsValue = 0.0f;
    this->ClarityValue = 0.0f;
    this->HighlightsValue = 0.0f;
    this->HsMaskRadiusValue = 1.25f;

    m_suppressRendererUpdates = false;

    TryUpdateRenderer();
}


void DirectXPage::Reset_Click(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    ResetPhotoAdjustmentValues();
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

void DirectXPage::OnColorProfileChanged(_In_ DisplayInformation^ sender, _In_ Object^)
{
    m_renderer->OnColorProfileChanged(sender);
    m_renderer->Draw();
}

void DirectXPage::OnDisplayContentsInvalidated(_In_ DisplayInformation^ sender, _In_ Object^ args)
{
    m_deviceResources->ValidateDevice();
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

// Update the PhotoAdjustmentRenderer with new property values.
// Does nothing in cases where this operation is invalid.
void DirectXPage::TryUpdateRenderer()
{
    if (m_renderer != nullptr && 
        m_suppressRendererUpdates == false)
    {
        m_renderer->UpdatePhotoAdjustmentValues(m_properties);
        m_renderer->Draw();
    }
}

// Databinding properties for the photo adjustment pipeline.

double DirectXPage::TemperatureValue::get() { return m_properties.TemperatureValue; }
void   DirectXPage::TemperatureValue::set(double val)
{
    m_properties.TemperatureValue = val;
    OnPropertyChanged("TemperatureValue");
    TryUpdateRenderer();
}

double DirectXPage::TintValue::get() { return m_properties.TintValue; }
void   DirectXPage::TintValue::set(double val)
{
    m_properties.TintValue = val;
    OnPropertyChanged("TintValue");
    TryUpdateRenderer();
}

double DirectXPage::SaturationValue::get() { return m_properties.SaturationValue; }
void   DirectXPage::SaturationValue::set(double val)
{
    m_properties.SaturationValue = val;
    OnPropertyChanged("SaturationValue");
    TryUpdateRenderer();
}

double DirectXPage::ContrastValue::get() { return m_properties.ContrastValue; }
void   DirectXPage::ContrastValue::set(double val)
{
    m_properties.ContrastValue = val;
    OnPropertyChanged("ContrastValue");
    TryUpdateRenderer();
}

double DirectXPage::ShadowsValue::get() { return m_properties.ShadowsValue; }
void   DirectXPage::ShadowsValue::set(double val)
{
    m_properties.ShadowsValue = val;
    OnPropertyChanged("ShadowsValue");
    TryUpdateRenderer();
}

double DirectXPage::ClarityValue::get() { return m_properties.ClarityValue; }
void   DirectXPage::ClarityValue::set(double val)
{
    m_properties.ClarityValue = val;
    OnPropertyChanged("ClarityValue");
    TryUpdateRenderer();
}

double DirectXPage::HighlightsValue::get() { return m_properties.HighlightsValue; }
void   DirectXPage::HighlightsValue::set(double val)
{
    m_properties.HighlightsValue = val;
    OnPropertyChanged("HighlightsValue");
    TryUpdateRenderer();
}

double DirectXPage::StraightenValue::get() { return m_properties.StraightenValue; }
void   DirectXPage::StraightenValue::set(double val)
{
    m_properties.StraightenValue = val;
    OnPropertyChanged("StraightenValue");
    TryUpdateRenderer();
}

double DirectXPage::HsMaskRadiusValue::get() { return m_properties.HsMaskRadiusValue; }
void   DirectXPage::HsMaskRadiusValue::set(double val)
{
    m_properties.HsMaskRadiusValue = val;
    OnPropertyChanged("HsMaskRadiusValue");
    TryUpdateRenderer();
}

// INotifyPropertyChanged.
void DirectXPage::OnPropertyChanged(String^ propertyName)
{
    PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
}