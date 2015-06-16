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
using Windows.Devices.Enumeration;

namespace ScreenCasting.Util
{
     public class CustomDevicePickerFilter
    {
        private IList<string> supportedDeviceSelectors = new List<string>();

        public IList<string> SupportedDeviceSelectors
        {
            get { return supportedDeviceSelectors; }
        }
        public override string ToString()
        {
            string retval = string.Empty;

            if (supportedDeviceSelectors.Count > 0)
                retval = supportedDeviceSelectors[0];

            if (supportedDeviceSelectors.Count > 1)
                retval = "(" + retval + ")";

            for (int idx = 1; idx < supportedDeviceSelectors.Count; idx++)
                retval = retval + " OR (" + supportedDeviceSelectors[idx] + ")";

            return retval;
        }
    }
}
