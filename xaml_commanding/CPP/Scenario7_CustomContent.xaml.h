//
// Scenario7_CustomContent.xaml.h
// Declaration of the Scenario7_CustomContent class
//

#pragma once

#include "Scenario7_CustomContent.g.h"

namespace Commanding
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario7_CustomContent sealed
	{
	public:
		Scenario7_CustomContent();
	private:
		void CommandBar_Opening(Platform::Object^ sender, Platform::Object^ e);
		void CommandBar_Closed(Platform::Object^ sender, Platform::Object^ e);
	};
}
