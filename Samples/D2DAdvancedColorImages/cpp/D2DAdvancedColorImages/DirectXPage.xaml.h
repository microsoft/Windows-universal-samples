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

#include "DeviceResources.h"
#include "D2DAdvancedColorImagesRenderer.h"

namespace SDKTemplate
{
    /// <summary>
    /// A page that hosts the Direct2D app in a SwapChainPanel, and app UI in XAML.
    /// </summary>
    public ref class DirectXPage sealed
    {
    public:
        DirectXPage();
        virtual ~DirectXPage();

        void SaveInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state);
        void LoadInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state);

        void LoadDefaultImage();
        void LoadImage(Windows::Storage::StorageFile^ imageFile);

    private:
        // UI element event handlers.
        void LoadImageButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

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
        std::unique_ptr<D2DAdvancedColorImages> m_renderer; 
        bool m_isWindowVisible;
    };
}

