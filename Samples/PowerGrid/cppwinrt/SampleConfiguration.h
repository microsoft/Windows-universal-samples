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
#include "pch.h"

namespace winrt::SDKTemplate
{
    inline auto GetDateFormatter()
    {
        return winrt::Windows::Globalization::DateTimeFormatting::DateTimeFormatter(L"shortdate shorttime");
    }

    inline auto GetSeverityFormatter()
    {
        winrt::Windows::Globalization::NumberFormatting::DecimalFormatter severityFormatter;
        severityFormatter.FractionDigits(2);
        severityFormatter.IntegerDigits(1);
        severityFormatter.IsDecimalPointAlwaysDisplayed(true);
        winrt::Windows::Globalization::NumberFormatting::IncrementNumberRounder rounder;
        rounder.Increment(0.01);
        severityFormatter.NumberRounder(rounder);
        return severityFormatter;
    }
}
