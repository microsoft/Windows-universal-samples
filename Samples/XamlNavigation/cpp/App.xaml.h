//
// App.xaml.h
// Declaration of the App class.
//

#pragma once

#include "App.g.h"

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml::Navigation;

namespace NavigationMenuSample
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    ref class App sealed
    {
    protected:
        virtual void OnLaunched(Activation::LaunchActivatedEventArgs^ e) override;

    internal:
        App();

    private:
        void OnSuspending(Object^ sender, SuspendingEventArgs^ e);
        void OnNavigationFailed(Object ^sender, NavigationFailedEventArgs ^e);
    };
}
