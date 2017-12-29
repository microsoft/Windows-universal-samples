//
// SecondaryPage.xaml.cpp
// Implementation of the SecondaryPage class
//

#include "pch.h"
#include "SecondaryPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

SecondaryPage::SecondaryPage()
{
    InitializeComponent();
}


void SecondaryPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    bool optedIn = static_cast<bool>(e->Parameter);


    auto rootFrame = dynamic_cast<Windows::UI::Xaml::Controls::Frame^>(Window::Current->Content);

    if (rootFrame->CanGoBack && optedIn)
    {
        // If we have pages in our in-app backstack and have opted in to showing back, do so
        SystemNavigationManager::GetForCurrentView()->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
    }
    else
    {
        // Remove the UI from the title bar if there are no pages in our in-app back stack
        SystemNavigationManager::GetForCurrentView()->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Collapsed;
    }
}
