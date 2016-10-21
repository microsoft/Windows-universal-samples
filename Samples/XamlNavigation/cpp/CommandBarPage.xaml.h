//
// CommandBarPage.xaml.h
// Declaration of the CommandBarPage class
//

#pragma once

#include "CommandBarPage.g.h"

namespace NavigationMenuSample
{
    namespace Views
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class CommandBarPage sealed
        {
        public:
            CommandBarPage();
            void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
        };
    }
}
