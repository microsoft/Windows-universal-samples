//
// OtherBindings.xaml.cpp
// Implementation of the OtherBindings class
//

#include "pch.h"
#include "OtherBindings.xaml.h"

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
using namespace Windows::UI::Xaml::Interop;

using namespace xBindSampleModel;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

OtherBindings::OtherBindings()
{   
    Model = ref new DataModel();
	InitializeComponent();
    InitializeValues();
}

DependencyProperty^ OtherBindings::_DPOnPage = DependencyProperty::Register("DPOnPage", TypeName(::Platform::String::typeid), TypeName(OtherBindings::typeid), nullptr);


void OtherBindings::InitializeValues()
{
    this->DPOnPage = "DP on page";
}

void OtherBindings::UpdateValuesClick(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->Model->UpdateValues();
    this->DPOnPage += "-";
    this->MyInt += 1;
}

void OtherBindings::ResetValuesClick(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->Model->InitializeValues();
    this->InitializeValues();
    this->MyInt = 0;
}

void OtherBindings::UndeferElementClick(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->FindName("deferedTextBlock");
}