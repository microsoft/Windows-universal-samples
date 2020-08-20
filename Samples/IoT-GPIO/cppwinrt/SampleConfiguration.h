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
    inline hstring PinValueToString(Windows::Devices::Gpio::GpioPinValue value)
    {
        switch (value)
        {
        case Windows::Devices::Gpio::GpioPinValue::Low: return L"Low";
        case Windows::Devices::Gpio::GpioPinValue::High: return L"High";
        default: return to_hstring(static_cast<int32_t>(value));
        }
    }
}
