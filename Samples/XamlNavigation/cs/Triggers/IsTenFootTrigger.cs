using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.System.Profile;
using Windows.UI.Xaml;

namespace NavigationMenuSample.Triggers
{
    class IsTenFootTrigger : StateTriggerBase
    {
        private bool _isTenFootRequested;

        public IsTenFootTrigger()
        {
            // Set default values.
            IsTenFoot = true;
        }

        public bool IsTenFoot
        {
            get
            {
                return _isTenFootRequested;
            }
            set
            {
                _isTenFootRequested = value;
                SetActive(SystemInformationHelpers.IsTenFootExperience == _isTenFootRequested);
            }
        }
    }
}
