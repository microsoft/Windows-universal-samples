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
#include "BoolToVisibilityConverter.h"

using namespace AdventureWorks;

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;

BoolToVisibilityConverter::BoolToVisibilityConverter()
{
}

Object ^ BoolToVisibilityConverter::Convert(Object ^value, TypeName targetType, Object ^parameter, String ^language)
{
    IBox<Boolean>^ b = dynamic_cast<IBox<Boolean>^>(value);
    if (b != nullptr)
    {
        if (b->Value)
        {
            return Visibility::Visible;
        }
        else
        {
            return Visibility::Collapsed;
        }
    }

    return Visibility::Collapsed;
}

Object ^ BoolToVisibilityConverter::ConvertBack(Object ^value, TypeName targetType, Object ^parameter, String ^language)
{
    throw ref new NotImplementedException();
}
