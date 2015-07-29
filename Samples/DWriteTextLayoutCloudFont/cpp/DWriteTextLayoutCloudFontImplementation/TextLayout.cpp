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

#include "pch.h"
#include "TextLayout.h"
#include "DirectXHelper.h"

using namespace DWriteTextLayoutCloudFontImplementation;
using namespace Microsoft::WRL;

TextLayout::TextLayout(Platform::String^ text, Platform::String^ languageTag, Platform::String^ fontFamily, float fontSize, float width) :
    m_fontSize(fontSize),
    m_width(width)
{
    m_text.assign(text->Data(), text->Length());
    m_languageTag.assign(languageTag->Data(), languageTag->Length());
    m_fontFamily.assign(fontFamily->Data(), fontFamily->Length());

    // In order to use downloadable fonts, we will need a system font collection 
    // with access to downloadable fonts enabled.

    DX::ThrowIfFailed(
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), &m_dwriteFactory)
        );

    ComPtr<IDWriteFontCollection> systemFontCollection;
    ComPtr<IDWriteFontCollection1> systemFontCollection1;
    DX::ThrowIfFailed(
        m_dwriteFactory->GetSystemFontCollection(true, &systemFontCollection1, false)
        );

    DX::ThrowIfFailed(systemFontCollection1.As(&systemFontCollection));

    // Create the DWriteTextLayout.

    ComPtr<IDWriteTextFormat> textFormat;
    DX::ThrowIfFailed(
        m_dwriteFactory->CreateTextFormat(
            m_fontFamily.c_str(),
            systemFontCollection.Get(),
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            m_fontSize,
            m_languageTag.c_str(),
            &textFormat
            )
        );

    ComPtr<IDWriteTextLayout> textLayout;
    DX::ThrowIfFailed(
        m_dwriteFactory->CreateTextLayout(
            m_text.c_str(),
            static_cast<UINT32>(m_text.length()),
            textFormat.Get(),
            m_width, // format width
            0.0f, // max height
            &textLayout
            )
        );

    DX::ThrowIfFailed(textLayout.As(&m_textLayout));

    // Get the resulting height of the text layout.
    UpdateHeight();

    // We'll use FontNameCollector (an IDWriteRenderer implementation) to collect
    // details about which fonts were actually used. This is for informational
    // purposes, to better understand the behavior of the font download mechanism
    // while the sample app is running. It's not in any way required for using
    // downloadable fonts.
    m_fontNameCollector = new FontNameCollector();
    GetFontNames();
}


Platform::String^ TextLayout::FontsUsed::get()
{
    Platform::String^ fontNames = ref new Platform::String(m_usedFontNames.c_str());
    return fontNames;
}


void TextLayout::InvalidateLayout()
{
    // Calling InvalidateLayout will cause the layout to be performed again using
    // fonts that are locally available. If requested font data was previously not
    // available on the system but has been downloaded, then the locally cached data 
    // will be used.
    m_textLayout->InvalidateLayout();

    // Need to get the updated height of the text layout.
    UpdateHeight();

    // For informational purposes in the sample, get font names used in the updated
    // layout.
    GetFontNames();
}


void TextLayout::Width::set(float value)
{
    m_width = value;
    DX::ThrowIfFailed(
        m_textLayout->SetMaxWidth(value)
        );

    // Need to get the updated height of the text layout.
    UpdateHeight();
}


void TextLayout::UpdateHeight()
{
    // A side effect of accessing the text metrics is that it forces
    // DirectWrite to complete the layout of text in the text layout. That 
    // will include determining whether any downloadable font data may be 
    // needed to display the text using the specified fonts, and to add 
    // those requests into a font download queue. The app using DirectWrite
    // controls when the download requests will get executed.

    DWRITE_TEXT_METRICS textMetrics;
    DX::ThrowIfFailed(
        m_textLayout->GetMetrics(&textMetrics)
        );

    m_height = textMetrics.height;
}


void TextLayout::GetFontNames()
{
    // Assumed that m_fontNamesCollector is already created.

    // Clear any existing names collected for an earlier state of the layout.
    m_fontNameCollector->ClearNames();

    // If we call IDWriteTextLayout::Draw, IDWriteRenderer will get callbacks
    // that will allow it to obtain the font details. That will complete before
    // the call to Draw returns.
    DX::ThrowIfFailed(
        m_textLayout->Draw(NULL, m_fontNameCollector.Get(), 0, 0)
        );

    m_usedFontNames = m_fontNameCollector->GetFontNames();
}
