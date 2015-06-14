//
// MyUserControl1.xaml.cpp
// Implementation of the MyUserControl1 class
//

#include "pch.h"
#include "MyUserControl1.xaml.h"

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

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

MyUserControl1::MyUserControl1()
{
    this->_nonDPProperty = "Value not set";
    InitializeComponent();
}
