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

namespace winrt
{
    hstring to_hstring(Windows::Web::WebErrorStatus value);
    hstring to_hstring(Windows::Web::Http::HttpResponseMessageSource value);
    hstring to_hstring(Windows::Web::Http::HttpVersion value);
    hstring to_hstring(Windows::Web::Http::HttpProgressStage value);
}

namespace winrt::SDKTemplate
{
}
