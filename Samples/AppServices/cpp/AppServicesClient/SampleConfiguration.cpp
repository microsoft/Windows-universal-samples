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
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Open/Close Connection", "SDKTemplate.OpenCloseConnectionScenario" },
    { "Keep Connection Open", "SDKTemplate.KeepConnectionOpenScenario" }
};

// Utility function to convert a string to an int32_t and detect bad input
bool SDKTemplate::TryParseInt(const wchar_t* str, _Out_ int32_t* result)
{
    wchar_t* end;
    errno = 0;
    *result = std::wcstol(str, &end, 10);

    if (str == end)
    {
        // Not parseable.
        return false;
    }

    if (errno == ERANGE || *result < INT_MIN || INT_MAX < *result)
    {
        // Out of range.
        return false;
    }

    if (*end != L'\0')
    {
        // Extra unparseable characters at the end.
        return false;
    }

    return true;
}
