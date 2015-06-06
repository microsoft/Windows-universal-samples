//
// ListsAndTemplates.xaml.h
// Declaration of the ListsAndTemplates class
//

#pragma once

#include "ListsAndTemplates.g.h"

namespace xBindSampleCX
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ListsAndTemplates sealed
	{
	public:
		ListsAndTemplates();

        property Windows::Foundation::Collections::IObservableVector<xBindSampleModel::IEmployee^>^ Employees;
        property xBindSampleModel::DataModel^ Model;

    private:
        void InitializeValues();
        void UpdateValuesClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetValuesClick(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^ e);



	};
}
