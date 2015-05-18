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
#include "Scenario3_RegisterForUpdates.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::WiFiScan;

using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::WiFi;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

Scenario3_RegisterForUpdates::Scenario3_RegisterForUpdates() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario3_RegisterForUpdates::Button_Click_Register(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    _registrationToken = _firstAdapter->AvailableNetworksChanged +=
        ref new Windows::Foundation::TypedEventHandler<Windows::Devices::WiFi::WiFiAdapter ^, Platform::Object ^>(this, &WiFiScan::Scenario3_RegisterForUpdates::OnAvailableNetworksChanged);

    RegisterButton->IsEnabled = false;
    UnregisterButton->IsEnabled = true;
}

void Scenario3_RegisterForUpdates::Button_Click_Unregister(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    _firstAdapter->AvailableNetworksChanged -= _registrationToken;

    UnregisterButton->IsEnabled = false;
    RegisterButton->IsEnabled = true;
}

void Scenario3_RegisterForUpdates::OnNavigatedTo(NavigationEventArgs^ e)
{
    // RequestAccessAsync must have been called at least once by the app before using the API
    // Calling it multiple times is fine but not necessary
    // RequestAccessAsync must be called from the UI thread
    auto requestAccessOperation = WiFiAdapter::RequestAccessAsync();
    auto requestAccessTask = create_task(requestAccessOperation);

    requestAccessTask.then([this](WiFiAccessStatus access)
    {
        if (access != WiFiAccessStatus::Allowed)
        {
            ScenarioOutput->Text = "Access denied";
        }
        else
        {
            auto enumerationOperation = DeviceInformation::FindAllAsync(WiFiAdapter::GetDeviceSelector());
            auto enumerationTask = create_task(enumerationOperation);

            enumerationTask.then([this](DeviceInformationCollection^ result)
            {
                if (result->Size >= 1)
                {
                    auto firstAdapterOperation = WiFiAdapter::FromIdAsync(result->GetAt(0)->Id);
                    auto firstAdapterTask = create_task(firstAdapterOperation);

                    firstAdapterTask.then([this](WiFiAdapter^ adapter)
                    {
                        _firstAdapter = adapter;
                        RegisterButton->IsEnabled = true;
                    });
                }
                else
                {
                    ScenarioOutput->Text = L"No WiFi Adapters detected on this machine";
                }
            });
        }
    });
}

void Scenario3_RegisterForUpdates::DisplayNetworkReport(WiFiNetworkReport^ report)
{
    auto timestampString = DateTimeFormatter::LongDate->Format(report->Timestamp);
    auto message = L"Network Report Timestamp: " + timestampString;
    for (auto network : report->AvailableNetworks)
    {
        message += L"\nNetworkName: " + network->Ssid + L", BSSID: " + network->Bssid
            + L", RSSI: " + network->NetworkRssiInDecibelMilliwatts
            + L"dBm, Channel Frequency: " + network->ChannelCenterFrequencyInKilohertz
            + L"kHz";
    }

    // there is no guarantee of what thread the AvailableNetworksChanged callback is run on
    _rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, message]
        {
            ScenarioOutput->Text = message;
        }));
}

void Scenario3_RegisterForUpdates::OnAvailableNetworksChanged(Windows::Devices::WiFi::WiFiAdapter ^sender, Platform::Object ^args)
{
    DisplayNetworkReport(_firstAdapter->NetworkReport);
}
