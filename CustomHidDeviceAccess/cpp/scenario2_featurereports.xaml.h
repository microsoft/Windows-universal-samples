//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario2.xaml.h
// Declaration of the Scenario2 class
//

#pragma once
#include "Scenario2_FeatureReports.g.h"

namespace CustomHidDeviceAccess
{
	/// <summary>
	/// This page demonstrates how to use FeatureReports to communicate with the device
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class FeatureReports sealed
	{
	public:
		FeatureReports();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		void GetLedBlinkPattern_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SetLedBlinkPattern_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		Concurrency::task<void> SetLedBlinkPatternAsync(uint8 pattern);
		Concurrency::task<void> GetLedBlinkPatternAsync(void);
	};
}
