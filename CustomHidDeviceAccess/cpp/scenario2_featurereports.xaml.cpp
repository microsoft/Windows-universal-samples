//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario2.xaml.cpp
// Implementation of the Scenario2 class
//

#include "pch.h"
#include "Utilities.h"
#include "EventHandlerForDevice.h"
#include "Scenario2_FeatureReports.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace CustomHidDeviceAccess;

using namespace Platform;
using namespace Platform::Collections;
using namespace Concurrency;
using namespace Windows::Devices::HumanInterfaceDevice;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

FeatureReports::FeatureReports()
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
void FeatureReports::OnNavigatedTo(NavigationEventArgs^ /* e */)
{
	// We will disable the scenario that is not supported by the device.
	// If no devices are connected, none of the scenarios will be shown and an error will be displayed
	Platform::Collections::Map<DeviceType, UIElement^>^ deviceScenarios = ref new Platform::Collections::Map<DeviceType, UIElement^>();
	deviceScenarios->Insert(DeviceType::SuperMutt, SuperMuttScenario);

	Utilities::SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);
}

void FeatureReports::GetLedBlinkPattern_Click(Platform::Object^ /* sender */, Windows::UI::Xaml::RoutedEventArgs^ /* e */)
{
	if (EventHandlerForDevice::Current->IsDeviceConnected)
	{
		ButtonGetLedBlinkPattern->IsEnabled = false;

		// Re-enable get button after completing the get regardless of there was an error or not
		GetLedBlinkPatternAsync().then([this](task<void> getLedTask)
		{
			ButtonGetLedBlinkPattern->IsEnabled = true;

			getLedTask.get();   // May throw exception if there was a problem getting Led pattern
		});
	}
	else
	{
		Utilities::NotifyDeviceNotConnected();
	}
}

void FeatureReports::SetLedBlinkPattern_Click(Platform::Object^ /* sender */, Windows::UI::Xaml::RoutedEventArgs^ /* e */)
{
	if (EventHandlerForDevice::Current->IsDeviceConnected)
	{
		ButtonSetLedBlinkPattern->IsEnabled = false;

		uint8 pattern = static_cast<uint8>(LedBlinkPatternInput->SelectedIndex);

		// Re-enable set button after completing the set regardless of there was an error or not
		SetLedBlinkPatternAsync(pattern).then([this](task<void> setLedTask)
		{
			ButtonSetLedBlinkPattern->IsEnabled = true;

			setLedTask.get();   // May throw exception if there was an error reading
		});
	}
	else
	{
		Utilities::NotifyDeviceNotConnected();
	}
}

/// <summary>
/// Uses feature report to get the LED blink pattern. 
/// 
/// Any errors in async function will be passed down the task chain and will not be caught here because errors should 
/// be handled at the end of the task chain.
/// 
/// The simplest way to obtain a byte from the buffer is by using a DataReader. DataReader provides a simple way
/// to read from buffers (e.g. can return bytes, strings, ints).
/// </summary>
/// <returns>A task that can be used to chain more methods after completing the scenario</returns>
task<void> FeatureReports::GetLedBlinkPatternAsync()
{
	return create_task(EventHandlerForDevice::Current->Device->GetFeatureReportAsync(SuperMutt::LedPattern::ReportId))
		.then([](task<HidFeatureReport^> featureReportTask)
	{
		HidFeatureReport^ featureReport = featureReportTask.get();    // Will throw an exception if there was an error getting the report

		if (featureReport->Data->Length == 2)
		{
			Array<uint8>^ reportBuffer = ref new Array<uint8>(2);
			auto dataReader = DataReader::FromBuffer(featureReport->Data);

			// Alternatively, you can call dataReader->ReadByte twice to get the second byte.
			dataReader->ReadBytes(reportBuffer);

			// Report Id is always the first byte 
			auto pattern = reportBuffer[1];

			MainPage::Current->NotifyUser("The Led blink pattern is " + pattern.ToString(), NotifyType::StatusMessage);
		}
		else
		{
			MainPage::Current->NotifyUser("Expecting 2 bytes, but received " + featureReport->Data->Length, NotifyType::ErrorMessage);
		}
	});
}

/// <summary>
/// Uses a feature report to set the blink pattern on the SuperMutt's LED. 
///
/// Please note that when we create a FeatureReport, all data is nulled out in the report. Since we will only be modifying 
/// data we care about, the other bits that we don't care about, will be zeroed out. Controls will effectively do the same thing (
/// all bits are zeroed out except for the bits we care about).
///
/// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
/// at the end of the task chain.
///
/// The SuperMutt has the following patterns:
/// 0 - LED always on
/// 1 - LED flash 2 seconds on, 2 off, repeat
/// 2 - LED flash 2 seconds on, 1 off, 2 on, 4 off, repeat
/// ...
/// 7 - 7 iterations of 2 on, 1 off, followed by 4 off, repeat
/// </summary>
/// <param name="pattern">A number from 0-7. Each number represents a different blinking pattern</param>
/// <returns>A task that can be used to chain more methods after completing the scenario</returns>
task<void> FeatureReports::SetLedBlinkPatternAsync(uint8 pattern)
{
	auto featureReport = EventHandlerForDevice::Current->Device->CreateFeatureReport(SuperMutt::LedPattern::ReportId);

	auto dataWriter = ref new DataWriter();

	// Report Id is always the first byte
	dataWriter->WriteByte((uint8)featureReport->Id);
	dataWriter->WriteByte(pattern);

	featureReport->Data = dataWriter->DetachBuffer();

	return create_task(EventHandlerForDevice::Current->Device->SendFeatureReportAsync(featureReport))
		.then([pattern](task<uint32> bytesWrittenTask)
	{
		bytesWrittenTask.get();    // If exception occured, let an exception flow down the task chain so it can be caught

		MainPage::Current->NotifyUser("The Led blink pattern is set to " + pattern.ToString(), NotifyType::StatusMessage);
	});
}
