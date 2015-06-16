//
// EventTests.xaml.cpp
// Implementation of the EventTests class
//

#include "pch.h"
#include "EventTests.xaml.h"

using namespace xBindSampleCX;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Popups;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

EventTests::EventTests()
{
	InitializeComponent();
    this->Model = ref new xBindSampleModel::DataModel();
}


void EventTests::Click_RegularArgs(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    MessageDialog^ dlg = ref new MessageDialog("Clicked - Regular Args");
    dlg->ShowAsync();
}

void EventTests::Click_NoArgs()
{
    MessageDialog^ dlg = ref new MessageDialog("Clicked - No Args");
    dlg->ShowAsync();
}

void EventTests::Click_BaseArgs(Platform::Object^ sender, Platform::Object^ e)
{
    MessageDialog^ dlg = ref new MessageDialog("Clicked - Base Args");
    dlg->ShowAsync();
}