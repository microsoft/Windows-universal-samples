//
// DrillInPage.xaml.h
// Declaration of the DrillInPage class
//

#pragma once

using namespace Platform;

#include "DrillInPage.g.h"
#include "AppShell.xaml.h"

namespace NavigationMenuSample
{
    namespace Views
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class DrillInPage sealed
        {
        public:
            DrillInPage();
        private:
            void ListView_ItemClick(Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
        };
    }
}
