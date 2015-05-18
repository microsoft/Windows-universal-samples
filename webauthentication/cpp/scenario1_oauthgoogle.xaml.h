// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_oAuthGoogle.g.h"
#include "MainPage.xaml.h"

namespace WebAuthentication
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_oAuthGoogle sealed
    {
    public:
        Scenario1_oAuthGoogle();
    private:
		void Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
