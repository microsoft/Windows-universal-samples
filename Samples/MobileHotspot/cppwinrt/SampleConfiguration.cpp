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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::NetworkOperators;

namespace winrt::SDKTemplate
{
    hstring implementation::MainPage::FEATURE_NAME()
    {
        return L"MobileHotspot C++/WinRT Sample";
    }

    IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
    {
        Scenario{ L"Configure Mobile Hotspot", xaml_typename<SDKTemplate::Scenario1_ConfigureMobileHotspot>() },
        Scenario{ L"Toggle Mobile Hotspot", xaml_typename<SDKTemplate::Scenario2_ToggleMobileHotspot>() },
    });

    
    NetworkOperatorTetheringManager TryGetCurrentNetworkOperatorTetheringManager()
    {
        // Get the connection profile associated with the internet connection currently used by the local machine.
        ConnectionProfile currentConnectionProfile = NetworkInformation::GetInternetConnectionProfile();
        if (currentConnectionProfile == nullptr)
        {
            MainPage::Current().NotifyUser(L"System is not connected to the Internet.", NotifyType::ErrorMessage);
            return nullptr;
        }

        TetheringCapability tetheringCapability =
            NetworkOperatorTetheringManager::GetTetheringCapabilityFromConnectionProfile(currentConnectionProfile);

        if (tetheringCapability != TetheringCapability::Enabled)
        {
            hstring message;
            switch (tetheringCapability)
            {
            case TetheringCapability::DisabledByGroupPolicy:
                message = L"Tethering is disabled due to group policy.";
                break;
            case TetheringCapability::DisabledByHardwareLimitation:
                message = L"Tethering is not available due to hardware limitations.";
                break;
            case TetheringCapability::DisabledByOperator:
                message = L"Tethering operations are disabled for this account by the network operator.";
                break;
            case TetheringCapability::DisabledByRequiredAppNotInstalled:
                message = L"An application required for tethering operations is not available.";
                break;
            case TetheringCapability::DisabledBySku:
                message = L"Tethering is not supported by the current account services.";
                break;
            case TetheringCapability::DisabledBySystemCapability:
                // This will occur if the "wiFiControl" capability is missing from the App.
                message = L"This app is not configured to access Wi-Fi devices on this machine.";
                break;
            default:
                message = L"Tethering is disabled on this machine. (Code " + to_hstring(static_cast<int32_t>(tetheringCapability)) + L").";
                break;
            }
            MainPage::Current().NotifyUser(message, NotifyType::ErrorMessage);
            return nullptr;
        }

        try
        {
            return NetworkOperatorTetheringManager::CreateFromConnectionProfile(currentConnectionProfile);
        }
        catch (hresult_error const& ex)
        {
            if (ex.code() == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
            {
                MainPage::Current().NotifyUser(L"System has no Wi-Fi adapters.", NotifyType::ErrorMessage);
                return nullptr;
            }
            throw;
        }
    }

    hstring GetFriendlyName(TetheringWiFiBand value)
    {
        switch (value)
        {
        case TetheringWiFiBand::Auto: return L"Any available";
        case TetheringWiFiBand::TwoPointFourGigahertz: return L"2.4 GHz";
        case TetheringWiFiBand::FiveGigahertz: return L"5 Ghz";
        default: return L"Unknown (" + to_hstring(static_cast<uint32_t>(value)) + L")";
        }
    }

    // Calls NetworkOperatorTetheringAccessPointConfiguration.IsBandSupported but handles
    // certain boundary cases.

    bool IsBandSupported(NetworkOperatorTetheringAccessPointConfiguration const& configuration, TetheringWiFiBand band)
    {
        // "Auto" mode is always supported even though it is technically not a frequency band.
        if (band == TetheringWiFiBand::Auto)
        {
            return true;
        }

        try
        {
            return configuration.IsBandSupported(band);
        }
        catch (hresult_error const& ex)
        {
            if (ex.code() == HRESULT_FROM_WIN32(ERROR_INVALID_STATE))
            {
                // The WiFi device has been disconnected. Report that we support nothing.
                return false;
            }
            throw;
        }
    }
}
