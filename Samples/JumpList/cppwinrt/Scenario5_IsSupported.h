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

#include "Scenario5_IsSupported.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_IsSupported : Scenario5_IsSupportedT<Scenario5_IsSupported>
    {
        Scenario5_IsSupported();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_IsSupported : Scenario5_IsSupportedT<Scenario5_IsSupported, implementation::Scenario5_IsSupported>
    {
    };
}
