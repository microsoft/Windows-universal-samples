//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario3.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include "Utilities.h"
#include "Scenario3_InputReportEvents.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace CustomHidDeviceAccess;

using namespace Platform;
using namespace Platform::Collections;
using namespace Concurrency;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::HumanInterfaceDevice;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

InputReportEvents::InputReportEvents(void) :
	isRegisteredForInputReportEvents(false),
	numInputReportEventsReceived(0),
	navigatedAway(false),
	registeredDevice(nullptr)
{
	InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
///
/// We will enable/disable parts of the UI if the device doesn't support it.
/// 
/// The SuperMutt requires an output report to tell the device to start sending interrupts, so we will start it
/// just in case we register for the events. We want this to be transparent because it doesn't demonstrate how 
/// to register for events.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void InputReportEvents::OnNavigatedTo(NavigationEventArgs^ /* e */)
{
	navigatedAway = false;

	// We will disable the scenario that is not supported by the device.
	// If no devices are connected, none of the scenarios will be shown and an error will be displayed
	Platform::Collections::Map<DeviceType, UIElement^>^ deviceScenarios = ref new Platform::Collections::Map<DeviceType, UIElement^>();
	deviceScenarios->Insert(DeviceType::SuperMutt, SuperMuttScenario);

	Utilities::SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);

	// Get notified when the device is about to be closed so we can unregister eventd
	EventHandlerForDevice::Current->OnDeviceClose =
		ref new TypedEventHandler<EventHandlerForDevice^, DeviceInformation^>(this, &InputReportEvents::OnDeviceClosing);

	UpdateRegisterEventButtons();
}

/// <summary>
/// Reset the device into a known state by undoing everything we did to the device (unregister interrupts and
/// stop the device from creating interrupts)
/// </summary>
/// <param name="e"></param>
void InputReportEvents::OnNavigatedFrom(NavigationEventArgs^ /* e */)
{
	navigatedAway = true;

	if (EventHandlerForDevice::Current->IsDeviceConnected)
	{
		UnregisterFromInputReportEvent();

		// Stop interrupts after unregistering so that device interrupts do not effect the scenario (stopping before unregistering
		// will cause events to stop being raised, which will be noticable by the app; we want this to be transparent)
		StopSuperMuttInputReports();
	}

	// We are leaving and no longer care about the device closing
	EventHandlerForDevice::Current->OnDeviceClose = nullptr;
}

void InputReportEvents::OnDeviceClosing(EventHandlerForDevice^ /* sender */, DeviceInformation^ /* deviceInformation */)
{
	UnregisterFromInputReportEvent();

	MainPage::Current->Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		UpdateRegisterEventButtons();
	}));
}

void InputReportEvents::RegisterForInputReportEvents_Click(Object^ /* sender */, RoutedEventArgs^ /* eventArgs */)
{
	if (EventHandlerForDevice::Current->IsDeviceConnected)
	{
		// SuperMutt requires an output report to make the device send periodic input reports
		StartSuperMuttInputReports();

		RegisterForInputReportEvents();

		UpdateRegisterEventButtons();
	}
	else
	{
		Utilities::NotifyDeviceNotConnected();
	}
}

void InputReportEvents::UnregisterFromInputReportEvents_Click(Object^ sender, RoutedEventArgs^ eventArgs)
{
	if (EventHandlerForDevice::Current->IsDeviceConnected)
	{
		UnregisterFromInputReportEvent();

		StopSuperMuttInputReports();

		UpdateRegisterEventButtons();
	}
	else
	{
		Utilities::NotifyDeviceNotConnected();
	}
}

/// <summary>
/// Register for the event that is triggered when the device sends a report to us.
/// 
/// All input reports share the same event in HidDevice, so once we register the event, all HidInputReports (regardless of the report id) will
/// raise the event. Read the comment above OnInputReportEvent for more information on how to distinguish input reports.
///
/// The function also saves the event token so that we can unregister from the event later on.
/// </summary>
void InputReportEvents::RegisterForInputReportEvents()
{
	if (!isRegisteredForInputReportEvents)
	{
		// Remember which device we are registering the device with, in case there is a device disconnect and reconnect. We want to avoid unregistering
		// a stale token. Ideally, one should remove the event token (e.g. assign to null) upon the device removal to avoid using it again.
		registeredDevice = EventHandlerForDevice::Current->Device;

		// Save event registration token so we can unregisted for events
		inputReportEventToken = registeredDevice->InputReportReceived +=
			ref new TypedEventHandler<HidDevice^, HidInputReportReceivedEventArgs^>(this, &InputReportEvents::OnInputReportEvent);

		isRegisteredForInputReportEvents = true;

		numInputReportEventsReceived = 0;
		totalNumberBytesReceived = 0;
	}
}

/// <summary>
/// Unregisters from the HidDevice's InputReportReceived event that was registered for in the RegisterForInputReportEvents();
/// </summary>
void InputReportEvents::UnregisterFromInputReportEvent()
{
	if (isRegisteredForInputReportEvents)
	{
		// Don't unregister event token if the device was removed and reconnected because registration token is no longer valid
		if (registeredDevice == EventHandlerForDevice::Current->Device)
		{
			registeredDevice->InputReportReceived -= inputReportEventToken;
		}

		registeredDevice = nullptr;
		isRegisteredForInputReportEvents = false;
	}
}

/// <summary>
/// This callback only increments the total number of events received and prints it
///
/// This method is called whenever the device's state changes and sends a report. Since all input reports share the same event in 
/// HidDevice, the app needs to get the HidInputReport from eventArgs.Report and compare report ids and usages with the desired
/// report.
/// </summary>
/// <param name="sender">HidDevice that the event is being raised from</param> 
/// <param name="eventArgs">Contains the HidInputReport that caused the event to raise</param> 
void InputReportEvents::OnInputReportEvent(HidDevice^ sender, HidInputReportReceivedEventArgs^ eventArgs)
{
	// If we navigated away from this page, we don't need to process this event
	// This also prevents output from spilling into another page
	if (!navigatedAway)
	{
		numInputReportEventsReceived++;

		// The data from the InputReport
		HidInputReport^ inputReport = eventArgs->Report;
		IBuffer^ buffer = inputReport->Data;

		totalNumberBytesReceived += buffer->Length;

		// Create a DispatchedHandler for the because we are interracting with the UI directly and the
		// thread that this function is running on may not be the UI thread; if a non-UI thread modifies
		// the UI, an exception is thrown
		MainPage::Current->Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this, buffer]()
		{
			// If we navigated away from this page, do not print anything. The dispatch may be handled after
			// we move to a different page.
			if (!navigatedAway)
			{
				MainPage::Current->NotifyUser(
					"Total number of input report events received: " + numInputReportEventsReceived.ToString()
					+ "\nTotal number of bytes received: " + totalNumberBytesReceived.ToString() + " bytes",
					NotifyType::StatusMessage);
			}
		}));
	}
}

/// <summary>
/// Tells the SuperMutt device to start creating interrupts. The app won't get any interrupts until after we register for the event.
/// 
/// We are using an output report to start the interrupts, please go to Scenario4_InputOutputReports for more information on how to use
/// output reports.
/// </summary>
void InputReportEvents::StartSuperMuttInputReports(void)
{
	auto outputReport = EventHandlerForDevice::Current->Device->CreateOutputReport(SuperMutt::DeviceInputReportControlInformation::ReportId);

	auto dataWriter = ref new DataWriter();

	// Report Id is always the first byte
	dataWriter->WriteByte((uint8)outputReport->Id);
	dataWriter->WriteByte(SuperMutt::DeviceInputReportControlInformation::DataTurnOn);

	outputReport->Data = dataWriter->DetachBuffer();

	create_task(EventHandlerForDevice::Current->Device->SendOutputReportAsync(outputReport))
		.then([](task<uint32> bytesWrittenTask)
	{
		bytesWrittenTask.get();    // Make sure there was no error when turning on interrupts on the device
	});
}

/// <summary>
/// Tells the SuperMutt device to stop creating interrupts. The app will stop getting event raises unless we start the interrupts again.
/// 
/// We are using an output report to start the interrupts, please go to Scenario4_InputOutputReports for more information on how to use
/// output reports.
/// </summary>
void InputReportEvents::StopSuperMuttInputReports(void)
{
	auto outputReport = EventHandlerForDevice::Current->Device->CreateOutputReport(SuperMutt::DeviceInputReportControlInformation::ReportId);

	auto dataWriter = ref new DataWriter();

	// Report Id is always the first byte
	dataWriter->WriteByte((uint8)outputReport->Id);
	dataWriter->WriteByte(SuperMutt::DeviceInputReportControlInformation::DataTurnOff);

	outputReport->Data = dataWriter->DetachBuffer();

	create_task(EventHandlerForDevice::Current->Device->SendOutputReportAsync(outputReport))
		.then([](task<uint32> bytesWrittenTask)
	{
		bytesWrittenTask.get();
	});
}

void InputReportEvents::UpdateRegisterEventButtons(void)
{
	ButtonRegisterForInputReportEvents->IsEnabled = !isRegisteredForInputReportEvents;
	ButtonUnregisterFromInputReportEvents->IsEnabled = isRegisteredForInputReportEvents;
}

