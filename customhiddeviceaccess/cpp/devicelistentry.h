//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once

#include "SampleConfiguration.h"

namespace CustomHidDeviceAccess
{
	/// <summary>
	/// The class will only expose properties from DeviceInformation that are going to be used
	/// in this sample. Each instance of this class provides information about a single device.
	///
	/// This class is used by the UI to display device specific information so that
	/// the user can identify which device to use.
	/// </summary>
	[Windows::UI::Xaml::Data::Bindable]
	public ref class DeviceListEntry sealed
	{
	public:
		property Platform::String^ InstanceId
		{
			Platform::String^ get(void);
		}

		property Windows::Devices::Enumeration::DeviceInformation^ DeviceInformation
		{
			Windows::Devices::Enumeration::DeviceInformation^ get(void)
			{
				return device;
			}
		}

		property Platform::String^ DeviceSelector
		{
			Platform::String^ get(void)
			{
				return deviceSelector;
			}
		}

		DeviceListEntry(Windows::Devices::Enumeration::DeviceInformation^ deviceInformation, Platform::String^ deviceSelector);

	private:
		Windows::Devices::Enumeration::DeviceInformation^ device;
		Platform::String^ deviceSelector;
	};
}