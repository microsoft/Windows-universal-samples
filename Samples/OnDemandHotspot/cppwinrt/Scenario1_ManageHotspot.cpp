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
#include "SampleConfiguration.h"
#include "Scenario1_ManageHotspot.h"
#include "Scenario1_ManageHotspot.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Devices::WiFi;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Security::Credentials;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_ManageHotspot::Scenario1_ManageHotspot()
    {
        InitializeComponent();
    }

    void Scenario1_ManageHotspot::OnNavigatedTo(NavigationEventArgs const&)
    {
        m_network = WiFiOnDemandHotspotNetwork::GetOrCreateById(Constants::SampleHotspotGuid);

        // Copy the values from the network properties into our UI.
        WiFiOnDemandHotspotNetworkProperties properties = m_network.GetProperties();
        DisplayNameText().Text(properties.DisplayName());
        AvailableToggle().IsOn(properties.Availability() == WiFiOnDemandHotspotAvailability::Available);
        std::optional<WiFiOnDemandHotspotCellularBars> bars = properties.CellularBars();
        CellularBarsToggle().IsOn(bars.has_value());
        static_assert(0 == (int)WiFiOnDemandHotspotCellularBars::ZeroBars);
        static_assert(1 == (int)WiFiOnDemandHotspotCellularBars::OneBar);
        static_assert(2 == (int)WiFiOnDemandHotspotCellularBars::TwoBars);
        static_assert(3 == (int)WiFiOnDemandHotspotCellularBars::ThreeBars);
        static_assert(4 == (int)WiFiOnDemandHotspotCellularBars::FourBars);
        static_assert(5 == (int)WiFiOnDemandHotspotCellularBars::FiveBars);
        CellularBarsSlider().Value(static_cast<int>(bars.value_or(WiFiOnDemandHotspotCellularBars::ZeroBars)));
        std::optional<uint32_t> batteryPercent = properties.RemainingBatteryPercent();
        BatteryPercentageToggle().IsOn(batteryPercent.has_value());
        BatteryPercentageSlider().Value(batteryPercent.value_or(0));
        SsidText().Text(properties.Ssid());
        PasswordCredential password = properties.Password();
        PasswordText().Password(password ? password.Password() : L"");

        // Determine whether the background tasks are registered.
        bool isRegistered = false;
        for (const auto& [guid, task] : BackgroundTaskRegistration::AllTasks())
        {
            hstring name = task.Name();
            if (name == Constants::MetadataUpdateTaskName ||
                name == Constants::DeviceWatcherTaskName ||
                name == Constants::ConnectTaskName)
            {
                isRegistered = true;
                break;
            }
        }

        VisualStateManager::GoToState(*this, isRegistered ? L"Registered" : L"NotRegistered", false);
    }

    void Scenario1_ManageHotspot::RegisterTasks_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Register the background task which updates the hotspot metadata when the system
        // shows a list of Wi-Fi networks.
        WiFiOnDemandHotspotUpdateMetadataTrigger updateMetadataTrigger;
        BackgroundTaskBuilder backgroundTaskBuilder;
        backgroundTaskBuilder.Name(Constants::MetadataUpdateTaskName);
        backgroundTaskBuilder.TaskEntryPoint(Constants::MetadataUpdateEntryPoint);
        backgroundTaskBuilder.SetTrigger(updateMetadataTrigger);
        BackgroundTaskRegistration updateMetadataRegistration = backgroundTaskBuilder.Register();

        // Register the background task which turns on the on-demand hotspot when the user
        // selects it.
        WiFiOnDemandHotspotConnectTrigger connectTrigger;
        backgroundTaskBuilder.Name(Constants::ConnectTaskName);
        backgroundTaskBuilder.TaskEntryPoint(Constants::ConnectEntryPoint);
        backgroundTaskBuilder.SetTrigger(connectTrigger);
        BackgroundTaskRegistration connectRegistration = backgroundTaskBuilder.Register();

        // In practice, you will monitor when an on-demand hotspot comes into range or goes
        // of range so you can update the Availability. This sample does not have a physical
        // hotspot device, but we leave the code here as a demonstration.
#if 0
        DeviceWatcher deviceWatcher = DeviceInformation::CreateWatcher(Contoso::Devices::OnDemandHotSpot::GetDeviceSelector());
        DeviceWatcherTrigger watcherTrigger = deviceWatcher.GetBackgroundTrigger({ DeviceWatcherEventKind::Add, DeviceWatcherEventKind::Remove, DeviceWatcherEventKind::Update });
        backgroundTaskBuilder.Name(Constants::DeviceWatcherTaskName);
        backgroundTaskBuilder.TaskEntryPoint(Constants::DeviceWatcherEntryPoint);
        backgroundTaskBuilder.SetTrigger(watcherTrigger);
        BackgroundTaskRegistration deviceWatcherRegistration = backgroundTaskBuilder.Register();
#endif

        VisualStateManager::GoToState(*this, L"Registered", false);

    }

    void Scenario1_ManageHotspot::UnregisterTasks_Click(IInspectable const&, RoutedEventArgs const&)
    {
        for (const auto& [guid, task] : BackgroundTaskRegistration::AllTasks())
        {
            hstring name = task.Name();
            if (name == Constants::MetadataUpdateTaskName ||
                name == Constants::DeviceWatcherTaskName ||
                name == Constants::ConnectTaskName)
            {
                task.Unregister(true);
            }
        }

        VisualStateManager::GoToState(*this, L"NotRegistered", false);
    }

    bool Scenario1_ManageHotspot::SavePropertiesForBackgroundTask()
    {
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        if (AvailableToggle().IsOn() && DisplayNameText().Text().empty())
        {
            rootPage.NotifyUser(L"An Available network must have a name.", NotifyType::ErrorMessage);
            return false;
        }

        if (SsidText().Text().empty())
        {
            rootPage.NotifyUser(L"You must provide an SSID for the Wi-Fi network.", NotifyType::ErrorMessage);
            return false;
        }

        if (PasswordText().Password().empty())
        {
            rootPage.NotifyUser(L"You must provide a password for the Wi-Fi network.", NotifyType::ErrorMessage);
            return false;
        }

        IPropertySet values = ApplicationData::Current().LocalSettings().Values();
        values.Insert(L"availability", box_value(AvailableToggle().IsOn()));
        values.Insert(L"displayName", box_value(DisplayNameText().Text()));
        values.Insert(L"bars", CellularBarsToggle().IsOn() ? box_value(static_cast<int>(CellularBarsSlider().Value())) : nullptr);
        values.Insert(L"battery", BatteryPercentageToggle().IsOn() ? box_value(static_cast<uint32_t>(BatteryPercentageSlider().Value())) : nullptr);
        values.Insert(L"ssid", box_value(SsidText().Text()));
        values.Insert(L"password", box_value(PasswordText().Password()));
        values.Insert(L"hotspotGuid", box_value(Constants::SampleHotspotGuid));

        return true;
    }

    void Scenario1_ManageHotspot::UpdateNowButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (SavePropertiesForBackgroundTask())
        {
            // Copy the properties from the UI into the system network properties
            // to configure how the on-demand hotspot appears in the system Wi-Fi list.
            WiFiOnDemandHotspotNetworkProperties properties = m_network.GetProperties();
            properties.Availability(AvailableToggle().IsOn() ? WiFiOnDemandHotspotAvailability::Available : WiFiOnDemandHotspotAvailability::Unavailable);
            properties.DisplayName(DisplayNameText().Text());
            properties.CellularBars(CellularBarsToggle().IsOn() ? std::optional(static_cast<WiFiOnDemandHotspotCellularBars>(CellularBarsSlider().Value())) : std::nullopt);
            properties.RemainingBatteryPercent(BatteryPercentageToggle().IsOn() ? std::optional(static_cast<uint32_t>(BatteryPercentageSlider().Value())) : std::nullopt);
            properties.Ssid(SsidText().Text());

            PasswordCredential credential;
            credential.Password(PasswordText().Password());
            properties.Password(credential);

            m_network.UpdateProperties(properties);
        }
    }

    void Scenario1_ManageHotspot::UpdateOnDemandButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SavePropertiesForBackgroundTask();
    }
}
