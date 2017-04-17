//
// DrillInPage.xaml.cpp
// Implementation of the DrillInPage class
//

#include "pch.h"
#include "BasicSubPage.xaml.h"
#include "DrillInPage.xaml.h"

using namespace NavigationMenuSample::Views;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

DrillInPage::DrillInPage()
{
    InitializeComponent();
}

void DrillInPage::ListView_ItemClick(Platform::Object^ sender, ItemClickEventArgs^ e)
{
    Frame->Navigate(
        TypeName(BasicSubPage::typeid),
        e->ClickedItem,
        ref new Windows::UI::Xaml::Media::Animation::DrillInNavigationTransitionInfo());
}
