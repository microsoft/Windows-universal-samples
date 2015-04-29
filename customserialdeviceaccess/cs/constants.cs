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

using CustomSerialDeviceAccess;
using Windows.UI.Xaml.Controls;

namespace CustomSerialDeviceAccess
{
    public class DeviceProperties
    {
        public const String DeviceInstanceId = "System.Devices.DeviceInstanceId";
    }

    public class DeviceConfiguration
    {
        public const UInt32 BaudRate = 115200;

        public const Windows.Devices.SerialCommunication.SerialParity Parity = Windows.Devices.SerialCommunication.SerialParity.Even;

        public const Windows.Devices.SerialCommunication.SerialStopBitCount StopBits = Windows.Devices.SerialCommunication.SerialStopBitCount.OnePointFive;

        public const UInt16 DataBits = 8;

        public const Windows.Devices.SerialCommunication.SerialHandshake Handshake = Windows.Devices.SerialCommunication.SerialHandshake.RequestToSend;

        public const Boolean BreakSignalState_false = false;
        public const Boolean BreakSignalState_true = true;

        public const Boolean IsDataTerminalReady_false = false;
        public const Boolean IsDataTerminalReady_true = true;

        public const Boolean IsRequestToSendEnabled_false = false;
        public const Boolean IsRequestToSendEnabled_true = true;

    }
    
}
