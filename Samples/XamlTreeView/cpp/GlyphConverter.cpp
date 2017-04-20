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
#include "GlyphConverter.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;

GlyphConverter::GlyphConverter()
{
}

Object ^ GlyphConverter::Convert(Object ^value, TypeName targetType, Object ^parameter, String ^language)
{
    IBox<Boolean>^ isExpanded = dynamic_cast<IBox<Boolean>^>(value);
    if (isExpanded != nullptr && isExpanded->Value)
    {
        return ExpandedGlyph;
    }
    else
    {
        return CollapsedGlyph;
    }
}

Object ^ GlyphConverter::ConvertBack(Object ^value, TypeName targetType, Object ^parameter, String ^language)
{
    throw ref new NotImplementedException();
}
