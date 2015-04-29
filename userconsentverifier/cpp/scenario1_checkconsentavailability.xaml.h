// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_CheckConsentAvailability.g.h"
#include "MainPage.xaml.h"

namespace UserConsentVerifier
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_CheckConsentAvailability sealed
    {
    public:
        Scenario1_CheckConsentAvailability();
    private:
		void CheckAvailability_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
