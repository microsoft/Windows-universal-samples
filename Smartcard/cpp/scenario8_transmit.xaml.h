// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario8_Transmit.g.h"
#include "MainPage.xaml.h"

namespace Smartcard
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario8_Transmit sealed
	{
	public:
		Scenario8_Transmit();
	private:
		void Transmit_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}