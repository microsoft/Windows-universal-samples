﻿//*********************************************************
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
#include "Scenario2_SimpleGridView.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_SimpleGridView : Scenario2_SimpleGridViewT<Scenario2_SimpleGridView>
    {
        Scenario2_SimpleGridView();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_SimpleGridView : Scenario2_SimpleGridViewT<Scenario2_SimpleGridView, implementation::Scenario2_SimpleGridView>
    {
    };
}
