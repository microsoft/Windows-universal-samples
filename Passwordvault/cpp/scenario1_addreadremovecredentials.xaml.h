// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_AddReadRemoveCredentials.g.h"
#include "MainPage.xaml.h"

namespace PasswordVault
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_AddReadRemoveCredentials sealed
    {
    public:
        Scenario1_AddReadRemoveCredentials();
    private:
		void Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Save_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Read_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Delete_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		Windows::Foundation::IAsyncAction^ InitializeVaultAsync();
    };
}
