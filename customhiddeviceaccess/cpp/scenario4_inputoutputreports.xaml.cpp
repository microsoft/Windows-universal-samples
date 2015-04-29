//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario4_InputOutputReports.xaml.cpp
// Implementation of the Scenario4_InputOutputReports class
//

#include "pch.h"
#include "Utilities.h"
#include "EventHandlerForDevice.h"
#include "Scenario4_InputOutputReports.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace CustomHidDeviceAccess;

using namespace Platform;
using namespace Platform::Collections;
using namespace Concurrency;
using namespace Windows::Devices::HumanInterfaceDevice;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Controls;

InputOutputReports::InputOutputReports()
{
	InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
///
/// We will enable/disable parts of the UI if the device doesn't support it.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void InputOutputReports::OnNavigatedTo(NavigationEventArgs^ /* e */)
{
	// We will disable the scenario that is not supported by the device.
	// If no devices are connected, none of the scenarios will be shown and an error will be displayed
	Platform::Collections::Map<DeviceType, UIElement^>^ deviceScenarios = ref new Platform::Collections::Map<DeviceType, UIElement^>();
	deviceScenarios->Insert(DeviceType::SuperMutt, SuperMuttScenario);

	Utilities::SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);
}


void InputOutputReports::GetNumericInputReport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs)
{
	if (EventHandlerForDevice::Current->IsDeviceConnected)
	{
		ButtonGetNumericInputReport->IsEnabled = false;

		// Re-enable get button after completing the set regardless of there was an error or not
		GetNumericInputReportAsync().then([this](task<void> getInputReportTask)
		{
			ButtonGetNumericInputReport->IsEnabled = true;

			getInputReportTask.get();   // May throw exception if there was an error reading
		});
	}
	else
	{
		Utilities::NotifyDeviceNotConnected();
	}
}

void InputOutputReports::SendNumericOutputReport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs)
{
	if (EventHandlerForDevice::Current->IsDeviceConnected)
	{
		ButtonSendNumericOutputReport->IsEnabled = false;

		uint8 pattern = static_cast<uint8>(NumericValueToWrite->SelectedIndex);

		// Re-enable set button after completing the set regardless of there was an error or not
		SendNumericOutputReportAsync(pattern).then([this](task<void> sendOutputReportTask)
		{
			ButtonSendNumericOutputReport->IsEnabled = true;

			sendOutputReportTask.get();   // May throw exception if there was an error reading
		});
	}
	else
	{
		Utilities::NotifyDeviceNotConnected();
	}
}

void InputOutputReports::GetBooleanInputReport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs)
{
	if (EventHandlerForDevice::Current->IsDeviceConnected)
	{
		ButtonGetBooleanInputReport->IsEnabled = false;

		// Re-enable get button after completing the set regardless of there was an error or not
		// This sample will only demonstrate the first button
		GetBooleanInputReportAsync(1).then([this](task<void> getInputReportTask)
		{
			ButtonGetBooleanInputReport->IsEnabled = true;

			getInputReportTask.get();   // May throw exception if there was an error reading
		});
	}
	else
	{
		Utilities::NotifyDeviceNotConnected();
	}
}

void InputOutputReports::SendBooleanOutputReport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs)
{
	if (EventHandlerForDevice::Current->IsDeviceConnected)
	{
		ButtonSendBooleanOutputReport->IsEnabled = false;

		uint8 pattern = static_cast<uint8>(BooleanValueToWrite->SelectedIndex);

		// Re-enable set button after completing the set regardless of there was an error or not
		// The sample will only demonstrate writing to the first button because writing to all other buttons are the same
		SendBooleanOutputReportAsync(pattern == 1 ? true : false, 1).then([this](task<void> sendOutputReportTask)
		{
			ButtonSendBooleanOutputReport->IsEnabled = true;

			sendOutputReportTask.get();   // May throw exception if there was an error reading
		});
	}
	else
	{
		Utilities::NotifyDeviceNotConnected();
	}
}

/// <summary>
/// Uses an input report to get the value that was previously written to the device with output report.
/// 
/// We will be using controls to modify the data buffer. See SendNumericOutputReportAsync for how to modify the data buffer directly.
///
/// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
/// at the end of the task chain.
/// </summary>
/// <returns>A task that can be used to chain more methods after completing the scenario</returns>
task<void> InputOutputReports::GetNumericInputReportAsync(void)
{
	return create_task(EventHandlerForDevice::Current->Device->GetInputReportAsync(SuperMutt::ReadWriteBufferControlInformation::ReportId))
		.then([](task<HidInputReport^> inputReportTask)
	{
		auto inputReport = inputReportTask.get(); // Pass down the error if one occurred
		auto inputReportControl = inputReport->GetNumericControl(SuperMutt::ReadWriteBufferControlInformation::VolumeUsagePage, SuperMutt::ReadWriteBufferControlInformation::VolumeUsageId);

		auto data = inputReportControl->Value;

		MainPage::Current->NotifyUser("Value read: " + data, NotifyType::StatusMessage);
	});
}

/// <summary>
/// Uses an output report to write data to the device so that it can be read back using input report. 
///
/// Please note that when we create an OutputReport, all data is nulled out in the report. Since we will only be modifying 
/// data we care about, the other bits that we don't care about, will be zeroed out. Controls will effectively do the same thing (
/// all bits are zeroed out except for the bits we care about).
/// 
/// We will modify the data buffer directly. See GetNumericInputReportAsync for how to use controls.
///
/// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
/// at the end of the task chain.
/// </summary>
/// <param name="valueToWrite"></param>
/// <returns>A task that can be used to chain more methods after completing the scenario</returns>
task<void> InputOutputReports::SendNumericOutputReportAsync(uint8 valueToWrite)
{
	auto outputReport = EventHandlerForDevice::Current->Device->CreateOutputReport(SuperMutt::ReadWriteBufferControlInformation::ReportId);

	// Our data is supposed to be stored in the upper 4 bits of the byte
	uint8 byteToWrite = (uint8)(valueToWrite << SuperMutt::ReadWriteBufferControlInformation::VolumeDataStartBit);

	auto dataWriter = ref new DataWriter();

	// Report Id is always the first byte
	dataWriter->WriteByte((uint8)outputReport->Id);
	dataWriter->WriteByte(byteToWrite);

	outputReport->Data = dataWriter->DetachBuffer();

	return create_task(EventHandlerForDevice::Current->Device->SendOutputReportAsync(outputReport))
		.then([valueToWrite](task<uint32> bytesWrittenTask)
	{
		// bytesWrittenTask.get() either returns the number of bytes written or throws an exception
		// if SendOutputReportAsync() failed.
		bytesWrittenTask.get();

		MainPage::Current->NotifyUser("Value written: " + valueToWrite.ToString(), NotifyType::StatusMessage);
	});
}

/// <summary>
/// Uses an input report to get the value that was previously written to the device with output report.
///
/// We will be using controls to modify the data buffer. See SendBooleanOutputReportAsync for how to modify the data buffer directly.
///
/// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
/// at the end of the task chain.
/// </summary>
/// <param name="buttonNumber">Button to get the value of (1-4)</param>
/// <returns>A task that can be used to chain more methods after completing the scenario</returns>
task<void> InputOutputReports::GetBooleanInputReportAsync(uint8 buttonNumber)
{
	return create_task(EventHandlerForDevice::Current->Device->GetInputReportAsync(SuperMutt::ReadWriteBufferControlInformation::ReportId))
		.then([buttonNumber](task<HidInputReport^> inputReportTask)
	{
		auto inputReport = inputReportTask.get(); // Pass down the error if one occurred
		auto inputReportControl = inputReport->GetBooleanControl(
			SuperMutt::ReadWriteBufferControlInformation::ButtonUsagePage,
			SuperMutt::ReadWriteBufferControlInformation::ButtonUsageId[buttonNumber - 1]);

		String^ data = inputReportControl->IsActive ? "1" : "0";

		MainPage::Current->NotifyUser("Bit value: " + data, NotifyType::StatusMessage);
	});
}

/// <summary>
/// Uses an output report to write data to the device so that it can be read back using input report.
///
/// Please note that when we create an OutputReport, all data is nulled out in the report. Since we will only be modifying 
/// data we care about, the other bits that we don't care about, will be zeroed out. Controls will effectively do the same thing (
/// all bits are zeroed out except for the bits we care about).
///
/// We will modify the data buffer directly. See GetBooleanInputReportAsync for how to use controls.
///
/// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
/// at the end of the task chain.
/// </summary>
/// <param name="bitValue"></param>
/// <param name="buttonNumber">Button to change value of (1-4)</param>
/// <returns>A task that can be used to chain more methods after completing the scenario</returns>
task<void> InputOutputReports::SendBooleanOutputReportAsync(bool bitValue, uint8 buttonNumber)
{
	auto buttonIndex = buttonNumber - 1;    // Make 0 based index

	auto outputReport = EventHandlerForDevice::Current->Device->CreateOutputReport(SuperMutt::ReadWriteBufferControlInformation::ReportId);

	uint8 bitNumericValue = (uint8)(bitValue ? 0xFF : 0x00);

	// Clear our button's data
	uint8 valueAfterSettingBit = (uint8)(bitNumericValue & SuperMutt::ReadWriteBufferControlInformation::ButtonDataMask[buttonIndex]);

	auto dataWriter = ref new DataWriter();

	// Report Id is always the first byte
	dataWriter->WriteByte((uint8)outputReport->Id);
	dataWriter->WriteByte(valueAfterSettingBit);

	outputReport->Data = dataWriter->DetachBuffer();

	return create_task(EventHandlerForDevice::Current->Device->SendOutputReportAsync(outputReport))
		.then([bitValue](task<uint32> bytesWrittenTask)
	{
		// bytesWrittenTask.get() either returns the number of bytes written or throws an exception
		// if SendOutputReportAsync() failed.
		bytesWrittenTask.get();

		String^ valueWritten = bitValue ? "1" : "0";
		MainPage::Current->NotifyUser("Bit value written: " + valueWritten, NotifyType::StatusMessage);
	});
}
