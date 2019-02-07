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
    struct Constants
    {
        static constexpr guid CalcServiceUuid{ 0xcaecface, 0xe1d9, 0x11e6, { 0xbf, 0x01, 0xfe, 0x55, 0x13, 0x50, 0x34, 0xf0 } }; // {caecface-e1d9-11e6-bf01-fe55135034f0}
        static constexpr guid Op1CharacteristicUuid{ 0xcaecface, 0xe1d9, 0x11e6, { 0xbf, 0x01, 0xfe, 0x55, 0x13, 0x50, 0x34, 0xf1 } }; // {caecface-e1d9-11e6-bf01-fe55135034f1}
        static constexpr guid Op2CharacteristicUuid{ 0xcaecface, 0xe1d9, 0x11e6, { 0xbf, 0x01, 0xfe, 0x55, 0x13, 0x50, 0x34, 0xf2 } }; // {caecface-e1d9-11e6-bf01-fe55135034f2}
        static constexpr guid OperatorCharacteristicUuid{ 0xcaecface, 0xe1d9, 0x11e6, { 0xbf, 0x01, 0xfe, 0x55, 0x13, 0x50, 0x34, 0xf3 } }; // {caecface-e1d9-11e6-bf01-fe55135034f3}
        static constexpr guid ResultCharacteristicUuid{ 0xcaecface, 0xe1d9, 0x11e6, { 0xbf, 0x01, 0xfe, 0x55, 0x13, 0x50, 0x34, 0xf4 } }; // {caecface-e1d9-11e6-bf01-fe55135034f4}
    };

    struct SampleState
    {
        static hstring SelectedBleDeviceId;
        static hstring SelectedBleDeviceName;
    };

    Windows::Foundation::Rect GetElementRect(Windows::UI::Xaml::FrameworkElement const& element);
}
