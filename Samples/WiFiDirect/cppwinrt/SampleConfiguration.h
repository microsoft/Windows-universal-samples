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
    // Helper function to take the selected item
    // and return its tag, which is assumed to be of type T.
    template<typename T, typename Control>
    T GetSelectedItemTag(Control&& control)
    {
        return unbox_value<T>(control.SelectedItem().as<Windows::UI::Xaml::FrameworkElement>().Tag());
    }

    // Helper function to remove an item from a collection by value
    // rather than by index.
    template<typename T, typename V>
    bool RemoveByValue(T&& collection, V&& value)
    {
        uint32_t index;
        bool found = collection.IndexOf(std::forward<V>(value), index);
        if (found)
        {
            collection.RemoveAt(index);
        }
        return found;
    }


    fire_and_forget HandlePairing(Windows::UI::Core::CoreDispatcher dispatcher, Windows::Devices::Enumeration::DevicePairingRequestedEventArgs e);

    static constexpr std::array<uint8_t, 3> CustomOui{ 0xAA, 0xBB, 0xCC };
    static constexpr uint8_t CustomOuiType{ 0xDD };
    static constexpr std::array<uint8_t, 3> WfaOui{ 0x50, 0x6F, 0x9A };
    static constexpr std::array<uint8_t, 3> MsftOui{ 0x00, 0x50, 0xF2 };
    static constexpr wchar_t ServerPortString[] = L"50001";
}

namespace winrt
{
    hstring to_hstring(Windows::Devices::Enumeration::DevicePairingResultStatus);
    hstring to_hstring(Windows::Devices::Enumeration::DeviceUnpairingResultStatus);
    hstring to_hstring(Windows::Devices::WiFiDirect::WiFiDirectAdvertisementPublisherStatus);
    hstring to_hstring(Windows::Devices::WiFiDirect::WiFiDirectConfigurationMethod);
    hstring to_hstring(Windows::Devices::WiFiDirect::WiFiDirectConnectionStatus);
    hstring to_hstring(Windows::Devices::WiFiDirect::WiFiDirectError);
}
