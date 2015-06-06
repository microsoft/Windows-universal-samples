// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_MessageCaption.g.h"
#include "MainPage.xaml.h"

namespace CredentialPicker
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_MessageCaption sealed
    {
    public:
        Scenario2_MessageCaption();
	private:
		void Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SetResult(Windows::Security::Credentials::UI::CredentialPickerResults^ res);
    };
}
