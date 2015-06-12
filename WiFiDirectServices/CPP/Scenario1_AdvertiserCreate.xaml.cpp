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
#include "Scenario1_AdvertiserCreate.xaml.h"

using namespace SDKTemplate::WiFiDirectServices;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::WiFiDirect::Services;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace std;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1::Scenario1() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}


void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetFields();
    WiFiDirectServiceManager::Instance->CurrentScenario1 = this;
}

void Scenario1::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    WiFiDirectServiceManager::Instance->CurrentScenario1 = nullptr;
}

void Scenario1::ResetFields()
{
    ServiceName->Text = "";
    AutoAccept->IsChecked = true;
    PreferGO->IsChecked = true;
    Status->SelectedIndex = 0;
    CustomStatus->Text = "";
    ConfigMethodDefault->IsChecked = true;
    ConfigMethodDispaly->IsChecked = true;
    ConfigMethodDisplayPin->Text = "";
    ConfigMethodKeypad->IsChecked = false;
    DeferredSessionInfo->Text = "";
    ServiceInformation->Text = "";
    ServicePrefixList->Items->Clear();
}

void Scenario1::CreateAdvertiser_Click(Object^ sender, RoutedEventArgs^ e)
{
    _rootPage->NotifyUser("Creating Advertisement", NotifyType::StatusMessage);

    try
    {
        auto configMethods = ref new Vector<WiFiDirectServiceConfigurationMethod>();

        if (ConfigMethodDispaly->IsChecked != nullptr && ConfigMethodDispaly->IsChecked->Value)
        {
            configMethods->Append(WiFiDirectServiceConfigurationMethod::PinDisplay);
        }
        if (ConfigMethodKeypad->IsChecked != nullptr && ConfigMethodKeypad->IsChecked->Value)
        {
            configMethods->Append(WiFiDirectServiceConfigurationMethod::PinEntry);
        }
        if (ConfigMethodDefault->IsChecked != nullptr && ConfigMethodDefault->IsChecked->Value)
        {
            configMethods->Append(WiFiDirectServiceConfigurationMethod::Default);
        }

        WiFiDirectServiceStatus status = WiFiDirectServiceStatus::Available;
        if (Status->SelectedItem->ToString() == "Busy")
        {
            status = WiFiDirectServiceStatus::Busy;
        }
        else if (Status->SelectedItem->ToString() == "Custom")
        {
            status = WiFiDirectServiceStatus::Custom;
        }

        unsigned int customStatus = 0;
        if (CustomStatus->Text->Length() > 0)
        {
            wistringstream ss(CustomStatus->Text->Data());
            ss >> customStatus;
        }

        // NOTE: Each prefix published will be handled by the driver so there are limitations
        // to how many prefixes can be supported. In general, apps should only publish prefixes
        // of the service name that are required for discovery
        // Typically, this would be something like "com.example.games" for a service called "com.example.games.foobar"
        auto prefixList = ref new Vector<String^>();
        for (auto&& prefix : ServicePrefixList->Items)
        {
            // NOTE: these must be actual prefixes of the service name
            // Publication will fail if they are not
            prefixList->Append(dynamic_cast<String^>(static_cast<Object^>(prefix)));
        }

        WiFiDirectServiceManager::Instance->StartAdvertisement(
            ServiceName->Text,
            (AutoAccept->IsChecked != nullptr) ? AutoAccept->IsChecked->Value : false,
            (PreferGO->IsChecked != nullptr) ? PreferGO->IsChecked->Value : false,
            ConfigMethodDisplayPin->Text,
            configMethods,
            status,
            customStatus,
            ServiceInformation->Text,
            DeferredSessionInfo->Text,
            prefixList
            );

        ResetFields();

        // Force navigation to the next page for smoother experience
        _rootPage->GoToScenario(1);
    }
    catch (Exception^ ex)
    {
        _rootPage->NotifyUser("Failed to start service: " + ex->Message, NotifyType::ErrorMessage);
    }
}

void Scenario1::RemovePrefix_Click(Object^ sender, RoutedEventArgs^ e)
{
    for (auto&& prefix : ServicePrefixList->SelectedItems)
    {
        for (unsigned int i = 0; i < ServicePrefixList->Items->Size; ++i)
        {
            String^ current = dynamic_cast<String^>(ServicePrefixList->Items->GetAt(i));
            if (current != nullptr && current == dynamic_cast<String^>(static_cast<Object^>(prefix)))
            {
                ServicePrefixList->Items->RemoveAt(i);
                // Quick fix to handle duplicates by walking the list again
                i = 0;
            }
        }
    }
}

void Scenario1::AddPrefix_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (NewPrefix->Text != "")
    {
        ServicePrefixList->Items->Append(NewPrefix->Text);
        NewPrefix->Text = "";
    }
}
