//
// ListsAndTemplates.xaml.cpp
// Implementation of the ListsAndTemplates class
//

#include "pch.h"
#include "ListsAndTemplates.xaml.h"

using namespace SDKTemplate;

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

ListsAndTemplates::ListsAndTemplates()
{
    this->Model = ref new DataModel();
    this->Employees = ref new Platform::Collections::Vector<IEmployee^>();
    this->InitializeComponent();
    this->InitializeValues();
}

void ListsAndTemplates::InitializeValues()
{
    this->Model->InitializeValues();
    this->Employees->Clear();
    for (IEmployee^ e : Model->ManagerProp->ReportsList)
    {   
        this->Employees->Append(e);
    }
}

void ListsAndTemplates::UpdateValuesClick(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->Model->UpdateValues();
    IEmployee^ temp = Employees->GetAt(0);
    Employees->RemoveAt(0);
    Employees->Append(temp);
}

void ListsAndTemplates::ResetValuesClick(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->InitializeValues();
}