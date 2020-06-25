// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// LocalCache.h
// Declaration of the LocalCache class.
//


#include "pch.h"

#pragma once
namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class FontInfoLocal sealed
    {
    public:
        void Set(Windows::UI::Xaml::Controls::TextBlock^ textBlock);
        void Reset(Windows::UI::Xaml::Controls::TextBlock^ textBlock);
        static void SetFont(Windows::UI::Xaml::Controls::TextBlock^ textBlock, Windows::Globalization::Fonts::LanguageFont^ languageFont);
    private:
        Windows::UI::Xaml::Media::FontFamily^  _fontFamily;
        Windows::UI::Text::FontWeight  _fontWeight;
        Windows::UI::Text::FontStyle  _fontStyle;
        Windows::UI::Text::FontStretch  _fontStretch;
    };
}