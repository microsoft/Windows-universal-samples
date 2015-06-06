// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario6_DeleteTPMVSC.g.h"
#include "MainPage.xaml.h"

namespace Smartcard
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario6_DeleteTPMVSC sealed
	{
	public:
		Scenario6_DeleteTPMVSC();
	private:
		void Delete_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void HandleCardRemoved(Windows::Devices::SmartCards::SmartCardReader^ evtReader, Windows::Devices::SmartCards::CardRemovedEventArgs^ args);
		Windows::Devices::SmartCards::SmartCardReader ^reader;
	};
}