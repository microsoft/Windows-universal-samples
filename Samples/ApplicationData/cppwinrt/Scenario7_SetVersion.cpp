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
#include "Scenario7_SetVersion.h"
#include "Scenario7_SetVersion.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario7_SetVersion::Scenario7_SetVersion()
    {
        InitializeComponent();

        DisplayOutput();
    }

    void Scenario7_SetVersion::SetVersionHandler0(SetVersionRequest const& request)
    {
        auto deferral = request.GetDeferral();

        uint32_t version = appData.Version();

        switch (version)
        {
        case 0:
            // Version is already 0.  Nothing to do.
            break;

        case 1:
            // Need to convert data from v1 to v0.

            // This sample simulates that conversion by writing a version-specific value.
            appData.LocalSettings().Values().Insert(settingName, box_value(settingValue0));

            break;

        default:
            throw hresult_invalid_argument(L"Unexpected ApplicationData Version: " + to_hstring(version));
        }

        deferral.Complete();
    }

    void Scenario7_SetVersion::SetVersionHandler1(SetVersionRequest const& request)
    {
        auto deferral = request.GetDeferral();

        uint32_t version = appData.Version();

        switch (version)
        {
        case 0:
            // Need to convert data from v0 to v1.

            // This sample simulates that conversion by writing a version-specific value.
            appData.LocalSettings().Values().Insert(settingName, box_value(settingValue1));

            break;

        case 1:
            // Version is already 1.  Nothing to do.
            break;

        default:
            throw hresult_invalid_argument(L"Unexpected ApplicationData Version: " + to_hstring(version));
        }

        deferral.Complete();
    }

    fire_and_forget Scenario7_SetVersion::SetVersion0_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto strong = get_strong();
        co_await appData.SetVersionAsync(0, { get_weak(), &Scenario7_SetVersion::SetVersionHandler0 });
        DisplayOutput();
    }

    fire_and_forget Scenario7_SetVersion::SetVersion1_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto strong = get_strong();
        co_await appData.SetVersionAsync(1, { get_weak(), &Scenario7_SetVersion::SetVersionHandler1 });
        DisplayOutput();
    }

    void Scenario7_SetVersion::DisplayOutput()
    {
        OutputTextBlock().Text(L"Version: " + to_hstring(appData.Version()));
    }

    void Scenario7_SetVersion::OnNavigatedTo(NavigationEventArgs const&)
    {
        DisplayOutput();
    }
}

