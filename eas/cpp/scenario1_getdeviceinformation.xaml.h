// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_GetDeviceInformation.g.h"
#include "MainPage.xaml.h"

namespace EAS
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_GetDeviceInformation sealed
    {
    public:
        Scenario1_GetDeviceInformation();
    private:
		SDKTemplate::MainPage^ rootPage;

		void Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
