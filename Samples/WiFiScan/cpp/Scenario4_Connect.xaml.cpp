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
#include "Scenario4_Connect.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::WiFiScan;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::WiFi;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Security::Credentials;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

Scenario4_Connect::Scenario4_Connect() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario4_Connect::OnNavigatedTo(NavigationEventArgs^ e)
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
                        DisconnectButton->IsEnabled = true;
                        UpdateConnectivityStatus();
                        NetworkInformation::NetworkStatusChanged += ref new NetworkStatusChangedEventHandler(this, &SDKTemplate::WiFiScan::Scenario4_Connect::OnNetworkStatusChanged);
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

void Scenario4_Connect::DisplayNetworkReport(WiFiNetworkReport^ report)
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

void Scenario4_Connect::ScanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto scanOperation = _firstAdapter->ScanAsync();
    auto scanTask = create_task(scanOperation);

    ConnectionBar->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    scanTask.then([this]()
    {
        DisplayNetworkReport(_firstAdapter->NetworkReport);
        WiFiNetworkDisplay::UpdateConnectivityLevels(ResultCollection, 0);
    });
}

void Scenario4_Connect::DisconnectButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    _firstAdapter->Disconnect();
}

void Scenario4_Connect::UpdateConnectivityStatus()
{
    auto getConnectedProfileOperation = _firstAdapter->NetworkAdapter->GetConnectedProfileAsync();
    auto getConnectedProfileTask = create_task(getConnectedProfileOperation);

    getConnectedProfileTask.then([this](ConnectionProfile^ connectedProfile)
    {
        if (connectedProfile != nullptr && _savedProfileName != connectedProfile->ProfileName)
        {
            _savedProfileName = connectedProfile->ProfileName;
            _rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
                ref new DispatchedHandler([this, connectedProfile]
            {
                _rootPage->NotifyUser("WiFi adapter connected to: " + connectedProfile->ProfileName +
                    " (" + WiFiNetworkDisplay::GetConnectivityLevelAsString(connectedProfile->GetNetworkConnectivityLevel()) + ")",
                    SDKTemplate::NotifyType::StatusMessage);
            }));
        }
        else  if (connectedProfile == nullptr && _savedProfileName != nullptr)
        {
            _savedProfileName = nullptr;
            _rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
                ref new DispatchedHandler([this, connectedProfile]
            {
                _rootPage->NotifyUser("WiFi adapter disconnected", SDKTemplate::NotifyType::StatusMessage);
            }));
        }
    });
}

void Scenario4_Connect::OnNetworkStatusChanged(Platform::Object^ sender)
{
    UpdateConnectivityStatus();

    // Since the network status was changed, update the connectivity level displayed
    WiFiNetworkDisplay::UpdateConnectivityLevels(ResultCollection, 0);
}

void Scenario4_Connect::ResultsListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    WiFiNetworkDisplay^ selectedNetwork = reinterpret_cast<WiFiNetworkDisplay^>(ResultsListView->SelectedItem);
    if (selectedNetwork == nullptr)
    {
        return;
    }

    // Show the connection bar
    ConnectionBar->Visibility = Windows::UI::Xaml::Visibility::Visible;

    // Only show the password box if needed
    if (selectedNetwork->AvailableNetwork->SecuritySettings->NetworkAuthenticationType == NetworkAuthenticationType::Open80211 &&
        selectedNetwork->AvailableNetwork->SecuritySettings->NetworkEncryptionType == NetworkEncryptionType::None)
    {
        NetworkKeyInfo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    else
    {
        NetworkKeyInfo->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}


void Scenario4_Connect::ConnectButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto selectedNetwork = reinterpret_cast<WiFiNetworkDisplay^>(ResultsListView->SelectedItem);

    if (selectedNetwork == nullptr || _firstAdapter == nullptr)
    {
        _rootPage->NotifyUser(L"Network not selcted", NotifyType::ErrorMessage);
        return;
    }
    WiFiReconnectionKind reconnectionKind = WiFiReconnectionKind::Manual;
    if (IsAutomaticReconnection->IsChecked->Value)
    {
        reconnectionKind = WiFiReconnectionKind::Automatic;
    }

    IAsyncOperation<WiFiConnectionResult^>^ connectOperation;

    if (selectedNetwork->AvailableNetwork->SecuritySettings->NetworkAuthenticationType == NetworkAuthenticationType::Open80211 &&
        selectedNetwork->AvailableNetwork->SecuritySettings->NetworkEncryptionType == NetworkEncryptionType::None)
    {
        connectOperation = _firstAdapter->ConnectAsync(selectedNetwork->AvailableNetwork, reconnectionKind);
    }
    else
    {
        // Only the password portion of the credential need to be supplied
        PasswordCredential^ credential = ref new PasswordCredential();

        // Make sure Credential.Password property is not set to an empty string. 
        // Otherwise, a System.ArgumentException will be thrown.
        // The default empty password string will still be passed to the ConnectAsync method,
        // which should return an "InvalidCredential" error
        if (!NetworkKey->Password->IsEmpty())
        {
            credential->Password = NetworkKey->Password;
        }

        connectOperation = _firstAdapter->ConnectAsync(selectedNetwork->AvailableNetwork, reconnectionKind, credential);
    }

    auto connectTask = create_task(connectOperation);
    connectTask.then([this, selectedNetwork](WiFiConnectionResult^ result)
    {
        if (result->ConnectionStatus == WiFiConnectionStatus::Success)
        {
            _rootPage->NotifyUser(L"Successfully connected to " + selectedNetwork->Ssid, NotifyType::StatusMessage);
        }
        else
        {
            _rootPage->NotifyUser(L"Failed to connect to " + selectedNetwork->Ssid + " ConnectionStatus = " +
                WiFiNetworkDisplay::GetConnectionStatusAsString(result->ConnectionStatus), NotifyType::ErrorMessage);
        }

    });

}
