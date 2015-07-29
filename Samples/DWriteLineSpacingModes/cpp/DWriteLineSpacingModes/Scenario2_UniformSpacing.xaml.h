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

#include "Scenario2_UniformSpacing.g.h"

using namespace DWriteTextLayoutImplementation;
using namespace Windows::UI::Xaml::Interop;

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_UniformSpacing sealed
    {
    public:
        Scenario2_UniformSpacing();

        // bindable view-model properties:

        property double LineSpacingHeight
        {
            double get() { return static_cast<double>(m_textLayout->LineSpacingHeight); }
            void set(double value);
        }

        property double LineSpacingBaseline
        {
            double get() { return static_cast<double>(m_textLayout->LineSpacingBaseline); }
            void set(double value);
        }

        property Platform::String^ TextLayoutHeight
        {
            Platform::String^ get() { return m_textLayoutLineMetrics[0]->height.ToString(); }
        }

        property Platform::String^ TextLayoutBaseline
        {
            Platform::String^ get() { return m_textLayoutLineMetrics[0]->baseline.ToString(); }
        }

        property Platform::String^ TextLayoutLeadingBefore
        {
            Platform::String^ get() { return m_textLayoutLineMetrics[0]->leadingBefore.ToString(); }
        }

        property Platform::String^ TextLayoutLeadingAfter
        {
            Platform::String^ get() { return m_textLayoutLineMetrics[0]->leadingAfter.ToString(); }
        }

    private:
        TextLayout^                                     m_textLayout;
        TextLayoutImageSource^                          m_textLayoutImageSource;
        Platform::Array<LineMetrics^>^                  m_textLayoutLineMetrics;
    };
}
