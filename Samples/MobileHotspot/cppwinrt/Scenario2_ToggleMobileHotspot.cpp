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
#include "Scenario2_ToggleMobileHotspot.h"
#include "Scenario2_ToggleMobileHotspot.g.cpp"
#include "MainPage.h"

using namespace winrt;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::NetworkOperators;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_ToggleMobileHotspot::Scenario2_ToggleMobileHotspot()
    {
        InitializeComponent();
    }

    void Scenario2_ToggleMobileHotspot::OnNavigatedTo(NavigationEventArgs const&)
    {
        MainPage::Current().NotifyUser(L"", NotifyType::StatusMessage);

        m_tetheringManager = SDKTemplate::TryGetCurrentNetworkOperatorTetheringManager();

        if (m_tetheringManager)
        {
            NetworkOperatorTetheringAccessPointConfiguration configuration = m_tetheringManager.GetCurrentAccessPointConfiguration();
            SsidRun().Text(configuration.Ssid());
            PasswordRun().Text(configuration.Passphrase());
            BandRun().Text(SDKTemplate::GetFriendlyName(configuration.Band()));

            m_toggling = true;
            bool isTethered = m_tetheringManager.TetheringOperationalState() == TetheringOperationalState::On;
            MobileHotspotToggle().IsOn(isTethered);
            m_toggling = false;

            HotspotPanel().Visibility(Visibility::Visible);
        }
    }

    fire_and_forget Scenario2_ToggleMobileHotspot::MobileHotspotToggle_Toggled(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        if (m_toggling)
        {
            co_return;
        }
        MainPage::Current().NotifyUser(L"", NotifyType::StatusMessage);

        bool isTethered = m_tetheringManager.TetheringOperationalState() == TetheringOperationalState::On;
        if (MobileHotspotToggle().IsOn() == isTethered)
        {
            // Already in desired state.
            co_return;
        }

        // Switch to opposite state.
        m_toggling = true;

        NetworkOperatorTetheringOperationResult result{ nullptr };
        if (isTethered)
        {
            result = co_await m_tetheringManager.StopTetheringAsync();
        }
        else
        {
            result = co_await m_tetheringManager.StartTetheringAsync();
        }

        if (result.Status() == TetheringOperationStatus::Success)
        {
            // Change to new state.
            MobileHotspotToggle().IsOn(!isTethered);
        }
        else
        {
            // Return to old state.
            MobileHotspotToggle().IsOn(isTethered);

            switch (result.Status())
            {
            case TetheringOperationStatus::WiFiDeviceOff:
                MainPage::Current().NotifyUser(
                    L"Wi-Fi adapter is either turned off or missing.",
                    NotifyType::ErrorMessage);
                break;

            default:
                MainPage::Current().NotifyUser(
                    L"Unhandled result while toggling Mobile Hotspot: " + to_hstring(static_cast<int>(result.Status())), // TODO: stringify
                    NotifyType::ErrorMessage);
                break;
            }
        }
        m_toggling = false;
    }
}
