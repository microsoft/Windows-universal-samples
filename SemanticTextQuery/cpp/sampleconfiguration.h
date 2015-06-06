// Copyright (c) Microsoft. All rights reserved.

#pragma once 
#include "pch.h"

namespace SDKTemplate
{
    value struct Scenario;

    partial ref class MainPage
    {
    internal:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return "Semantic text query C++ sample"; 
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

    private:
        static Platform::Array<Scenario>^ scenariosInner;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}

namespace SDKTemplate
{
    void HighlightRanges(Windows::UI::Xaml::Controls::TextBlock^ tb,  Platform::String^ strText, Windows::Foundation::Collections::IVectorView<Windows::Data::Text::TextSegment>^ ranges);
}
