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

using System;
using Windows.Devices.Custom;

namespace SDKTemplate
{
    static class Fx2Driver
    {
        public const ushort DeviceType = 65500;
        public const ushort FunctionBase = 0x800;

        public static
        IOControlCode GetSevenSegmentDisplay = new IOControlCode(DeviceType,
                                                                 FunctionBase + 7,
                                                                 IOControlAccessMode.Read,
                                                                 IOControlBufferingMethod.Buffered);

        public static
        IOControlCode SetSevenSegmentDisplay = new IOControlCode(DeviceType,
                                                                 FunctionBase + 8,
                                                                 IOControlAccessMode.Write,
                                                                 IOControlBufferingMethod.Buffered);

        public static
        IOControlCode ReadSwitches = new IOControlCode(DeviceType,
                                                       FunctionBase + 6,
                                                       IOControlAccessMode.Read,
                                                       IOControlBufferingMethod.Buffered);

        public static
        IOControlCode GetInterruptMessage = new IOControlCode(DeviceType,
                                                              FunctionBase + 9,
                                                              IOControlAccessMode.Read,
                                                              IOControlBufferingMethod.DirectOutput);

        static readonly byte[] SevenSegmentValues = {
                                            0xD7, // 0
                                            0x06, // 1
                                            0xB3, // 2
                                            0xA7, // 3
                                            0x66, // 4
                                            0xE5, // 5
                                            0xF4, // 6
                                            0x07, // 7
                                            0xF7, // 8
                                            0x67, // 9
                                          };

        public static
        byte DigitToSevenSegment(byte value)
        {
            return SevenSegmentValues[value];
        }

        public static
        byte SevenSegmentToDigit(byte value)
        {
            for (byte i = 0; i < SevenSegmentValues.Length; i += 1)
            {
                if (SevenSegmentValues[i] == value)
                {
                    return i;
                }
            }
            return 0xff;
        }

        public static readonly Guid DeviceInterfaceGuid = new Guid("573E8C73-0CB4-4471-A1BF-FAB26C31D384");
    }
}
