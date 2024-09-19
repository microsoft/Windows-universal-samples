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

namespace std
{
    std::wstring to_wstring(winrt::Windows::Devices::Lights::LampArrayKind lampArrayKind);
}

namespace winrt::SDKTemplate
{
    struct LampArrayInfo
    {
        LampArrayInfo(winrt::hstring const& id, winrt::hstring const& displayName, winrt::Windows::Devices::Lights::LampArray const& lampArray)
            : id(id), displayName(displayName), lampArray(lampArray)
        {
        }

        winrt::hstring const id;
        winrt::hstring const displayName;
        winrt::Windows::Devices::Lights::LampArray const lampArray;

        winrt::Windows::Devices::Lights::LampArray::AvailabilityChanged_revoker availabilityChangedRevoker;
    };
}
