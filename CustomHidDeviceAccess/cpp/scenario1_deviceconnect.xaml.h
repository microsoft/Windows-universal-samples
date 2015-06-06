//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario1_DeviceConnect.xaml.h
// Declaration of the DeviceConnect class
//

#pragma once
#include "DeviceListEntry.h"
#include "EventHandlerForDevice.h"
#include "Scenario1_DeviceConnect.g.h"

namespace CustomHidDeviceAccess
{
	/// <summary>
	/// Demonstrates how to connect to a Hid device and respond to device disconnects, app suspending and resuming events.
	/// 
	/// To use this sample with your device:
	/// 1) Include the device in the Package.appxmanifest. For instructions, see Package.appxmanifest documentation.
	/// 2) Create a DeviceWatcher object for your device. See the InitializeDeviceWatcher method in this sample.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class DeviceConnect sealed
	{
	public:
		DeviceConnect(void);

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	private:
		static Platform::String^ ButtonNameDisconnectFromDevice;
		static Platform::String^ ButtonNameDisableReconnectToDevice;

		Windows::Foundation::EventRegistrationToken appSuspendEventToken;
		Windows::Foundation::EventRegistrationToken appResumeEventToken;

		Platform::Collections::Vector<DeviceListEntry^>^ listOfDevices;

		Platform::Collections::Vector<Windows::Devices::Enumeration::DeviceWatcher^>^ deviceWatchers;
		bool watchersSuspended;
		bool watchersStarted;

		// Has all the devices enumerated by the device watcher?
		bool isAllDevicesEnumerated;

		void ConnectToDevice_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs);
		void DisconnectFromDevice_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ eventArgs);

		void InitializeDeviceWatchers(void);
		void InitializeOsrFx2DeviceWatcher(void);
		void InitializeSuperMuttDeviceWatcher(void);

		void StartHandlingAppEvents(void);
		void StopHandlingAppEvents(void);

		void AddDeviceWatcher(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Platform::String^ deviceSelector);

		void StartDeviceWatchers(void);
		void StopDeviceWatchers(void);

		void AddDeviceToList(Windows::Devices::Enumeration::DeviceInformation^ deviceInformation, Platform::String^ deviceSelector);
		void RemoveDeviceFromList(Platform::String^ deviceId);

		void ClearDeviceEntries(void);

		DeviceListEntry^ FindDevice(Platform::String^ deviceId);

		void OnAppSuspension(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
		void OnAppResume(Platform::Object^ sender, Platform::Object^ args);

		void OnDeviceRemoved(
			Windows::Devices::Enumeration::DeviceWatcher^ sender,
			Windows::Devices::Enumeration::DeviceInformationUpdate^ deviceInformationUpdate);

		void OnDeviceAdded(
			Windows::Devices::Enumeration::DeviceWatcher^ sender,
			Windows::Devices::Enumeration::DeviceInformation^ deviceInformation,
			Platform::String^ deviceSelector);

		void OnDeviceEnumerationComplete(Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::Object^ args);

		void OnDeviceConnected(EventHandlerForDevice^ sender, OnDeviceConnectedEventArgs^ onDeviceConnectedEventArgs);
		void OnDeviceClosing(EventHandlerForDevice^ sender, Windows::Devices::Enumeration::DeviceInformation^ deviceInformation);

		void SelectDeviceInList(Platform::String^ deviceIdToSelect);

		void UpdateConnectDisconnectButtonsAndList(bool enableConnectButton);
	};
}
