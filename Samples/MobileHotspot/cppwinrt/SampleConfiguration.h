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
    Windows::Networking::NetworkOperators::NetworkOperatorTetheringManager TryGetCurrentNetworkOperatorTetheringManager();
    hstring GetFriendlyName(Windows::Networking::NetworkOperators::TetheringWiFiBand value);
    bool IsBandSupported(Windows::Networking::NetworkOperators::NetworkOperatorTetheringAccessPointConfiguration const& configuration, Windows::Networking::NetworkOperators::TetheringWiFiBand band);
}
