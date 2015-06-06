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

using SDKTemplate;

namespace CustomSerialDeviceAccess
{
    /// <summary>
    /// This class is intended to host any functionality that will be shared among different
    /// scenario/pages such as common error messages.
    /// </summary>
    public class Utilities
    {
        /// <summary>
        /// Prints an error message stating that device is not connected
        /// </summary>
        public static void NotifyDeviceNotConnected()
        {
            MainPage.Current.NotifyUser("Device is not connected, please select a plugged in device to try the scenario again", NotifyType.ErrorMessage);
        }
    }
}
