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
    struct Helpers
    {
        static Windows::UI::Xaml::Controls::TextBlock CreateHeaderTextBlock(param::hstring const& contents);
        static Windows::UI::Xaml::Controls::TextBlock CreateLineItemTextBlock(param::hstring const& contents);
    };
}
