using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;

namespace StateTriggersSample.CustomTriggers
{
    public class DeviceFamilyTrigger : StateTriggerBase
    {
        //private variables
        private string _currentDeviceFamily, _queriedDeviceFamily;
        //Public property
        public string DeviceFamily
        {
            get
            {
                return _queriedDeviceFamily;
            }
            set
            {
                _queriedDeviceFamily = value;
                //Get the current device family
                _currentDeviceFamily = Windows.System.Profile.AnalyticsInfo.VersionInfo.DeviceFamily;
                //The trigger will be activated if the current device family matches the device family value in XAML
                SetActive(_queriedDeviceFamily == _currentDeviceFamily);
            }
        }
    }
}
