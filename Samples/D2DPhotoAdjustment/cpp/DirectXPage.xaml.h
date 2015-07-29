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
#include "PhotoAdjustmentRenderer.h"
#include "PhotoAdjustmentProperties.h"

namespace D2DPhotoAdjustment
{
    /// <summary>
    /// A page that hosts the Direct2D photo editor in a SwapChainPanel, and photo pipeline UI.
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

        // Databinding properties for the photo adjustment pipeline.
        property double StraightenValue     { double get(); void set(double val); }
        property double TemperatureValue    { double get(); void set(double val); }
        property double TintValue           { double get(); void set(double val); }
        property double SaturationValue     { double get(); void set(double val); }
        property double ContrastValue       { double get(); void set(double val); }
        property double ShadowsValue        { double get(); void set(double val); }
        property double ClarityValue        { double get(); void set(double val); }
        property double HighlightsValue     { double get(); void set(double val); }
        property double HsMaskRadiusValue   { double get(); void set(double val); }

    private:
        void CreateRenderer();
        void ResetPhotoAdjustmentValues();
        void TryUpdateRenderer();

        // UI element event handlers.
        void Slider_ValueChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void Reset_Click(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);

        // XAML low-level rendering event handler.
        void OnRendering(_In_ Platform::Object^ sender, _In_ Platform::Object^ args);

        // Window event handlers.
        void OnVisibilityChanged(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::VisibilityChangedEventArgs^ args);

        // DisplayInformation event handlers.
        void OnDpiChanged(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);
        void OnOrientationChanged(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);
        void OnDisplayContentsInvalidated(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);
        void OnColorProfileChanged(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);

        // Other event handlers.
        void OnCompositionScaleChanged(_In_ Windows::UI::Xaml::Controls::SwapChainPanel^ sender, _In_ Object^ args);
        void OnSwapChainPanelSizeChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::SizeChangedEventArgs^ e);

        // Resources used to draw the DirectX content in the XAML page.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<PhotoAdjustmentRenderer> m_renderer; 
        bool m_isWindowVisible;
        bool m_suppressRendererUpdates;

        PhotoAdjustmentProperties m_properties;

        // INotifyPropertyChanged.
        void OnPropertyChanged(Platform::String^ propertyName);
    };
}

