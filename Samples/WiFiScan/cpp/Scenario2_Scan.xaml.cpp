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
using namespace Platform::Collections;
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

Scenario2_Scan::Scenario2_Scan() : _rootPage(MainPage::Current)
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
    ResultCollection = ref new Vector<WiFiNetworkDisplay^>();
    DataContext = this;

    // RequestAccessAsync must have been called at least once by the app before using the API
    // Calling it multiple times is fine but not necessary
    // RequestAccessAsync must be called from the UI thread
    auto requestAccessOperation = WiFiAdapter::RequestAccessAsync();
    auto requestAccessTask = create_task(requestAccessOperation);

    requestAccessTask.then([this](WiFiAccessStatus access)
    {
        if (access != WiFiAccessStatus::Allowed)
        {
            _rootPage->NotifyUser(L"Access denied", SDKTemplate::NotifyType::ErrorMessage);
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

                        ScanButton->IsEnabled = true;
                    });
                }
                else
                {
                    _rootPage->NotifyUser(L"No WiFi Adapters detected on this machine", SDKTemplate::NotifyType::ErrorMessage);
                }
            });
        }
    });
}

void Scenario2_Scan::DisplayNetworkReport(WiFiNetworkReport^ report)
{
    auto timestampString = DateTimeFormatter::LongDate->Format(report->Timestamp);
    auto message = L"Network Report Timestamp: " + timestampString;
    // there is no guarantee of what thread the AvailableNetworksChanged callback is run on
    _rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, message]
    {
        _rootPage->NotifyUser(message, SDKTemplate::NotifyType::StatusMessage);
    }));

    ResultCollection->Clear();
    for (auto network : report->AvailableNetworks)
    {
        ResultCollection->Append(ref new WiFiNetworkDisplay(network, _firstAdapter));
    }
}
