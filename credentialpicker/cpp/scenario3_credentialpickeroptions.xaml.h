// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3_CredentialPickerOptions.g.h"
#include "MainPage.xaml.h"

namespace CredentialPicker
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_CredentialPickerOptions sealed
    {
    public:
        Scenario3_CredentialPickerOptions();
    private:
        void Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SetResult(Windows::Security::Credentials::UI::CredentialPickerResults^ res);
        void SetPasswordExplainVisibility(bool isShown);
        void Protocol_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
    };
}
