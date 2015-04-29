// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3_ApplyEASPolicy.g.h"
#include "MainPage.xaml.h"

namespace EAS
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario3_ApplyEASPolicy sealed
	{
	public:
		Scenario3_ApplyEASPolicy();
	private:
		SDKTemplate::MainPage^ rootPage;

		void Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
