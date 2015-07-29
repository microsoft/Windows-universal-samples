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

#include "Scenario1_DefaultSpacing.g.h"
#include "MainPage.xaml.h"

using namespace DWriteTextLayoutImplementation;

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_DefaultSpacing sealed
    {
    public:
        Scenario1_DefaultSpacing();

        // Bindable view-model properties:

        property bool IsFontLineGapUsageEnabled
        {
            bool get() { return (m_textLayout->FontLineGapUsage == FontLineGapUsage::Enabled); }
            void set(bool value);
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
