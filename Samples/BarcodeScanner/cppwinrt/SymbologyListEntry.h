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

#include "SymbologyListEntry.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct SymbologyListEntry : SymbologyListEntryT<SymbologyListEntry>
    {
        SymbologyListEntry(uint32_t symbologyId, bool symbologyEnabled = true) :
            m_id(symbologyId),
            m_enabled(symbologyEnabled)
        {
        }

        uint32_t Id()
        {
            return m_id;
        }

        bool IsEnabled()
        {
            return m_enabled;
        }

        void IsEnabled(bool enabled)
        {
            m_enabled = enabled;
        }

        winrt::hstring Name()
        {
            return winrt::Windows::Devices::PointOfService::BarcodeSymbologies::GetName(m_id);
        }

    private:
        uint32_t m_id;
        bool m_enabled;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct SymbologyListEntry : SymbologyListEntryT<SymbologyListEntry, implementation::SymbologyListEntry>
    {
    };
}