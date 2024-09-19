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

#pragma once

#include "Scenario1_Basics.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Basics : Scenario1_BasicsT<Scenario1_Basics>
    {
    public:
        Scenario1_Basics();

        void OnNavigatedTo(winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        void Apply_Clicked(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        winrt::Windows::Devices::Enumeration::DeviceWatcher m_deviceWatcher{ nullptr };

        // Currently attached LampArrays
        std::vector<std::shared_ptr<LampArrayInfo>> m_attachedLampArrays;

        winrt::Windows::Devices::Enumeration::DeviceWatcher::Added_revoker m_deviceAddedRevoker;
        winrt::Windows::Devices::Enumeration::DeviceWatcher::Removed_revoker m_deviceRemovedRevoker;

        winrt::fire_and_forget Watcher_Added(
            winrt::Windows::Devices::Enumeration::DeviceWatcher const&,
            winrt::Windows::Devices::Enumeration::DeviceInformation const& deviceInformation);

        winrt::fire_and_forget Watcher_Removed(
            winrt::Windows::Devices::Enumeration::DeviceWatcher const&,
            winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInformationUpdate);

        winrt::fire_and_forget LampArray_AvailabilityChanged(
            winrt::Windows::Devices::Lights::LampArray const& sender,
            winrt::Windows::Foundation::IInspectable const&);

        void UpdateLampArrayList();

        void ApplySettingsToLampArray(winrt::Windows::Devices::Lights::LampArray const& lampArray);
        void SetGradientPatternToLampArray(winrt::Windows::Devices::Lights::LampArray const& lampArray);
        void SetWasdPatternToLampArray(winrt::Windows::Devices::Lights::LampArray const& lampArray);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Basics : Scenario1_BasicsT<Scenario1_Basics, implementation::Scenario1_Basics>
    {
    };
}
