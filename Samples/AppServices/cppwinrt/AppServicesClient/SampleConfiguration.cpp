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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"AppServicesClient C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Open/Close Connection", xaml_typename<OpenCloseConnectionScenario>() },
    Scenario{ L"Keep Connection Open", xaml_typename<KeepConnectionOpenScenario>() },
});

bool SDKTemplate::TryParseInt32(wchar_t const* text, int32_t& value)
{
    wchar_t* end;
    errno = 0;
    value = std::wcstol(text, &end, 10);

    if (text == end || *end != L'\0')
    {
        // Not parseable.
        return false;
    }

    if (errno == ERANGE)
    {
        // Out of range.
        return false;
    }

    return true;
}
