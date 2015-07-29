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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using SerialArduino;
using Windows.UI.Xaml.Controls;

namespace SerialArduino
{
    public class DeviceProperties
    {
        public const String DeviceInstanceId = "System.Devices.DeviceInstanceId";
    }

    public class ArduinoDevice
    {
        public const UInt16 Vid = 0x2341;
        public const UInt16 Pid = 0x0043;
    }

}
