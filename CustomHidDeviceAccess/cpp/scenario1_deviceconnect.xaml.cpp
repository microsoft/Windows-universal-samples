//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario1_DeviceConnect.xaml.cpp
// Implementation of the DeviceConnect class
//

#include "pch.h"
#include "Scenario1_DeviceConnect.xaml.h"
#include "EventHandlerForDevice.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Platform::Collections;
using namespace Concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::HumanInterfaceDevice;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace CustomHidDeviceAccess;

Platform::String^ DeviceConnect::ButtonNameDisconnectFromDevice = "Disconnect from device";
Platform::String^ DeviceConnect::ButtonNameDisableReconnectToDevice = "Do not automatically reconnect to device that was just closed";

DeviceConnect::DeviceConnect(void) :
	listOfDevices(ref new Vector<DeviceListEntry^>()),
	deviceWatchers(ref new Vector<DeviceWatcher^>()),
	watchersStarted(false),
	watchersSuspended(false),
	isAllDevicesEnumerated(false)
{
	InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// 
/// Create the DeviceWatcher objects when the user navigates to this page so the UI list of devices is populated.
///
/// The UI elements are controlled/modified through device watcher events, device opening/closing, and app events
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void DeviceConnect::OnNavigatedTo(NavigationEventArgs^ /* e */)
{
	// Default UI state
	UpdateConnectDisconnectButtonsAndList(true);

	// Register for device events in case we are already watching for a device
	EventHandlerForDevice::Current->OnDeviceConnected = ref new TypedEventHandler<EventHandlerForDevice^, OnDeviceConnectedEventArgs^>(this, &DeviceConnect::OnDeviceConnected);
	EventHandlerForDevice::Current->OnDeviceClose = ref new TypedEventHandler<EventHandlerForDevice^, DeviceInformation^>(this, &DeviceConnect::OnDeviceClosing);

	// Begin watching out for app events
	StartHandlingAppEvents();

	// Initalize the desired device watchers so that we can watch for when devices are connected/removed
	InitializeDeviceWatchers();
	StartDeviceWatchers();

	DeviceListSource->Source = listOfDevices;
}

/// <summary>
/// Unregister from App events and DeviceWatcher events because this page will be unloaded.
/// </summary>
/// <param name="e"></param>
void DeviceConnect::OnNavigatedFrom(NavigationEventArgs^ /* e */)
{
	StopDeviceWatchers();
	StopHandlingAppEvents();

	// We no longer care about the device being connected
	EventHandlerForDevice::Current->OnDeviceConnected = nullptr;
	EventHandlerForDevice::Current->OnDeviceClose = nullptr;
}

void DeviceConnect::ConnectToDevice_Click(Object^ /* sender */, RoutedEventArgs^ /* eventArgs */)
{
	auto selection = ConnectDevices->SelectedItems;
	DeviceListEntry^ entry = nullptr;

	if (selection->Size > 0)
	{
		auto obj = selection->GetAt(0);
		entry = safe_cast<DeviceListEntry^>(obj);

		// Create an EventHandlerForDevice to watch for the device we are connecting to
		EventHandlerForDevice::CreateNewEventHandlerForDevice();

		// Get notified when the device was successfully connected to or about to be closed
		EventHandlerForDevice::Current->OnDeviceConnected = ref new TypedEventHandler<EventHandlerForDevice^, OnDeviceConnectedEventArgs^>(this, &DeviceConnect::OnDeviceConnected);
		EventHandlerForDevice::Current->OnDeviceClose = ref new TypedEventHandler<EventHandlerForDevice^, DeviceInformation^>(this, &DeviceConnect::OnDeviceClosing);

		// It is important that the FromIdAsync call is made on the UI thread because the consent prompt can only be displayed
		// on the UI thread. Since this method is invoked by the UI, we are already in the UI thread.
		create_task(EventHandlerForDevice::Current->OpenDeviceAsync(entry->DeviceInformation, entry->DeviceSelector)).then([this](task<bool> openDeviceTask)
		{
			auto openSuccess = openDeviceTask.get();

			// Disable connect button if we connected to the device
			UpdateConnectDisconnectButtonsAndList(!openSuccess);
		});
	}
}

void DeviceConnect::DisconnectFromDevice_Click(Object^ /* sender */, RoutedEventArgs^ /* eventArgs */)
{
	auto selection = ConnectDevices->SelectedItems;
	DeviceListEntry^ entry = nullptr;

	// Prevent auto reconnect because we are voluntarily closing it
	// Re-enable the ConnectDevice list and ConnectToDevice button if the connected/opened device was removed
	EventHandlerForDevice::Current->IsEnabledAutoReconnect = false;

	if (selection->Size > 0)
	{
		auto obj = selection->GetAt(0);
		entry = safe_cast<DeviceListEntry^>(obj);

		if (entry != nullptr)
		{
			EventHandlerForDevice::Current->CloseDevice();
		}
	}

	ButtonDisconnectFromDevice->Content = ButtonNameDisconnectFromDevice;

	UpdateConnectDisconnectButtonsAndList(true);
}

/// <summary>
/// Initialize device watchers to watch for the SuperMUTT.
/// Several other ways of using a device selector to get the desired devices (the devices are really DeviceInformation objects):
/// 1) Using only the Usage Page and the Usage Id of the device (enumerates any device with the same usage page and usage id)
/// 2) Using the Usage Page, Usage Id, Vendor Id, Product Id (Same as above except it also matches the VIDs and PIDs)
///
/// GetDeviceSelector return an AQS string that can be passed directly into DeviceWatcher.createWatcher() or  DeviceInformation.createFromIdAsync(). 
///
/// In this sample, a DeviceWatcher will be used to watch for devices because we can detect surprise device removals.
/// </summary>
void DeviceConnect::InitializeDeviceWatchers(void)
{
	// SuperMUTT
	auto superMuttSelector = HidDevice::GetDeviceSelector(SuperMutt::Device::UsagePage, SuperMutt::Device::UsageId, SuperMutt::Device::Vid, SuperMutt::Device::Pid);

	// Create a device watcher to look for instances of the OSRFX2 device interface and SuperMUTT device
	auto superMuttWatcher = DeviceInformation::CreateWatcher(superMuttSelector);

	// Allow the EventHandlerForDevice to handle device watcher events that relate to or affect our device (i.e. device removal, addition, app suspension/resume)
	AddDeviceWatcher(superMuttWatcher, superMuttSelector);
}

void DeviceConnect::StartHandlingAppEvents(void)
{
	// This event is raised when the app is exited and when the app is suspended
	appSuspendEventToken = App::Current->Suspending += ref new SuspendingEventHandler(this, &DeviceConnect::OnAppSuspension);
	appResumeEventToken = App::Current->Resuming += ref new EventHandler<Object^>(this, &DeviceConnect::OnAppResume);
}

void DeviceConnect::StopHandlingAppEvents(void)
{
	// This event is raised when the app is exited and when the app is suspended
	App::Current->Suspending -= appSuspendEventToken;
	appSuspendEventToken.Value = 0;

	App::Current->Resuming -= appResumeEventToken;
	appResumeEventToken.Value = 0;
}

/// <summary>
/// Registers for Added, Removed, and Enumerated events on the provided deviceWatcher before adding it to an internal list.
/// </summary>
/// <param name="deviceWatcher"></param>
/// <param name="deviceSelector">The AQS used to create the device watcher</param>  
void DeviceConnect::AddDeviceWatcher(DeviceWatcher^ deviceWatcher, String^ deviceSelector)
{
	deviceWatcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>([this, deviceSelector](DeviceWatcher^ deviceWatcher, DeviceInformation^ deviceInformation)
	{
		// Forward this event to a more generic event handler for added devices, but provide it with a deviceSelector that is
		// created that device watcher
		OnDeviceAdded(deviceWatcher, deviceInformation, deviceSelector);
	});
	deviceWatcher->Removed += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(this, &DeviceConnect::OnDeviceRemoved);
	deviceWatcher->EnumerationCompleted += ref new TypedEventHandler<DeviceWatcher^, Object^>(this, &DeviceConnect::OnDeviceEnumerationComplete);

	deviceWatchers->Append(deviceWatcher);
}

/// <summary>
/// Starts all device watchers including ones that have been individually stopped.
/// </summary>
void DeviceConnect::StartDeviceWatchers(void)
{
	// Start all device watchers
	watchersStarted = true;
	isAllDevicesEnumerated = false;

	for (auto deviceWatcher : deviceWatchers)
	{
		if ((deviceWatcher->Status != DeviceWatcherStatus::Started)
			&& (deviceWatcher->Status != DeviceWatcherStatus::EnumerationCompleted))
		{
			deviceWatcher->Start();
		}
	}
}

/// <summary>
/// Stops all device watchers.
/// </summary>
void DeviceConnect::StopDeviceWatchers(void)
{
	// Stop all device watchers
	for (auto deviceWatcher : deviceWatchers)
	{
		if ((deviceWatcher->Status == DeviceWatcherStatus::Started)
			|| (deviceWatcher->Status == DeviceWatcherStatus::EnumerationCompleted))
		{
			deviceWatcher->Stop();
		}
	}

	// Clear the list of devices so we don't have potentially disconnected devices around
	ClearDeviceEntries();

	watchersStarted = false;
}

/// <summary>
/// Creates a DeviceListEntry for a device and adds it to the list of devices in the UI
/// </summary>
/// <param name="deviceInformation">DeviceInformation on the device to be added to the list</param>
/// <param name="deviceSelector">The AQS used to find this device</param>
void DeviceConnect::AddDeviceToList(DeviceInformation^ deviceInformation, String^ deviceSelector)
{
	// search the device list for a device with a matching interface ID
	auto match = FindDevice(deviceInformation->Id);

	// Add the device if it's new
	if (match == nullptr)
	{
		// Create a new element for this device interface, and queue up the query of its
		// device information
		match = ref new DeviceListEntry(deviceInformation, deviceSelector);

		// Add the new element to the end of the list of devices
		listOfDevices->Append(match);
	}
}

void DeviceConnect::RemoveDeviceFromList(String^ deviceId)
{
	// Search the list of devices for one with a matching ID.  Move the matched 
	// item to the end of the list.
	for (uint32 entryIndex = 0; entryIndex < listOfDevices->Size; entryIndex++)
	{
		DeviceListEntry^ entry = listOfDevices->GetAt(entryIndex);

		if (entry->DeviceInformation->Id == deviceId)
		{
			listOfDevices->RemoveAt(entryIndex);

			break;
		}
	}
}

void DeviceConnect::ClearDeviceEntries(void)
{
	listOfDevices->Clear();
}

/// <summary>
/// Searches through the existing list of devices for the first DeviceListEntry that has
/// the specified Id.
/// </summary>
/// <param name="Id">Id of the device that is being searched for</param>
/// <returns>DeviceListEntry that has the provided Id; else a nullptr</returns>
DeviceListEntry^ DeviceConnect::FindDevice(String^ deviceId)
{
	if (deviceId != nullptr)
	{
		for (DeviceListEntry^ entry : listOfDevices)
		{
			if (entry->DeviceInformation->Id == deviceId)
			{
				return entry;
			}
		}
	}

	return nullptr;
}

/// <summary>
/// We must stop the DeviceWatchers because device watchers will continue to raise events even if
/// the app is in suspension, which is not desired (drains battery). We resume the device watcher once the app resumes again.
/// </summary>
/// <param name="sender"></param>
/// <param name="eventArgs"></param>
void DeviceConnect::OnAppSuspension(Object^ /* sender */, SuspendingEventArgs^ /* args */)
{
	if (watchersStarted)
	{
		watchersSuspended = true;
		StopDeviceWatchers();
	}
	else
	{
		watchersSuspended = false;
	}
}

/// <summary>
/// See OnAppSuspension for why we are starting the device watchers again
/// </summary>
/// <param name="sender"></param>
/// <param name="arg"></param>
void DeviceConnect::OnAppResume(Object^ /* sender */, Object^ /* args */)
{
	if (watchersSuspended)
	{
		watchersSuspended = false;
		StartDeviceWatchers();
	}
}

/// <summary>
/// We will remove the device from the UI
/// </summary>
/// <param name="sender"></param>
/// <param name="deviceInformationUpdate"></param>
void DeviceConnect::OnDeviceRemoved(DeviceWatcher^ /* sender */, DeviceInformationUpdate^ deviceInformationUpdate)
{
	MainPage::Current->Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, deviceInformationUpdate]()
	{
		MainPage::Current->NotifyUser(deviceInformationUpdate->Id + " was removed.", NotifyType::StatusMessage);

		RemoveDeviceFromList(deviceInformationUpdate->Id);
	}));

}

/// <summary>
/// This function will add the device to the listOfDevices so that it shows up in the UI
/// </summary>
/// <param name="sender"></param>
/// <param name="deviceInformation"></param>
/// <param name="deviceSelector">AQS used to create the device watcher (the sender)</param>
void DeviceConnect::OnDeviceAdded(DeviceWatcher^ /* sender */, DeviceInformation^ deviceInformation, String^ deviceSelector)
{
	MainPage::Current->Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, deviceInformation, deviceSelector]()
	{
		MainPage::Current->NotifyUser(deviceInformation->Id + " was added.", NotifyType::StatusMessage);

		AddDeviceToList(deviceInformation, deviceSelector);
	}));
}

/// <summary>
/// Notifies the UI that we are done enumerating devices
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void DeviceConnect::OnDeviceEnumerationComplete(DeviceWatcher^ /* sender */, Object^ /* args */)
{
	MainPage::Current->Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		isAllDevicesEnumerated = true;

		// If we finished enumerating devices and the device has not been connected yet, the OnDeviceConnected method
		// is responsible for selecting the device in the device list (UI); otherwise, this method does that
		if (EventHandlerForDevice::Current->IsDeviceConnected)
		{
			SelectDeviceInList(EventHandlerForDevice::Current->DeviceInformation->Id);

			ButtonDisconnectFromDevice->Content = ButtonNameDisconnectFromDevice;

			UpdateConnectDisconnectButtonsAndList(false);

			MainPage::Current->NotifyUser("Currently connected to: " + EventHandlerForDevice::Current->DeviceInformation->Id, NotifyType::StatusMessage);
		}
		else if (EventHandlerForDevice::Current->IsEnabledAutoReconnect && EventHandlerForDevice::Current->DeviceInformation != nullptr)
		{
			// We will be reconnecting to a device
			ButtonDisconnectFromDevice->Content = ButtonNameDisableReconnectToDevice;

			UpdateConnectDisconnectButtonsAndList(false);

			MainPage::Current->NotifyUser("Waiting to reconnect to device: " + EventHandlerForDevice::Current->DeviceInformation->Id, NotifyType::StatusMessage);
		}
		else
		{
			UpdateConnectDisconnectButtonsAndList(true);

			MainPage::Current->NotifyUser("No devices currently selected", NotifyType::StatusMessage);
		}
	}));
}

/// <summary>
/// If all the devices have been enumerated, select the device in the list we connected to. Otherwise let the EnumerationComplete event
/// from the device watcher handle the device selection
/// </summary>
/// <param name="sender"></param>
/// <param name="onDeviceConnectedEventArgs"></param>
void DeviceConnect::OnDeviceConnected(EventHandlerForDevice^ /* sender */, OnDeviceConnectedEventArgs^ onDeviceConnectedEventArgs)
{
	// Find and select our connected device
	if (isAllDevicesEnumerated)
	{
		ButtonDisconnectFromDevice->Content = ButtonNameDisconnectFromDevice;

		if (onDeviceConnectedEventArgs->IsDeviceSuccessfullyConnected)
		{
			SelectDeviceInList(EventHandlerForDevice::Current->DeviceInformation->Id);

			UpdateConnectDisconnectButtonsAndList(false);

			MainPage::Current->NotifyUser("Currently connected to: " + EventHandlerForDevice::Current->DeviceInformation->Id, NotifyType::StatusMessage);
		}
		else
		{
			// Error occurred. EventHandlerForDevice will automatically prevent future automatic reconnections

			UpdateConnectDisconnectButtonsAndList(true);
		}
	}
}

/// <summary>
/// The device was closed. If we will autoreconnect to the device, reflect that in the UI
/// </summary>
/// <param name="sender"></param>
/// <param name="deviceInformation"></param>
void DeviceConnect::OnDeviceClosing(EventHandlerForDevice^ /* sender */, DeviceInformation^ /* deviceInformation */)
{
	MainPage::Current->Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		// We were connected to the device that was unplugged, so change the "Disconnect from device" button
		// to "Do not reconnect to device"
		if (ButtonDisconnectFromDevice->IsEnabled && EventHandlerForDevice::Current->IsEnabledAutoReconnect)
		{
			ButtonDisconnectFromDevice->Content = ButtonNameDisableReconnectToDevice;

			UpdateConnectDisconnectButtonsAndList(false);
		}
		else
		{
			// We don't plan on automatically reconnecting to a device, so enable the connect button
			UpdateConnectDisconnectButtonsAndList(true);
		}
	}));
}

/// <summary>
/// Selects the item in the UI's listbox that corresponds to the provided device id. If there are no
/// matches, we will deselect anything that is selected.
/// </summary>
/// <param name="deviceIdToSelect">The device id of the device to select on the list box</param>
void DeviceConnect::SelectDeviceInList(String^ deviceIdToSelect)
{
	// Don't select anything by default.
	ConnectDevices->SelectedIndex = -1;

	for (uint32 deviceListIndex = 0; deviceListIndex < listOfDevices->Size; deviceListIndex++)
	{
		if (listOfDevices->GetAt(deviceListIndex)->DeviceInformation->Id == deviceIdToSelect)
		{
			ConnectDevices->SelectedIndex = deviceListIndex;

			break;
		}
	}
}


/// <summary>
/// When ButtonConnectToDevice is disabled, ConnectDevices list will also be disabled.
/// </summary>
/// <param name="enableConnectButton">The state of ButtonConnectToDevice</param>
void DeviceConnect::UpdateConnectDisconnectButtonsAndList(bool enableConnectButton)
{
	ButtonConnectToDevice->IsEnabled = enableConnectButton;
	ButtonDisconnectFromDevice->IsEnabled = !ButtonConnectToDevice->IsEnabled;

	ConnectDevices->IsEnabled = ButtonConnectToDevice->IsEnabled;
}

