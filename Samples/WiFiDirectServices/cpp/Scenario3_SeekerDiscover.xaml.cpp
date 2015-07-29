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
#include "Scenario3_SeekerDiscover.xaml.h"

using namespace SDKTemplate::WiFiDirectServices;

using namespace Platform;
using namespace Windows::Devices::WiFiDirect::Services;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace std;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario3::Scenario3() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario3::SessionConnected()
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this]()
        {
            // Force navigation to the next page for smoother experience
            _rootPage->GoToScenario(3);
        }));
}

void Scenario3::UpdateDiscoveryList(IVector<DiscoveredDeviceWrapper^>^ devices)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, devices]()
        {
            lock_guard<mutex> lock(_mutex);
            
            DiscoveredServices->Items->Clear();
            for (auto&& device : devices)
            {
                DiscoveredServices->Items->Append(device);
            }
        }));
}

void Scenario3::OnNavigatedTo(NavigationEventArgs^ e)
{
    lock_guard<mutex> lock(_mutex);
    _rootPage = MainPage::Current;

    // Cleanup past discoveries
    DiscoveredServices->Items->Clear();
    _provisioningInfo = nullptr;

    UpdateDiscoveryList(WiFiDirectServiceManager::Instance->DiscoveredDevices);
    WiFiDirectServiceManager::Instance->CurrentScenario3 = this;
}

void Scenario3::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    WiFiDirectServiceManager::Instance->CurrentScenario3 = nullptr;
}

void Scenario3::Discover_Click(Object^ sender, RoutedEventArgs^ e)
{
    WiFiDirectServiceManager::Instance->DiscoverServicesAsync(
        ServiceName->Text,
        RequestedServiceInfo->Text
        );
}

void Scenario3::GetProvisioningInfo_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (DiscoveredServices->Items->Size > 0)
    {
        int index = DiscoveredServices->SelectedIndex;
        if (index >= 0)
        {
            DiscoveredDeviceWrapper^ service = dynamic_cast<DiscoveredDeviceWrapper^>(DiscoveredServices->Items->GetAt(index));

            WiFiDirectServiceConfigurationMethod configMethod = WiFiDirectServiceConfigurationMethod::Default;
            auto selectedConfigMethod = dynamic_cast<ComboBoxItem^>(SelectedConfigMethod->SelectedItem);
            
            if (selectedConfigMethod->Content->ToString() == "WFDS Default")
            {
                configMethod = WiFiDirectServiceConfigurationMethod::Default;
            }
            else if (selectedConfigMethod->Content->ToString() == "PIN Keypad")
            {
                configMethod = WiFiDirectServiceConfigurationMethod::PinEntry;
            }
            else if (selectedConfigMethod->Content->ToString() == "PIN Display")
            {
                configMethod = WiFiDirectServiceConfigurationMethod::PinDisplay;
            }

            service->OpenSessionAsync()
            .then([this, service]()
            {
                return service->SetServiceOptionsAsync(
                    (PreferGO->IsChecked != nullptr) ? PreferGO->IsChecked->Value : false,
                    SessionInfo->Text
                    );
            }, Concurrency::task_continuation_context::use_current()).then([this, service, configMethod]()
            {
                return service->GetProvisioningInfoAsync(configMethod);
            }).then([this](WiFiDirectServiceProvisioningInfo^ provisioningInfo)
            {
                _provisioningInfo = provisioningInfo;
            }).then([this](Concurrency::task<void> previousTask)
            {
                try
                {
                    // Try getting all exceptions from the continuation chain above this point. 
                    previousTask.get();
                }
                catch (Exception^ ex)
                {
                    _rootPage->NotifyUser("GetProvisioningInfoAsync Failed: " + ex->Message, NotifyType::ErrorMessage);
                }
            }, Concurrency::task_continuation_context::use_current());
        }
    }
}

void Scenario3::DoConnect_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (DiscoveredServices->Items->Size > 0)
    {
        int index = DiscoveredServices->SelectedIndex;
        if (index >= 0)
        {
            DiscoveredDeviceWrapper^ service = dynamic_cast<DiscoveredDeviceWrapper^>(DiscoveredServices->Items->GetAt(index));

            service->SetServiceOptionsAsync(
                (PreferGO->IsChecked != nullptr) ? PreferGO->IsChecked->Value : false,
                SessionInfo->Text
                )
            .then([this, service]()
            {
                if (_provisioningInfo != nullptr &&
                    _provisioningInfo->IsGroupFormationNeeded &&
                    _provisioningInfo->SelectedConfigurationMethod != WiFiDirectServiceConfigurationMethod::Default)
                {
                    // These are the conditions under which a PIN is required
                    return service->ConnectAsync(ConnectPin->Text);
                }
                else
                {
                    // No PIN required
                    return service->ConnectAsync();
                }
            }, Concurrency::task_continuation_context::use_current()).then([this](Concurrency::task<void> previousTask)
            {
                try
                {
                    // Try getting all exceptions from the continuation chain above this point. 
                    previousTask.get();
                }
                catch (Exception^ ex)
                {
                    _rootPage->NotifyUser("ConnectAsync Failed: " + ex->Message, NotifyType::ErrorMessage);
                }
            }, Concurrency::task_continuation_context::use_current());
        }
    }
}
