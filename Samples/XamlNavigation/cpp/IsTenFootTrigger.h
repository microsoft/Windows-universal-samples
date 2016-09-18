#pragma once

namespace NavigationMenuSample {
    namespace Triggers {

        public ref class IsTenFootTrigger sealed :
            public Windows::UI::Xaml::StateTriggerBase
        {
        public:
            IsTenFootTrigger()
            {
                // Set default values.
                IsTenFoot = true;
            }
            property bool IsTenFoot
            {
                bool get() { return _isTenFootRequested; }
                void set(bool value);
            }
        private:
            bool _isTenFootRequested;
        };
    }
}

