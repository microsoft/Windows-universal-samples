//
// PageHeader.xaml.h
// Declaration of the PageHeader class
//

#pragma once

#include "PageHeader.g.h"
#include "AppShell.xaml.h"

using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace NavigationMenuSample
{
    namespace Controls
    {
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class PageHeader sealed
        {
        public:
            PageHeader();
            void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
            void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);

            static property DependencyProperty^ HeaderContentProperty
            {
                DependencyProperty^ get() { return _headerContentProperty; }
            }

            property UIElement^ HeaderContent
            {
                UIElement^ get() { return (UIElement^)GetValue(_headerContentProperty); }
                void set(UIElement^ value) { SetValue(_headerContentProperty, value); }
            }

        private:
            static DependencyProperty^ _headerContentProperty;
			Windows::Foundation::EventRegistrationToken _toggleButtonRectChangedToken;
            void Current_TogglePaneButtonSizeChanged(AppShell^ sender, Rect e);
        };
    }
}
