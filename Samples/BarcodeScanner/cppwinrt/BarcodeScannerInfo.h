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

#include "BarcodeScannerInfo.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct BarcodeScannerInfo : BarcodeScannerInfoT<BarcodeScannerInfo>
    {
        BarcodeScannerInfo(winrt::hstring const& deviceName, winrt::hstring const& deviceId) :
            m_deviceName(deviceName),
            m_deviceId(deviceId)
        {
        }

        winrt::hstring Name() const
        {
            return m_deviceName;
        }

        winrt::hstring DeviceId() const
        {
            return m_deviceId;
        }

    private:
        winrt::hstring m_deviceName;
        winrt::hstring m_deviceId;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct BarcodeScannerInfo : BarcodeScannerInfoT<BarcodeScannerInfo, implementation::BarcodeScannerInfo>
    {
    };
}