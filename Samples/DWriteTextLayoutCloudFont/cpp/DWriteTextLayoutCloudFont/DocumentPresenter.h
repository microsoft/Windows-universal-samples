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

#include "pch.h"

namespace SDKTemplate
{
    public ref class DocumentPresenter sealed
    {
    public:
        DocumentPresenter(
            Platform::String^ text,
            Platform::String^ downloadableFontName,
            Platform::String^ languageTag,
            Windows::UI::Color textColor,
            Windows::UI::Color textBackgroundColor,
            Windows::UI::Xaml::Controls::Image^ imageTarget
        );

        virtual ~DocumentPresenter();

        void SetLayoutWidth(float width);

    private:
        Platform::String^                                                   m_downloadableFontName; // Storing the requested font so we can later check whether it's the one actually used in the layout.
        Windows::UI::Color                                                  m_textColor;
        Windows::UI::Color                                                  m_textBackgroundColor;
        DWriteTextLayoutCloudFontImplementation::TextLayout^                m_textLayout;
        DWriteTextLayoutCloudFontImplementation::TextLayoutImageSource^     m_textLayoutImageSource;
        Windows::UI::Xaml::Controls::Image^                                 m_imageTarget; // The control in the owner page that will use the surface image source as its source.
        DWriteTextLayoutCloudFontImplementation::FontDownloadListener^      m_fontDownloadListener;
        float                                                               m_dpi = 96.0f;
        Windows::Graphics::Display::DisplayInformation^                     m_displayInformation;
        bool                                                                m_layoutUpdateInProgress = false;

        Windows::Foundation::EventRegistrationToken                         m_downloadCompletedEventToken;
        Windows::Foundation::EventRegistrationToken                         m_dpiChangedEventToken;
        Windows::Foundation::EventRegistrationToken                         m_visibilityChangedEventToken;

        void PresentTextLayout();
        void RequestTextLayoutUpdate();
        void UpdateStatus();
        void UpdateTextLayout();

        void DisplayInformation_DpiChanged(Windows::Graphics::Display::DisplayInformation^ displayInformation, Platform::Object^ e);
        void FontDownloadListener_DownloadCompleted();
        void Window_VisibilityChanged(Platform::Object ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^e);
    }; // class DocumentPresenter

} // namespace SDKTemplate
