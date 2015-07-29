//
// BasicSubPage.xaml.h
// Declaration of the BasicSubPage class
//

#pragma once

using namespace Platform;

#include "BasicSubPage.g.h"

namespace NavigationMenuSample
{
    namespace Views
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class BasicSubPage sealed
        {
        public:
            BasicSubPage();
            property String^ Message
            {
                String^ get();
                void set(String^);
            }

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        private:
            String^ _string;
        };
    }
}
