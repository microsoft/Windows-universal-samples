// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3_ResetPin.g.h"
#include "MainPage.xaml.h"

namespace Smartcard
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario3_ResetPin sealed
	{
	public:
		Scenario3_ResetPin();
	private:
		void ResetPin_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}