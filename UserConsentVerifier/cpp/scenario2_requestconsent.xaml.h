// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_RequestConsent.g.h"
#include "MainPage.xaml.h"

namespace UserConsentVerifier
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_RequestConsent sealed
    {
    public:
        Scenario2_RequestConsent();
	private:
		void RequestConsent_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
