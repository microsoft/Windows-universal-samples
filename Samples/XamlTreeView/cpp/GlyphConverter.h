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
namespace SDKTemplate
{
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class GlyphConverter sealed : Windows::UI::Xaml::Data::IValueConverter
    {
    public:
        GlyphConverter();

        // Inherited via IValueConverter
        virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
        virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);

        property Platform::String^ ExpandedGlyph
        {
            Platform::String^ get() { return expandedGlyph; }
            void set(Platform::String^ value) { expandedGlyph = value; }
        }

        property Platform::String^ CollapsedGlyph
        {
            Platform::String^ get() { return collapsedGlyph; }
            void set(Platform::String^ value) { collapsedGlyph = value; }
        }
    private:
        Platform::String^ expandedGlyph = nullptr;
        Platform::String^ collapsedGlyph = nullptr;
    };
}

