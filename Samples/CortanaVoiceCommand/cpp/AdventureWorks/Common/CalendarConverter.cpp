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
#include "CalendarConverter.h"

using namespace AdventureWorks;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;

CalendarConverter::CalendarConverter()
{
}

Object ^ CalendarConverter::Convert(Object ^value, TypeName targetType, Object ^parameter, String ^language)
{
    Calendar^ cal = dynamic_cast<Calendar^>(value);
    if (cal != nullptr)
    {
        return cal->GetDateTime();
    }

    return nullptr;
}

Object ^ CalendarConverter::ConvertBack(Object ^value, TypeName targetType, Object ^parameter, String ^language)
{
    DateTime dt = (DateTime)value;

    Calendar^ cal = ref new Calendar();
    cal->SetDateTime(dt);

    return cal;
}
