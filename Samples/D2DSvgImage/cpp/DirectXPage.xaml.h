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

#pragma once

#include "DirectXPage.g.h"

#include "Common\DeviceResources.h"
#include "D2DSvgImageRenderer.h"

namespace SDKTemplate
{
    /// <summary>
    /// A page that hosts the Direct2D app in a SwapChainPanel, and app UI in XAML.
    /// </summary>
    [Windows::UI::Xaml::Data::Bindable]
    public ref class DirectXPage sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        DirectXPage();
        virtual ~DirectXPage();

        void SaveInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state);
        void LoadInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state);

        // INotifyPropertyChanged.
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

    private:
        // UI element event handlers.
        void YellowButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void GreenButton_Click(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void PurpleButton_Click(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void TongueButton_Click(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void Slider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);

        // XAML low-level rendering event handler.
        void OnRendering(_In_ Platform::Object^ sender, _In_ Platform::Object^ args);

        // Window event handlers.
        void OnVisibilityChanged(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::VisibilityChangedEventArgs^ args);

        // DisplayInformation event handlers.
        void OnDpiChanged(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);
        void OnOrientationChanged(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);
        void OnDisplayContentsInvalidated(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);

        // Other event handlers.
        void OnCompositionScaleChanged(_In_ Windows::UI::Xaml::Controls::SwapChainPanel^ sender, _In_ Object^ args);
        void OnSwapChainPanelSizeChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::SizeChangedEventArgs^ e);

        // Resources used to draw the DirectX content in the XAML page.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<D2DSvgImageRenderer> m_renderer; 
        bool m_isWindowVisible;

        // INotifyPropertyChanged.
        void OnPropertyChanged(Platform::String^ propertyName);
    };
}

