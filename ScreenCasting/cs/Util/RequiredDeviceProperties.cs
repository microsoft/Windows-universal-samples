using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScreenCasting.Util
{
    public class RequiredDeviceProperties
    {
        public static readonly string DEVPKEY_AepContainer_FriendlyName = "{0bba1ede-7566-4f47-90ec-25fc567ced2a}, 5";
        public static readonly string DEVPKEY_AepContainer_SupportsAudio = "{6af55d45-38db-4495-acb0-d4728a3b8314}, 2";
        public static readonly string DEVPKEY_AepContainer_SupportsVideo = "{6af55d45-38db-4495-acb0-d4728a3b8314}, 3";
        public static readonly string DEVPKEY_AepContainer_SupportsImages = "{6af55d45-38db-4495-acb0-d4728a3b8314}, 4";
        public static readonly string DEVPKEY_AepContainer_SupportedUriSchemes = "{6af55d45-38db-4495-acb0-d4728a3b8314}, 5";
        public static readonly string DEVPKEY_AepContainer_Categories = "System.Devices.AepContainer.Categories";
        public static readonly string DEVPKEY_DeviceContainer_Manufacturer = "System.Devices.Manufacturer";
        public static readonly string DEVPKEY_DeviceContainer_ModelName = "System.Devices.ModelName";
        public static readonly string DEVPKEY_Device_ContainerId = "System.Devices.ContainerId";
        public static readonly string DEVPKEY_Device_InstanceId = "System.Devices.DeviceInstanceId";

        private RequiredDeviceProperties() { }

        public static void AddProps(IList<string> list)
        {
            foreach (string s in Props)
                list.Add(s);
        }
        public static List<string> Props
        {
            get
            {
                List<string> properties = new List<string>();
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_FriendlyName);
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_SupportsAudio);
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_SupportsVideo);
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_SupportsImages);
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_SupportedUriSchemes);
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_Categories);
                properties.Add(RequiredDeviceProperties.DEVPKEY_DeviceContainer_Manufacturer);
                properties.Add(RequiredDeviceProperties.DEVPKEY_DeviceContainer_ModelName);
                properties.Add(RequiredDeviceProperties.DEVPKEY_Device_ContainerId);
                properties.Add(RequiredDeviceProperties.DEVPKEY_Device_InstanceId);

                return properties;
            }
        }
    }
}
