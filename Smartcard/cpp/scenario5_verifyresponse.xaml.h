// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario5_VerifyResponse.g.h"
#include "MainPage.xaml.h"

namespace Smartcard
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario5_VerifyResponse sealed
	{
	public:
		Scenario5_VerifyResponse();
	private:
		void VerifyResponse_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}