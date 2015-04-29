// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario4_ChangeAdminKey.g.h"
#include "MainPage.xaml.h"

namespace Smartcard
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario4_ChangeAdminKey sealed
	{
	public:
		Scenario4_ChangeAdminKey();
	private:
		void ChangeAdminKey_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}