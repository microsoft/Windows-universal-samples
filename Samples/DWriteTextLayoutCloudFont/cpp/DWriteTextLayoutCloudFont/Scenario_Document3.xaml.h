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

#include "Scenario_Document3.g.h"
#include "MainPage.xaml.h"

using namespace DWriteTextLayoutCloudFontImplementation;

using Windows::Graphics::Display::DisplayInformation;

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario_Document3 sealed
    {
    public:
        Scenario_Document3();
        void HandleDpiChanged(DisplayInformation^ displayInformation);

    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^                                       rootPage;
        TextLayout^                                     m_textLayout;
        TextLayoutImageSource^                          m_textLayoutImageSource;
        FontDownloadListener^                           m_fontDownloadListener;
        Windows::Foundation::EventRegistrationToken     m_eventToken;
        Windows::UI::Color                              m_textColor;
        Windows::UI::Color                              m_textBackgroundColor;
        Platform::String^                               m_downloadableFontName;
        bool                                            m_layoutUpdateInProgress = false;
        float                                           m_dpi = 96.0f;

        void TextLayoutFrame_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
        void FontDownloadListener_DownloadCompleted();
        void RequestTextLayoutUpdate();
        void UpdateTextLayout();
        void PresentTextLayout();
        void UpdateStatus();
        void OnVisibilityChanged(Platform::Object ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^e);
    };
}
