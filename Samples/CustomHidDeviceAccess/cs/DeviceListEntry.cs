//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;

namespace CustomHidDeviceAccess
{
    /// <summary>
    /// The class will only expose properties from DeviceInformation that are going to be used
    /// in this sample. Each instance of this class provides information about a single device.
    ///
    /// This class is used by the UI to display device specific information so that
    /// the user can identify which device to use.
    /// </summary>
    public class DeviceListEntry
    {
        private DeviceInformation device;
        private String deviceSelector;

        public String InstanceId
        {
            get
            {
                return (String)device.Properties[DeviceProperties.DeviceInstanceId];
            }
        }

        public DeviceInformation DeviceInformation
        {
            get
            {
                return device;
            }
        }

        public String DeviceSelector
        {
            get
            {
                return deviceSelector;
            }
        }

        /// <summary>
        /// The class is mainly used as a DeviceInformation wrapper so that the UI can bind to a list of these.
        /// </summary>
        /// <param name="deviceInformation"></param>
        /// <param name="deviceSelector">The AQS used to find this device</param>
        public DeviceListEntry(Windows.Devices.Enumeration.DeviceInformation deviceInformation, String deviceSelector)
        {
            device = deviceInformation;
            this.deviceSelector = deviceSelector;
        }

    }
}
