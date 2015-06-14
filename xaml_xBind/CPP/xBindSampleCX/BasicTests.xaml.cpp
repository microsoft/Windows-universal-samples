//
// BasicTests.xaml.cpp
// Implementation of the BasicTests class
//

#include "pch.h"
#include "BasicTests.xaml.h"

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

DependencyProperty^ BasicTests::_DPOnPage = DependencyProperty::Register("DPOnPage", TypeName(::Platform::String::typeid), TypeName(BasicTests::typeid), nullptr);

BasicTests::BasicTests()
{
    Model = ref new DataModel();
    InitializeComponent();
    InitializeValues();
}

void BasicTests::InitializeValues()
{
    this->DPOnPage = "DP on page";
}

void BasicTests::UpdateValuesClick(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->Model->UpdateValues();
    this->DPOnPage += "-";
}

void BasicTests::ResetValuesClick(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->Model->InitializeValues();
    this->InitializeValues();

}
