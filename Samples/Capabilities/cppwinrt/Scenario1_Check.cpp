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
#include "Scenario1_Check.h"
#include "Scenario1_Check.g.cpp"
#include <sstream>

using namespace winrt;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Security::Authorization::AppCapabilityAccess;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace std::literals::chrono_literals;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Check::Scenario1_Check()
    {
        InitializeComponent();
    }

    void Scenario1_Check::OnNavigatedTo(NavigationEventArgs const&)
    {
        // Specify which capability we want to query/monitor.
        locationCapability = AppCapability::Create(L"location");

        // Register a handler to be called when access changes.
        accessChangedToken = locationCapability.AccessChanged({ get_weak(), &Scenario1_Check::OnCapabilityAccessChanged });

        // Update UI to match current access.
        UpdateCapabilityStatus();
    }

    void Scenario1_Check::OnNavigatedFrom(NavigationEventArgs const&)
    {
        locationCapability.AccessChanged(accessChangedToken);
    }

    fire_and_forget Scenario1_Check::OnCapabilityAccessChanged(AppCapability const&, IInspectable const&)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());
        UpdateCapabilityStatus();
    }

    fire_and_forget Scenario1_Check::StreamLocationButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        switch (locationCapability.CheckAccess())
        {
        case AppCapabilityAccessStatus::Allowed:
            // Access was already granted.
            // AccessChanged event will trigger a recalc.
            co_await StreamLocationAsync();
            break;

        case AppCapabilityAccessStatus::UserPromptRequired:
            // Request access directly for a better experience than going to Settings.
            // This is equivalent to calling Geolocator::RequestAccessAsync().
            if (co_await locationCapability.RequestAccessAsync() == AppCapabilityAccessStatus::Allowed)
            {
                // The user granted access.
                // AccessChanged event will trigger a recalc.
                co_await StreamLocationAsync();
            }
            break;

        case AppCapabilityAccessStatus::DeniedByUser:
        case AppCapabilityAccessStatus::DeniedBySystem:
        default:
            // Send user to Settings to obtain location permission
            // or explain why access is denied.
            co_await Launcher::LaunchUriAsync(Uri(L"ms-settings:privacy-location"));
            break;
        }
    }

    IAsyncAction Scenario1_Check::StreamLocationAsync()
    {
        auto lifetime = get_strong();

        if (co_await Geolocator::RequestAccessAsync() == GeolocationAccessStatus::Allowed)
        {
            // Need try/catch because we can lose geolocator access at any time.
            try
            {
                // Specify a message to explain to the user why we are tracking their location.
                locationCapability.DisplayMessage(winrt::hstring(L"Streaming location"));

                LocationTextBlock().Text(L"Finding your current location...");
                Geolocator geolocator;

                for (int i = 0; i < 4; i++)
                {
                    Geoposition pos = co_await geolocator.GetGeopositionAsync();

                    if (pos == nullptr)
                    {
                        LocationTextBlock().Text(L"Current location unknown.");
                    }
                    else
                    {
                        std::wostringstream output;
                        output << L"Your location is: Latitude " << pos.Coordinate().Point().Position().Latitude <<
                            L", Longitude" << pos.Coordinate().Point().Position().Longitude;
                        LocationTextBlock().Text(output.str());
                    }

                    co_await winrt::resume_after(500ms);
                    co_await resume_foreground(Dispatcher());

                    // You can change the message as your app's usage changes.
                    locationCapability.DisplayMessage(L"Tracking your current location...");
                }
            }
            catch (hresult_access_denied const&)
            {
                // Lost access in the middle of the operation.
                // AccessChanged event will trigger a recalc.
            }

            LocationTextBlock().Text(L"");

            // Even though we clear the message immediately, it will take time before the message
            // disappear from the system. The system has a minimum display time for the message,
            // so that users can observe brief usage.
            locationCapability.DisplayMessage(L"");
        }
        else
        {
            // Lost access before operation started.
            // AccessChanged event will trigger a recalc.
        }
    }

    void Scenario1_Check::UpdateCapabilityStatus()
    {
        auto lifetime = get_strong();

        AppCapabilityAccessStatus status = locationCapability.CheckAccess();
        if (status == AppCapabilityAccessStatus::Allowed)
        {
            LocationAccessBlock().Visibility(Visibility::Collapsed);
            LocationTextBlock().Visibility(Visibility::Visible);
        }
        else
        {
            LocationTextBlock().Visibility(Visibility::Collapsed);

            switch (status)
            {
            case AppCapabilityAccessStatus::NotDeclaredByApp:
                // The app neglected to declare the capability in its manifest.
                // This is a developer error.
                LocationAccessBlock().Text(L"App misconfiguration error. Contact vendor for support.");
                LocationAccessBlock().Visibility(Visibility::Visible);
                break;

            default:
            case AppCapabilityAccessStatus::DeniedBySystem:
                // We can send the user to the Settings page to obtain access
                // or at least explain why access is denied.
                LocationAccessBlock().Text(L"The system has blocked access to location.");
                LocationAccessBlock().Visibility(Visibility::Visible);
                break;

            case AppCapabilityAccessStatus::DeniedByUser:
                // We can send the user to the Settings page to obtain access.
                LocationAccessBlock().Text(L"You must enable location access in Settings.");
                LocationAccessBlock().Visibility(Visibility::Visible);
                break;

            case AppCapabilityAccessStatus::UserPromptRequired:
                // We can prompt the user to give us access.
                LocationAccessBlock().Visibility(Visibility::Collapsed);
                break;
            }
        }
    }
}