//
// EventTests.xaml.h
// Declaration of the EventTests class
//

#pragma once

#include "EventTests.g.h"

namespace xBindSampleCX
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class EventTests sealed
	{
	public:
		EventTests();

        property xBindSampleModel::DataModel^ Model;

        void Click_RegularArgs(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Click_NoArgs();
        void Click_BaseArgs(Platform::Object^ sender, Platform::Object^ e);
	};
}
