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

namespace DWriteTextLayoutImplementation
{
    // Wrapper for DWRITE_LINE_SPACING_METHOD enumeration.
    // The DWRITE_LINE_SPACING_METHOD enumeration is defined in dwrite.h.
    public enum class LineSpacingMethod
    {
        // Default line spacing depends solely on the content, growing 
        // to accommodate the size of fonts and inline objects.
        Default = DWRITE_LINE_SPACING_METHOD_DEFAULT,

        // Uniform line spacing: the spacing of all lines is determined by 
        // values provided by the app in a DWRITE_LINE_SPACING structure. 
        // One use of uniform spacing is to avoid the uneven appearance 
        // that can occur from font fallback.
        Uniform = DWRITE_LINE_SPACING_METHOD_UNIFORM,

        // Proportional line spacing: the spacing and baseline distances 
        // are based on the computed values based on the content, as with
        // default spacing, but adjusted proportionally by scaling factors
        // you provide in a DWRITE_LINE_SPACING structure.
        Proportional = DWRITE_LINE_SPACING_METHOD_PROPORTIONAL
    };

    // Wrapper for DWRITE_FONT_LINE_GAP_USAGE enumeration. The 
    // DWRITE_FONT_LINE_GAP_USAGE enumeration is defined in dwrite_3.h.
    public enum class FontLineGapUsage
    {
        // Use default behavior for line gap usage. (This is different
        // for GDI-compatible text layout than for normal text layout.)
        Default = DWRITE_FONT_LINE_GAP_USAGE_DEFAULT,

        // The font line gap is excluded from line spacing.
        Disabled = DWRITE_FONT_LINE_GAP_USAGE_DISABLED,

        // The font line gap is included in line spacing.
        Enabled = DWRITE_FONT_LINE_GAP_USAGE_ENABLED
    };


    // Wrapper for DWRITE_LINE_METRICS1 structure.
    public ref class LineMetrics sealed
    {
    public:
        property uint32 length                   { uint32 get() { return m_lineMetrics.length; } }
        property uint32 trailingWhitespaceLength { uint32 get() { return m_lineMetrics.trailingWhitespaceLength; } }
        property uint32 newlineLength            { uint32 get() { return m_lineMetrics.newlineLength; } }
        property bool   isTrimmed                { bool   get() { return !!m_lineMetrics.isTrimmed; } }
        property float  height                   { float  get() { return m_lineMetrics.height; } }
        property float  baseline                 { float  get() { return m_lineMetrics.baseline; } }
        property float  leadingBefore            { float  get() { return m_lineMetrics.leadingBefore; } }
        property float  leadingAfter             { float  get() { return m_lineMetrics.leadingAfter; } }

    internal:
        void SetLineMetrics(DWRITE_LINE_METRICS1 lineMetrics) { m_lineMetrics = lineMetrics; }

    private:
        DWRITE_LINE_METRICS1 m_lineMetrics;
    };



    public ref class TextLayout sealed
    {
    public:
        TextLayout(
            Platform::String^ text,
            Platform::String^ languageTag,
            Platform::String^ fontFamily,
            float fontSize,
            uint32 pixelWidth,
            DWriteTextLayoutImplementation::LineSpacingMethod lineSpacingMethod
            );

        Platform::Array<LineMetrics^>^ GetTextLayoutLineMetrics();

        property uint32 Width
        {
            uint32 get() { return m_pixelWidth; }
        }

        property uint32 Height
        {
            uint32 get() { return m_pixelHeight; }
        }

        property DWriteTextLayoutImplementation::LineSpacingMethod LineSpacingMethod
        {
            DWriteTextLayoutImplementation::LineSpacingMethod get()
            {
                return static_cast<DWriteTextLayoutImplementation::LineSpacingMethod>(m_lineSpacing.method);
            }

            void set(DWriteTextLayoutImplementation::LineSpacingMethod value);
        }

        property float LineSpacingHeight
        {
            float get() { return m_lineSpacing.height; }
            void set(float value);
        }

        property float LineSpacingBaseline
        {
            float get() { return m_lineSpacing.baseline; }
            void set(float value);
        }

        property float LineSpacingLeadingBefore
        {
            float get() { return m_lineSpacing.leadingBefore; }
            void set(float value);
        }

        property FontLineGapUsage FontLineGapUsage
        {
            DWriteTextLayoutImplementation::FontLineGapUsage get()
            {
                return static_cast<DWriteTextLayoutImplementation::FontLineGapUsage>(m_lineSpacing.fontLineGapUsage);
            }

            void set(DWriteTextLayoutImplementation::FontLineGapUsage value);
        }


    internal:
        Microsoft::WRL::ComPtr<IDWriteTextLayout> GetDWriteTextLayout() { return m_textLayout.Get(); }
        DWRITE_TEXT_METRICS GetTextMetrics();
        std::vector<DWRITE_LINE_METRICS1> GetLineMetrics();

    private:
        void SetLineSpacingDefaults();
        void SetLineSpacing();
        void InvalidateLayout();
        void UpdateCachedMetrics();
        void EnsureCachedLineMetrics();

        std::wstring                                        m_text;
        std::wstring                                        m_languageTag;
        std::wstring                                        m_fontFamily;
        float                                               m_fontSize;
        uint32                                              m_pixelWidth; // set in constructor
        uint32                                              m_pixelHeight; // derived from layout

        // DirectWrite objects
        Microsoft::WRL::ComPtr<IDWriteFactory3>             m_dwriteFactory;
        Microsoft::WRL::ComPtr<IDWriteTextLayout3>          m_textLayout;

        // The DWRITE_LINE_SPACING structure is defined in dwrite_3.h and has
        // these members which are used to control the line spacing behavior:
        //     DWRITE_LINE_SPACING_METHOD method
        //     float height
        //     float baseline
        //     float leadingBefore
        //     DWRITE_FONT_LINE_GAP_USAGE fontLineGapUsage
        // The method selected will determine different interpretation and 
        // effect for the other members. See dwrite_3.h for details.
        DWRITE_LINE_SPACING                                 m_lineSpacing;

        // The DWRITE_LINE_METRICS1 structure is defined in dwrite_3.h and
        // is used to report computed metrics for a given line of a text 
        // layout after the layout has been performed. It includes these
        // members:
        //     float height
        //     float baseline
        //     float leadingBefore
        //     float leadingAfter
        std::vector<DWRITE_LINE_METRICS1>                   m_lineMetrics;
    };
}
