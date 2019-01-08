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
#include "RenderOptions.h"

namespace D2DAdvancedColorImages
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
        void LoadImage(_In_ Windows::Storage::StorageFile^ imageFile);

        property RenderOptionsViewModel^ ViewModel
        {
            RenderOptionsViewModel^ get() { return m_renderOptionsViewModel; }
        }

    private:
        // UI element event handlers.
        void LoadImageButtonClick(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetButtonClick(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void CheckBoxChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnKeyUp(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::KeyEventArgs^ args);
        void SliderChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void ComboChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

        void UpdateDisplayACState(_In_ Windows::Graphics::Display::AdvancedColorInfo^ info);
        void UpdateDefaultRenderOptions();
        void UpdateRenderOptions();

        // XAML low-level rendering event handler.
        void OnRendering(_In_ Platform::Object^ sender, _In_ Platform::Object^ args);

        // Window event handlers.
        void OnVisibilityChanged(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::VisibilityChangedEventArgs^ args);
        void OnResizeCompleted(_In_ Windows::UI::Core::CoreWindow ^sender, _In_ Platform::Object ^args);

        // DisplayInformation event handlers.
        void OnDpiChanged(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);
        void OnOrientationChanged(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);
        void OnDisplayContentsInvalidated(_In_ Windows::Graphics::Display::DisplayInformation^ sender, _In_ Platform::Object^ args);
        void OnAdvancedColorInfoChanged(_In_ Windows::Graphics::Display::DisplayInformation ^sender, _In_ Platform::Object ^args);

        // Pointer input event handlers (for pan and zoom).
        void OnPointerPressed(_In_ Platform::Object ^sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e);
        void OnPointerMoved(_In_ Platform::Object ^sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e);
        void OnPointerReleased(_In_ Platform::Object ^sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e);
        void OnManipulationStarted(_In_ Windows::UI::Input::GestureRecognizer ^sender, _In_ Windows::UI::Input::ManipulationStartedEventArgs ^args);
        void OnManipulationUpdated(_In_ Windows::UI::Input::GestureRecognizer ^sender, _In_ Windows::UI::Input::ManipulationUpdatedEventArgs ^args);
        void OnManipulationCompleted(_In_ Windows::UI::Input::GestureRecognizer ^sender, _In_ Windows::UI::Input::ManipulationCompletedEventArgs ^args);
        void OnPointerCanceled(_In_ Platform::Object ^sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e);
        void OnPointerWheelChanged(_In_ Platform::Object ^sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e);

        // Other event handlers.
        void OnCompositionScaleChanged(_In_ Windows::UI::Xaml::Controls::SwapChainPanel^ sender, _In_ Object^ args);
        void OnSwapChainPanelSizeChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::SizeChangedEventArgs^ e);

        Platform::String^ ConvertACKindToString(Windows::Graphics::Display::AdvancedColorKind kind);

        // Resources used to draw the DirectX content in the XAML page.
        std::shared_ptr<DX::DeviceResources>            m_deviceResources;
        std::unique_ptr<D2DAdvancedColorImagesRenderer> m_renderer;
        Windows::UI::Input::GestureRecognizer^          m_gestureRecognizer;
        bool                                            m_isWindowVisible;

        // Cached information for UI.
        D2DAdvancedColorImages::ImageInfo               m_imageInfo;
        float                                           m_imageMaxCLL;
        bool                                            m_isImageValid;
        Windows::Graphics::Display::AdvancedColorInfo^  m_dispInfo;
        RenderOptionsViewModel^                         m_renderOptionsViewModel;
    };
}

