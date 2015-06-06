// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_ChangePin.g.h"
#include "MainPage.xaml.h"

namespace Smartcard
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario2_ChangePin sealed
	{
	public:
		Scenario2_ChangePin();
	private:
		void ChangePin_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}