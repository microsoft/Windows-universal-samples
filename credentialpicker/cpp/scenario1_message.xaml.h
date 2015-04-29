// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_Message.g.h"
#include "MainPage.xaml.h"

namespace CredentialPicker
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Message sealed
    {
    public:
        Scenario1_Message();
    private:
		void Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SetError(Platform::String^ errorText);
		void SetResult(Windows::Security::Credentials::UI::CredentialPickerResults^ res);
    };
}
