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
#include "ScenarioInput1.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::Fonts;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;

void FontInfoLocal::Set(TextBlock^ textBlock)
{
    _fontFamily = textBlock->FontFamily;
    _fontWeight = textBlock->FontWeight;
    _fontStyle = textBlock->FontStyle;
    _fontStretch = textBlock->FontStretch;
}

void FontInfoLocal::Reset(TextBlock^ textBlock)
{
    String^ fonName = textBlock->FontFamily->Source;
    textBlock->FontFamily = _fontFamily;
    textBlock->FontWeight = _fontWeight;
    textBlock->FontStyle = _fontStyle;
    textBlock->FontStretch = _fontStretch;
}

void FontInfoLocal::SetFont(TextBlock^ textBlock, Windows::Globalization::Fonts::LanguageFont^ languageFont)
{
    String^ fonName = languageFont->FontFamily;
    FontFamily^ fontFamily = ref new FontFamily(languageFont->FontFamily);
    textBlock->FontFamily = fontFamily;
    textBlock->FontWeight = languageFont->FontWeight;
    textBlock->FontStyle = languageFont->FontStyle;
    textBlock->FontStretch = languageFont->FontStretch;
}