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

#include "FontNameCollector.h"

namespace DWriteTextLayoutCloudFontImplementation
{
    // This sample uses a SurfaceImageSource as the means of presenting a DirectWrite
    // text layout within the XAML UI. The dimensions of the SurfaceImageSource need
    // to be determined when it is constructed. Therefore, a text layout needs to be
    // created first, and then the dimensions of the layout can be used to determine
    // the dimensions of the SurfaceImageSource.
    //
    // The width of the TextLayout will be determined by the caller (based on
    // the UI state). Then the height will get set based on the layout 
    // requirements.

    public ref class TextLayout sealed
    {
    public:
        TextLayout(Platform::String^ text, Platform::String^ languageTag, Platform::String^ fontFamily, float fontSize, float width);

        void InvalidateLayout(); // Will be called after downloadable font is available locally.

        property float Width
        {
            float get() { return m_width; }
            void set(float value);
        }

        property float Height
        {
            float get() { return m_height; }
        }

        property Platform::String^ FontsUsed
        {
            Platform::String^ get();
        }

    internal:
        Microsoft::WRL::ComPtr<IDWriteTextLayout> GetDWriteTextLayout() { return m_textLayout.Get(); }

    private:
        void UpdateHeight();
        void GetFontNames();

        std::wstring                                        m_text;
        std::wstring                                        m_languageTag;
        std::wstring                                        m_fontFamily;
        float                                               m_fontSize;
        float                                               m_width; // set in constructor or in Width setter, in DIPs
        float                                               m_height; // derived from layout, in DIPs
        std::wstring                                        m_usedFontNames; // derived from layout

        // DirectWrite objects
        Microsoft::WRL::ComPtr<IDWriteFactory3>             m_dwriteFactory;
        Microsoft::WRL::ComPtr<IDWriteTextLayout3>          m_textLayout;
        Microsoft::WRL::ComPtr<FontNameCollector>           m_fontNameCollector; // IDWriteRenderer implementation, used to obtain information about the fonts used in text layout.
    };
}
