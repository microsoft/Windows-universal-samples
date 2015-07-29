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

using namespace DWriteTextLayoutImplementation;
using namespace Platform;
using namespace Microsoft::WRL;

/// <summary>
/// Constructor for the TextLayout class, which wraps the IDWriteTextLayout3 API
/// to allow interop from XAML apps. A text layout will be created using the text,
/// font and line-spacing method specified. Other methods allow the line-spacing
/// parameters to be changed.
/// </summary>
/// <param name="text">The text to be laid out.</param>
/// <param name="languageTag">The language of the text.</param>
/// <param name="fontFamily">The font family to be used in the text layout. (Normal
/// weight, width and style will be used.)</param>
/// <param name="fontSize">The font size to be used, in DIPs.</param>
/// <param name="pixelWidth">The width for the text layout. (Height will be derived.)</param>
/// <param name="lineSpacingMethod">The line spacing method to be used for the layout.</param>
TextLayout::TextLayout(
    Platform::String^ text, 
    Platform::String^ languageTag, 
    Platform::String^ fontFamily, 
    float fontSize,
    uint32 pixelWidth,
    DWriteTextLayoutImplementation::LineSpacingMethod lineSpacingMethod
    ) :
    m_text(text->Data(), text->Length()),
    m_languageTag(languageTag->Data(), languageTag->Length()),
    m_fontFamily(fontFamily->Data(), fontFamily->Length()),
    m_fontSize(fontSize),
    m_pixelWidth(pixelWidth),
    m_pixelHeight(0)
{
    m_lineSpacing.method = static_cast<DWRITE_LINE_SPACING_METHOD>(lineSpacingMethod);

    DX::ThrowIfFailed(
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), &m_dwriteFactory)
        );

    // Create the IDWriteTextLayout.

    ComPtr<IDWriteTextFormat> textFormat;
    DX::ThrowIfFailed(
        m_dwriteFactory->CreateTextFormat(
            m_fontFamily.c_str(),
            nullptr,    // default font collection
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
            static_cast<float>(m_pixelWidth), // format width
            0.0f, // max height
            &textLayout
            )
        );

    DX::ThrowIfFailed(textLayout.As(&m_textLayout));

    // Before setting the line spacing method, select some default
    // parameters according to the method being used.
    SetLineSpacingDefaults();

    // Set the line spacing method.
    SetLineSpacing();
}


/// <summary>
/// Returns the line metrics for the text layout. The native implementation within
/// the TextLayout class obtains these as DWRITE_LINE_METRICS1 structures; these
/// are returned wrapped in the LineMetrics class.
/// </summary>
Platform::Array<LineMetrics^>^ TextLayout::GetTextLayoutLineMetrics()
{
    EnsureCachedLineMetrics();

    Platform::Array<LineMetrics^>^ results = ref new Platform::Array<LineMetrics^>(static_cast<unsigned int>(m_lineMetrics.size()));

    for (uint32 i = 0; i < m_lineMetrics.size(); i++)
    {
        LineMetrics^ line = ref new LineMetrics();
        line->SetLineMetrics(m_lineMetrics[i]);
        results[i] = line;
    }

    return results;
}



/// <summary>
/// Native API to obtain the DWRITE_TEXT_METRICS of the underlying 
/// IDWriteTextLayout3 object. Provided for use in TextLayoutImageSource.
/// </summary>
DWRITE_TEXT_METRICS TextLayout::GetTextMetrics()
{
    DWRITE_TEXT_METRICS textMetrics;
    DX::ThrowIfFailed(m_textLayout.Get()->GetMetrics(&textMetrics));
    return textMetrics;
}


/// <summary>
/// Native API to obtain the DWRITE_LINE_METRICS1 structures from the 
/// underlying IDWriteTextLayout3 object. Provided for use in 
/// TextLayoutImageSource.
/// </summary>
std::vector<DWRITE_LINE_METRICS1> TextLayout::GetLineMetrics()
{
    EnsureCachedLineMetrics();
    return m_lineMetrics;
}



// property setters

/// <summary>
/// Sets the line spacing method to be used. In the native implementation within
/// TextLayout, this is setting DWRITE_LINE_SPACING.method.
/// <summary>
/// <remarks>
/// The method selected will impact how other line-spacing parameters are interpreted.
/// </remarks>
void TextLayout::LineSpacingMethod::set(DWriteTextLayoutImplementation::LineSpacingMethod value)
{
    m_lineSpacing.method = static_cast<DWRITE_LINE_SPACING_METHOD>(value);
    SetLineSpacing();
}


/// <summary>
/// Sets the line spacing height. In the native implementation within TextLayout,
/// this is setting DWRITE_LINE_SPACING.height.
/// </summary>
/// <param name="value">The line spacing height.</param>
/// <remarks>
/// Interpretation of DWRITE_LINE_SPACING.height depends on the line spacing method
/// (DWRITE_LINE_SPACING.method), as follows:
/// Default line spacing: ignored.
/// Uniform line spacing: the explicit distance between lines, in DIPs.
/// Proportional line spacing: a scaling factor applied to the default computed height of each line.
/// </remarks>
void TextLayout::LineSpacingHeight::set(float value)
{
    m_lineSpacing.height = value;
    SetLineSpacing();
}


/// <summary>
/// Sets the line spacing baseline distance. In the native implementation within 
/// TextLayout, this is setting DWRITE_LINE_SPACING.baseline.
/// </summary>
/// <param name="value">The line spacing baseline distance.</param>
/// <remarks>
/// Interpretation of DWRITE_LINE_SPACING.baseline depends on the line spacing 
/// method (DWRITE_LINE_SPACING.method), as follows:
/// Default line spacing: ignored.
/// Uniform line spacing: the explicit distance from the top of the line to the baseline, in DIPs.
/// Proportional line spacing: a scaling factor applied to the default computed baseline of each line.
/// </remarks>
void TextLayout::LineSpacingBaseline::set(float value)
{
    m_lineSpacing.baseline = value;
    SetLineSpacing();
}


/// <summary>
/// Sets the proportion of extra leading distributed before the line, rather than 
/// after. In the native implementation within TextLayout, this is setting 
/// DWRITE_LINE_SPACING.leadingBefore.
/// </summary>
/// <param name="value">The proportion of extra leading applied before the line.</param>
/// <remarks>
/// Interpretation of DWRITE_LINE_SPACING.leadingBefore depends on the line spacing 
/// method (DWRITE_LINE_SPACING.method), as follows:
/// Default line spacing: ignored.
/// Uniform line spacing: ignored.
/// Proportional line spacing: a value between 0.0 and 1.0 indicating the proportion of the total leading distributed before the line.
/// </remarks>
void TextLayout::LineSpacingLeadingBefore::set(float value)
{
    m_lineSpacing.leadingBefore = value;
    SetLineSpacing();
}


/// <summary>
/// Sets whether line gap data in the font (if present) is to be used in computing
/// line spacing for each line. Ignored when the uniform line spacing method is used.
/// </summary>
void TextLayout::FontLineGapUsage::set(DWriteTextLayoutImplementation::FontLineGapUsage value)
{
    m_lineSpacing.fontLineGapUsage = static_cast<DWRITE_FONT_LINE_GAP_USAGE>(value);
    SetLineSpacing();
}



// private helper methods

void TextLayout::SetLineSpacingDefaults()
{
    // Used when the text layout is first created to set default
    // line spacing parameters according to the line spacing
    // method being used.
    //
    // Assumed: the m_lineSpacing.method value has been set.

    // Common to all cases:
    m_lineSpacing.fontLineGapUsage = DWRITE_FONT_LINE_GAP_USAGE_DISABLED;

    // These variables are used for the uniform spacing case.
    std::vector<DWRITE_LINE_METRICS1> lineMetrics;
    DWRITE_LINE_METRICS1 line;

    switch (m_lineSpacing.method)
    {
    case DWRITE_LINE_SPACING_METHOD_DEFAULT:
        // The height, baseline and leadingBefore members are ignored.
        return;

    case DWRITE_LINE_SPACING_METHOD_UNIFORM: 
        // A reasonable default will be to get height and baseline values
        // from the first line of the default layout.
        lineMetrics = this->GetLineMetrics();
        line = lineMetrics[0];

        m_lineSpacing.height = line.height;
        m_lineSpacing.baseline = line.baseline;
        // The leadingBefore member is ignored.
        return;

    case DWRITE_LINE_SPACING_METHOD_PROPORTIONAL:
        m_lineSpacing.height = 1.0f;
        m_lineSpacing.baseline = 1.0f;
        m_lineSpacing.leadingBefore = 0.0f;
        return;
    }
}


void TextLayout::SetLineSpacing()
{
    DX::ThrowIfFailed(
        m_textLayout->SetLineSpacing(&m_lineSpacing)
        );
    InvalidateLayout();
}


void TextLayout::InvalidateLayout()
{
    // Calling InvalidateLayout will cause the layout to be performed again the
    // next time it is used (draw, get metrics, etc.) using the line spacing 
    // parameters that are currently set.
    m_textLayout->InvalidateLayout();

    // Invalidate the cached line metrics.
    m_lineMetrics.clear();

    // Get the updated height and line metrics of the text layout.
    UpdateCachedMetrics();
}


void TextLayout::UpdateCachedMetrics()
{
    // Get the total height of the text layout.
    DWRITE_TEXT_METRICS textMetrics;
    DX::ThrowIfFailed(
        m_textLayout->GetMetrics(&textMetrics)
        );

    m_pixelHeight = static_cast<uint32>(ceilf(textMetrics.height));

    // Get the line metrics.
    EnsureCachedLineMetrics();
}


void TextLayout::EnsureCachedLineMetrics()
{
    // Any time the line metrics need to be accessed, we need to make
    // sure we have collected them. But we don't need to re-do that 
    // every time they might need to be accessed; we only need to do 
    // that if they've never been collected or if they had been 
    // collected but then became invalid (hence cleared) as a result
    // in a change to the layout (the line spacing parameters were
    // changed.

    // If m_lineMetrics has entries, assume they're valid. Otherwise,
    // get the line metrics.
    if (!m_lineMetrics.empty()) return;

    // We will need to determine how many lines there are to set the size
    // of the vector. If we had a reason to call IDWriteTextLayout::GetMetrics
    // and had cached the DWRITE_TEXT_METRICS structure, then we could get
    // the line count directly from DWRITE_TEXT_METRICS.lineCount. Since we
    // haven't cached the text metrics, we can call GetLineMetrics with a
    // tentative count and and vector size, and it will return the actual
    // line count. If our tentative count is too small, there isn't any
    // perf hit of copying values into the vector on both first and second
    // calls since the GetLineMetrics call will return on the first call
    // before it even initializes the output buffer.

    // Initialize the line metrics array to an estimated size.
    uint32 lineCount = 8;
    m_lineMetrics.resize(lineCount);

    // Get the line metrics, and then resize the array based on the actual line count.
    HRESULT hr = m_textLayout.Get()->GetLineMetrics(&m_lineMetrics[0], lineCount, &lineCount);
    m_lineMetrics.resize(lineCount);

    // If the initial buffer size was too small, get the line metrics again.
    if (hr == E_NOT_SUFFICIENT_BUFFER)
    {
        hr = m_textLayout.Get()->GetLineMetrics(&m_lineMetrics[0], lineCount, &lineCount);
    }

    DX::ThrowIfFailed(hr);
}


