//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario2_Settings.h"
#include "Scenario2_Settings.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Settings::Scenario2_Settings()
    {
        InitializeComponent();
    }

    // Guidance for Settings.
    //
    // Settings are a convenient way of storing small pieces of configuration data
    // for your application.
    //
    // There are two storage containers for settings: Local and Roaming. However,
    // Roaming settings no longer roam, so they are functionally equivalent to Local.
    //
    // Care should be taken to guard against an excessive volume of data being
    // stored in settings.  Settings are not intended to be used as a database.
    // Large data sets will take longer to load from disk during your application's
    // launch.

    // This sample illustrates reading and writing from a local setting.

    void Scenario2_Settings::WriteSetting_Click(IInspectable const&, RoutedEventArgs const&)
    {
        hstring toSet = L"Hello World";
        localSettings.Values().Insert(SettingName, box_value(toSet)); // example value

        DisplayOutput();
    }

    void Scenario2_Settings::DeleteSetting_Click(IInspectable const&, RoutedEventArgs const&)
    {
        localSettings.Values().Remove(SettingName);

        DisplayOutput();
    }

    void Scenario2_Settings::DisplayOutput()
    {
        auto stringValue = localSettings.Values().TryLookup(SettingName).try_as<hstring>();

        OutputTextBlock().Text(L"Setting: " + (stringValue.has_value() ? L"\"" + stringValue.value() + L"\"" : L"<empty>"));
    }

    void Scenario2_Settings::OnNavigatedTo(NavigationEventArgs const&)
    {
        DisplayOutput();
    }
}

