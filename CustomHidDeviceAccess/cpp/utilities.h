//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once

#include "pch.h"
#include "SampleConfiguration.h"
#include "EventHandlerForDevice.h"

namespace CustomHidDeviceAccess
{
	class Utilities
	{
	public:
		static void SetUpDeviceScenarios(
			Windows::Foundation::Collections::IMap<CustomHidDeviceAccess::DeviceType, Windows::UI::Xaml::UIElement^>^ scenarios,
			Windows::UI::Xaml::UIElement^ scenarioContainer);

		static void NotifyDeviceNotConnected(void);

		static DeviceType GetDeviceType(Windows::Devices::HumanInterfaceDevice::HidDevice^ device);

		static bool IsSuperMuttDevice(Windows::Devices::HumanInterfaceDevice::HidDevice^ device);

		static Platform::String^ ConvertToHex16Bit(uint16 value);

		static Platform::String^ ConvertToHex8Bit(uint8 value);

		static Platform::String^ ConvertToHex(uint32 value);
	};
}

