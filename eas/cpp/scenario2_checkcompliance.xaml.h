// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_CheckCompliance.g.h"
#include "MainPage.xaml.h"

namespace EAS
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario2_CheckCompliance sealed
	{
	public:
		Scenario2_CheckCompliance();
	private:
		SDKTemplate::MainPage^ rootPage;

		void Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
