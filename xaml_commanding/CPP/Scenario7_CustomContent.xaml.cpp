//
// Scenario7_CustomContent.xaml.cpp
// Implementation of the Scenario7_CustomContent class
//

#include "pch.h"
#include "Scenario7_CustomContent.xaml.h"

using namespace Commanding;

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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario7_CustomContent::Scenario7_CustomContent()
{
	InitializeComponent();
}


void Scenario7_CustomContent::CommandBar_Opening(Platform::Object^ sender, Platform::Object^ e)
{
	appbarbutton->IsCompact = !appbarbutton->IsCompact;
}


void Scenario7_CustomContent::CommandBar_Closed(Platform::Object^ sender, Platform::Object^ e)
{
	appbarbutton->IsCompact = !appbarbutton->IsCompact;
}
