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

#include "ShowPackageFamilyName.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct ShowPackageFamilyName : ShowPackageFamilyNameT<ShowPackageFamilyName>
    {
        ShowPackageFamilyName();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct ShowPackageFamilyName : ShowPackageFamilyNameT<ShowPackageFamilyName, implementation::ShowPackageFamilyName>
    {
    };
}
