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
#include "Tasks.h"
#include "UpdateMetadataTask.g.cpp"
#include "ConnectTask.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Devices::WiFi;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Security::Credentials;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::UI::Notifications;

namespace winrt::BackgroundTask::implementation
{
    // This task is triggered when the system wants us to refresh our hotspot metadata.
    // This typically happens when the system is about to show a list of available Wi-Fi networks.
    void UpdateMetadataTask::Run(IBackgroundTaskInstance const&)
    {
        // A real on-demand hotspot task would contact each hotspot device to            
        // obtain this information. This sample doesn't have a real hotspot device,
        // so it just repeats the information set by the UI portion of the app.

        IPropertySet values{ ApplicationData::Current().LocalSettings().Values() };
        std::optional<winrt::guid> hotspotGuid = values.TryLookup(L"hotspotGuid").try_as<winrt::guid>();
        if (!hotspotGuid.has_value())
        {
            // Nothing configured. 
            return;
        }

        WiFiOnDemandHotspotNetwork network = WiFiOnDemandHotspotNetwork::GetOrCreateById(hotspotGuid.value());
        WiFiOnDemandHotspotNetworkProperties properties = network.GetProperties();

        properties.Availability(values.Lookup(L"availability").as<bool>() ? WiFiOnDemandHotspotAvailability::Available : WiFiOnDemandHotspotAvailability::Unavailable);
        properties.DisplayName(values.Lookup(L"displayName").as<hstring>());
        std::optional<int> bars = values.TryLookup(L"bars").try_as<int>();
        properties.CellularBars(bars.has_value() ? std::optional(static_cast<WiFiOnDemandHotspotCellularBars>(bars.value())) : std::nullopt);
        properties.RemainingBatteryPercent(values.TryLookup(L"bars").try_as<uint32_t>());
        properties.Ssid(values.TryLookup(L"ssid").as<hstring>());

        PasswordCredential credential;
        credential.Password(values.TryLookup(L"password").as<hstring>());
        properties.Password(credential);

        network.UpdateProperties(properties);
    }

    // This task is triggered when the user selects our hotspot in the list of available Wi-Fi networks.
    // The task should turn on the Wi-Fi hotspot so that the system can connect to it.

    void ConnectTask::Run(IBackgroundTaskInstance const& taskInstance)
    {
        WiFiOnDemandHotspotConnectTriggerDetails triggerDetails = taskInstance.TriggerDetails().as<WiFiOnDemandHotspotConnectTriggerDetails>();

        // A real on-demand hotspot task would contact the hotspot device and tell it
        // to start advertising the SSID.
        //
        // This sample doesn't have a real hotspot device, so it will simulate the action
        // by displaying a toast to instruct the user to turn on the mobile hotspot manually.

        WiFiOnDemandHotspotNetwork network = triggerDetails.RequestedNetwork();

        // Is this the network we know how to turn on?
        IPropertySet values = ApplicationData::Current().LocalSettings().Values();
        if (values.TryLookup(L"hotspotGuid").try_as<winrt::guid>().value_or(winrt::guid{}) != network.Id())
        {
            triggerDetails.ReportError(WiFiOnDemandHotspotConnectStatus::UnspecifiedFailure);
            return;
        }

        // To demonstrate how to report errors turning on the hotspot, we will report an error
        // if the hotspot is configured with an SSID named "error".
        WiFiOnDemandHotspotNetworkProperties properties = network.GetProperties();
        hstring ssid = properties.Ssid();
        if (ssid == L"error")
        {
            // Simulate an error turning on the hotspot. You can report any error code from the
            // WiFiOnDemandHotspotConnectStatus enumeration.
            triggerDetails.ReportError(WiFiOnDemandHotspotConnectStatus::NetworkNotAvailable);
        }
        else
        {
            DisplayToast(ssid);
            Sleep(5000);

            WiFiOnDemandHotspotConnectionResult result = triggerDetails.Connect();
            values.Insert(L"connectResult", box_value(static_cast<int>(result.Status())));
        }
    }

    void ConnectTask::DisplayToast(const winrt::hstring& ssid)
    {
        ToastNotifier ToastNotifier = ToastNotificationManager::CreateToastNotifier();

        // Create a two line toast and add audio reminder

        // Here the xml that will be passed to the
        // ToastNotification for the toast is retrieved
        XmlDocument toastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastText02);

        winrt::hstring message(L"SSID: '" + ssid + L"'. Connecting in five seconds.");

        // Set both lines of text
        XmlNodeList toastNodeList = toastXml.GetElementsByTagName(L"text");
        toastNodeList.Item(0).AppendChild(toastXml.CreateTextNode(L"Please turn on your hotspot now!"));
        toastNodeList.Item(1).AppendChild(toastXml.CreateTextNode(message));

        // now create a xml node for the audio source
        IXmlNode toastNode = toastXml.SelectSingleNode(L"/toast");
        XmlElement audio = toastXml.CreateElement(L"audio");
        audio.SetAttribute(L"src", L"ms-winsoundevent:Notification.SMS");

        ToastNotification toast(toastXml);
        ToastNotifier.Show(toast);
    }
}
