//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once

#include "pch.h"
#include "SampleConfiguration.h"

namespace CustomHidDeviceAccess
{
	/// <summary>
	/// The purpose of this class is to demonstrate what to do to a HidDevice when a specific app event
	/// is raised (app suspension and resume) or when the device is disconnected. In addition to handling
	/// the HidDevice, the app's state should also be saved upon app suspension (will not be demonstrated here).
	/// 
	/// This class will also demonstrate how to handle device watcher events.
	/// 
	/// For simplicity, this class will only allow at most one device to be connected at any given time. In order
	/// to make this class support multiple devices, make this class a non-singleton and create multiple instances
	/// of this class; each instance should watch one connected device.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class EventHandlerForDevice sealed
	{
	public:
		static property EventHandlerForDevice^ Current
		{
			EventHandlerForDevice^ get(void);
		}

		static void CreateNewEventHandlerForDevice(void);

		property Windows::Foundation::TypedEventHandler<EventHandlerForDevice^, Windows::Devices::Enumeration::DeviceInformation^>^ OnDeviceClose
		{
			Windows::Foundation::TypedEventHandler<EventHandlerForDevice^, Windows::Devices::Enumeration::DeviceInformation^>^ get(void)
			{
				return deviceCloseCallback;
			}

			void set(Windows::Foundation::TypedEventHandler<EventHandlerForDevice^, Windows::Devices::Enumeration::DeviceInformation^>^ newHandler)
			{
				deviceCloseCallback = newHandler;
			}
		}

		property Windows::Foundation::TypedEventHandler<EventHandlerForDevice^, OnDeviceConnectedEventArgs^>^ OnDeviceConnected
		{
			Windows::Foundation::TypedEventHandler<EventHandlerForDevice^, OnDeviceConnectedEventArgs^>^ get(void)
			{
				return deviceConnectedCallback;
			}

			void set(Windows::Foundation::TypedEventHandler<EventHandlerForDevice^, OnDeviceConnectedEventArgs^>^ newHandler)
			{
				deviceConnectedCallback = newHandler;
			}
		}

		property bool IsDeviceConnected
		{
			bool get(void)
			{
				return device != nullptr;
			}
		}

		property Windows::Devices::HumanInterfaceDevice::HidDevice^ Device
		{
			Windows::Devices::HumanInterfaceDevice::HidDevice^ get(void)
			{
				return device;
			}
		}

		/// <summary>
		/// This DeviceInformation represents which device is connected or which device will be reconnected when
		/// the device is plugged in again (if IsEnabledAutoReconnect is true);.
		/// </summary>
		property Windows::Devices::Enumeration::DeviceInformation^ DeviceInformation
		{
			Windows::Devices::Enumeration::DeviceInformation^ get(void)
			{
				return deviceInformation;
			}
		}

		/// <summary>
		/// Returns DeviceAccessInformation for the device that is currently connected using this EventHandlerForDevice
		/// object.
		/// </summary>
		property Windows::Devices::Enumeration::DeviceAccessInformation^ DeviceAccessInformation
		{
			Windows::Devices::Enumeration::DeviceAccessInformation^ get(void)
			{
				return deviceAccessInformation;
			}
		}

		/// <summary>
		/// DeviceSelector AQS used to find this device
		/// </summary>
		property Platform::String^ DeviceSelector
		{
			Platform::String^ get(void)
			{
				return deviceSelector;
			}
		}

		/// <summary>
		/// True if EventHandlerForDevice will attempt to reconnect to the device once it is plugged into the computer again
		/// </summary>
		property bool IsEnabledAutoReconnect
		{
			bool get(void)
			{
				return isEnabledAutoReconnect;
			}
			void set(bool value)
			{
				isEnabledAutoReconnect = value;
			}
		}

		Windows::Foundation::IAsyncOperation<bool>^ OpenDeviceAsync(Windows::Devices::Enumeration::DeviceInformation^ deviceInfo, Platform::String^ deviceSelectorString);

		void CloseDevice(void);
	private:
		/// <summary>
		/// Allows for singleton EventHandlerForDevice
		/// </summary>
		static EventHandlerForDevice^ eventHandlerForDevice;

		/// <summary>
		/// Used to synchronize threads to avoid multiple instantiations of eventHandlerForDevice.
		/// </summary>
		static SRWLOCK srwSingletonCreationLock;

		Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher;
		Platform::String^ deviceSelector;

		Windows::Devices::Enumeration::DeviceInformation^ deviceInformation;
		Windows::Devices::Enumeration::DeviceAccessInformation^ deviceAccessInformation;
		Windows::Devices::HumanInterfaceDevice::HidDevice^ device;

		Windows::Foundation::EventRegistrationToken appSuspendEventToken;
		Windows::Foundation::EventRegistrationToken appResumeEventToken;

		Windows::Foundation::EventRegistrationToken deviceAddedEventToken;
		Windows::Foundation::EventRegistrationToken deviceRemovedEventToken;

		Windows::Foundation::EventRegistrationToken deviceAccessChangedEventToken;

		Windows::Foundation::TypedEventHandler<EventHandlerForDevice^, Windows::Devices::Enumeration::DeviceInformation^>^ deviceCloseCallback;
		Windows::Foundation::TypedEventHandler<EventHandlerForDevice^, OnDeviceConnectedEventArgs^>^ deviceConnectedCallback;

		bool watcherSuspended;
		bool watcherStarted;
		bool isEnabledAutoReconnect;

		EventHandlerForDevice(void);
		~EventHandlerForDevice();

		void CloseCurrentlyConnectedDevice(void);

		void RegisterForAppEvents(void);
		void UnregisterFromAppEvents(void);

		void RegisterForDeviceAccessStatusChange();
		void UnregisterFromDeviceAccessStatusChange();

		void RegisterForDeviceWatcherEvents(void);
		void UnregisterFromDeviceWatcherEvents(void);

		void StartDeviceWatcher(void);
		void StopDeviceWatcher(void);

		void OnAppSuspension(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
		void OnAppResume(Platform::Object^ sender, Platform::Object^ args);

		void OnDeviceRemoved(
			Windows::Devices::Enumeration::DeviceWatcher^ sender,
			Windows::Devices::Enumeration::DeviceInformationUpdate^ deviceInformationUpdate);

		void OnDeviceAdded(
			Windows::Devices::Enumeration::DeviceWatcher^ sender,
			Windows::Devices::Enumeration::DeviceInformation^ deviceInfo);

		void OnDeviceAccessChanged(
			Windows::Devices::Enumeration::DeviceAccessInformation^ sender,
			Windows::Devices::Enumeration::DeviceAccessChangedEventArgs^ eventArgs);
	};
}
