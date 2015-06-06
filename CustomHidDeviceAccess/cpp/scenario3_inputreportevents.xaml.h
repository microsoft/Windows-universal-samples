//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario3.xaml.h
// Declaration of the Scenario3 class
//

#pragma once
#include "EventHandlerForDevice.h"
#include "Scenario3_InputReportEvents.g.h"

namespace CustomHidDeviceAccess
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class InputReportEvents sealed
	{
	public:
		InputReportEvents();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		Windows::Foundation::EventRegistrationToken inputReportEventToken;
		bool isRegisteredForInputReportEvents;

		// Device that we registered for events with
		Windows::Devices::HumanInterfaceDevice::HidDevice^ registeredDevice;

		uint32 numInputReportEventsReceived;
		uint32 totalNumberBytesReceived;

		// Did we navigate away from this page?
		bool navigatedAway;

		void OnDeviceClosing(EventHandlerForDevice^ sender, Windows::Devices::Enumeration::DeviceInformation^ deviceInformation);

		void RegisterForInputReportEvents_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs);
		void UnregisterFromInputReportEvents_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs);

		void RegisterForInputReportEvents();

		void UnregisterFromInputReportEvent(void);

		void OnInputReportEvent(
			Windows::Devices::HumanInterfaceDevice::HidDevice^ sender,
			Windows::Devices::HumanInterfaceDevice::HidInputReportReceivedEventArgs^ eventArgs);

		void StartSuperMuttInputReports(void);
		void StopSuperMuttInputReports(void);

		void UpdateRegisterEventButtons(void);
	};
}
