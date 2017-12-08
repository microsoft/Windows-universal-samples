#pragma once
#include "pch.h"
#include "IntegerToIndentationConverter.h"

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;

namespace TreeViewControl {

    IntegerToIndentationConverter::IntegerToIndentationConverter()
    {
        //default
        indentMultiplier = 20;
    }

    Object^ IntegerToIndentationConverter::Convert(Object^ value, TypeName targetType, Object^ parameter, String^ language)
    {
        Thickness indent(0);
        if (value != nullptr)
        {
            indent.Left = (int)value * indentMultiplier;
            return indent;
        }

        return indent;
    }

    Object^ IntegerToIndentationConverter::ConvertBack(Object^ value, TypeName targetType, Object^ parameter, String^ language)
    {
        throw ref new NotImplementedException();
    }
}