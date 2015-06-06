// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_CreateTPMVSC.g.h"
#include "MainPage.xaml.h"

namespace Smartcard
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario1_CreateTPMVSC sealed
	{
	public:
		Scenario1_CreateTPMVSC();
	private:
		static const int pinPolicyDisallowedIndex = 0;
		static const int pinPolicyAllowedIndex = 1;
		static const int pinPolicyRequireOneIndex = 2;

		void Create_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void HandleCardAdded(Windows::Devices::SmartCards::SmartCardReader^ evtReader, Windows::Devices::SmartCards::CardAddedEventArgs^ args);
		Windows::Devices::SmartCards::SmartCardPinPolicy^ ParsePinPolicy();
		Windows::Devices::SmartCards::SmartCardReader^ reader;
	};
}