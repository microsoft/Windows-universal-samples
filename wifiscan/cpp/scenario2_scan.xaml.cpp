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
#include "Scenario2_Scan.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::WiFiScan;

using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::WiFi;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

Scenario2_Scan::Scenario2_Scan()
{
    InitializeComponent();
}

void Scenario2_Scan::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto scanOperation = _firstAdapter->ScanAsync();
    auto scanTask = create_task(scanOperation);

    scanTask.then([this]()
    {
        DisplayNetworkReport(_firstAdapter->NetworkReport);
    });
}

void Scenario2_Scan::OnNavigatedTo(NavigationEventArgs^ e)
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

                        auto button = ref new Button();
                        button->Content = L"Scan";
                        button->Click += ref new RoutedEventHandler(this, &Scenario2_Scan::Button_Click);
                        Buttons->Children->Append(button);
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

void Scenario2_Scan::DisplayNetworkReport(WiFiNetworkReport^ report)
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
    ScenarioOutput->Text = message;
}
