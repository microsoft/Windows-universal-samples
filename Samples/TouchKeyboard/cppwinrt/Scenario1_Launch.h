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

#include "Scenario1_Launch.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Launch : Scenario1_LaunchT<Scenario1_Launch>
    {
        Scenario1_Launch();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Launch : Scenario1_LaunchT<Scenario1_Launch, implementation::Scenario1_Launch>
    {
    };
}

#include "CustomTextBox.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct CustomTextBox : CustomTextBoxT<CustomTextBox>
    {
        CustomTextBox()
        {
            FontFamily(Windows::UI::Xaml::Media::FontFamily{ L"Times New Roman" });
            FontStyle(Windows::UI::Text::FontStyle::Italic);
            BorderThickness({ 1 });
        }

    };
}
namespace winrt::SDKTemplate::factory_implementation
{
    struct CustomTextBox : CustomTextBoxT<CustomTextBox, implementation::CustomTextBox>
    {
    };
}
