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

#include "Scenario_Document1.g.h"
#include "MainPage.xaml.h"

using namespace DWriteTextLayoutCloudFontImplementation;

using Windows::Graphics::Display::DisplayInformation;

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario_Document1 sealed
    {
    public:
        Scenario_Document1();

    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^                                       m_rootPage;
        TextLayout^                                     m_textLayout;
        TextLayoutImageSource^                          m_textLayoutImageSource;
        FontDownloadListener^                           m_fontDownloadListener;
        Windows::UI::Color                              m_textColor;
        Windows::UI::Color                              m_textBackgroundColor;
        Platform::String^                               m_downloadableFontName;
        bool                                            m_layoutUpdateInProgress = false;
        Windows::Graphics::Display::DisplayInformation^ m_displayInformation;
        float                                           m_dpi = 96.0f;

        Windows::Foundation::EventRegistrationToken     m_downloadCompletedEventToken{};
        Windows::Foundation::EventRegistrationToken     m_dpiChangedEventToken{};
        Windows::Foundation::EventRegistrationToken     m_visibilityChangedEventToken{};

        void RequestTextLayoutUpdate();
        void UpdateTextLayout();
        void PresentTextLayout();
        void UpdateStatus();
        void DisplayInformation_DpiChanged(DisplayInformation^ sender, Object^ e);
        void FontDownloadListener_DownloadCompleted();
        void TextLayoutFrame_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
        void Window_VisibilityChanged(Platform::Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e);
    };
}
