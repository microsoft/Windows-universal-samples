// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "MainPage.xaml.h"
#include "Scenario2_oAuthFacebook.g.h"

namespace WebAuthentication
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_oAuthFacebook sealed
    {
    public:
        Scenario2_oAuthFacebook();
	private:
		void Launch_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
