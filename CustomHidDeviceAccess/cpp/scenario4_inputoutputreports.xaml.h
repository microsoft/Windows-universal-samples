//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario4_InputOutputReports.xaml.h
// Declaration of the Scenario4_InputOutputReports class
//

#pragma once
#include "Scenario4_InputOutputReports.g.h"

namespace CustomHidDeviceAccess
{
	/// <summary>
	/// This scenario demonstrates how to get input reports and how to send output reports
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class InputOutputReports sealed
	{
	public:
		InputOutputReports();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		void GetNumericInputReport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs);
		void SendNumericOutputReport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs);
		void GetBooleanInputReport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs);
		void SendBooleanOutputReport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs);

		Concurrency::task<void> GetNumericInputReportAsync(void);
		Concurrency::task<void> SendNumericOutputReportAsync(uint8 valueToWrite);
		Concurrency::task<void> GetBooleanInputReportAsync(uint8 buttonNumber);
		Concurrency::task<void> SendBooleanOutputReportAsync(bool bitValue, uint8 buttonNumber);
	};
}