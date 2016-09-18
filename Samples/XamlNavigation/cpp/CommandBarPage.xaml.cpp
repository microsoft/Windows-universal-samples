//
// CommandBarPage.xaml.cpp
// Implementation of the CommandBarPage class
//

#include "pch.h"
#include "CommandBarPage.xaml.h"
#include "PageHeader.xaml.h"

using namespace NavigationMenuSample::Views;

using namespace Platform;
using namespace Windows::Graphics::Display;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

CommandBarPage::CommandBarPage()
{
    InitializeComponent();
    Loaded += ref new RoutedEventHandler(this, &CommandBarPage::OnLoaded);
}


void CommandBarPage::OnLoaded(Object ^sender, RoutedEventArgs ^e)
{
    IBox<double>^ diagonal = (DisplayInformation::GetForCurrentView()->DiagonalSizeInInches);
    //move commandbar to page bottom on small screens
    if (diagonal && diagonal->Value < 7)
    {
        topbar->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        pageTitleContainer->Visibility = Windows::UI::Xaml::Visibility::Visible;
        bottombar->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
    else
    {
        topbar->Visibility = Windows::UI::Xaml::Visibility::Visible;
        pageTitleContainer->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        bottombar->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
}
