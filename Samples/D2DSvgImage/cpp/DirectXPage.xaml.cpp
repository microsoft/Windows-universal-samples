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

using namespace SDKTemplate;

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
    m_isWindowVisible(true)
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

    m_renderer = std::unique_ptr<D2DSvgImageRenderer>(new D2DSvgImageRenderer(m_deviceResources));
}

DirectXPage::~DirectXPage()
{
}

// UI element event handlers.

void DirectXPage::YellowButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    m_renderer->RecolorSvg(D2D1::ColorF(D2D1::ColorF::Yellow));
    m_renderer->Draw();
}

void DirectXPage::GreenButton_Click(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    m_renderer->RecolorSvg(D2D1::ColorF(D2D1::ColorF::Green));
    m_renderer->Draw();
}

void DirectXPage::PurpleButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    m_renderer->RecolorSvg(D2D1::ColorF(D2D1::ColorF::Purple));
    m_renderer->Draw();
}

void DirectXPage::TongueButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    m_renderer->ToggleTongue();
    m_renderer->Draw();
}

void DirectXPage::Slider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    if (m_renderer)
    {
        m_renderer->SetScaleValue((float)e->NewValue);
        m_renderer->Draw();
    }
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

// INotifyPropertyChanged.
void DirectXPage::OnPropertyChanged(String^ propertyName)
{
    PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
}
