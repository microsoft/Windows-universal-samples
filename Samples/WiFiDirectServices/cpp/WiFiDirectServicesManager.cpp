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
#include "WiFiDirectServicesManager.h"

#include "Scenario1_AdvertiserCreate.xaml.h"
#include "Scenario2_AdvertiserAccept.xaml.h"
#include "Scenario3_SeekerDiscover.xaml.h"
#include "Scenario4_SeekerSession.xaml.h"
#include "Scenario5_SendData.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::WiFiDirectServices;
using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::WiFiDirect::Services;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace std;

WiFiDirectServiceManager^ WiFiDirectServiceManager::_instance = nullptr;

WiFiDirectServiceManager::WiFiDirectServiceManager()
{
    _rootPage = MainPage::Current;
}

WiFiDirectServiceManager^ WiFiDirectServiceManager::Instance::get()
{
    if (_instance == nullptr)
    {
        _instance = ref new WiFiDirectServiceManager();
    }
    return _instance;
}

MainPage^ WiFiDirectServiceManager::RootPage::get()
{
    if (_rootPage == nullptr)
    {
        _rootPage = MainPage::Current;
    }
    return _rootPage;
}

void WiFiDirectServiceManager::NotifyUser(String ^ strMessage, NotifyType type)
{
    // Always dispatch to UI thread
    _rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, strMessage, type]()
        {
            _rootPage->NotifyUser(strMessage, type);
        }));
}

void WiFiDirectServiceManager::StartAdvertisement(
    String^ serviceName,
    bool autoAccept,
    bool preferGO,
    String^ pin,
    IVector<WiFiDirectServiceConfigurationMethod>^ configMethods,
    WiFiDirectServiceStatus status,
    unsigned int customStatus,
    String^ serviceInfo,
    String^ deferredServiceInfo,
    IVector<String^>^ prefixList
    )
{
    // Create Advertiser object for the service
    // NOTE: service name is internally limited to up to 255 bytes in UTF-8 encoding
    // Valid characters include alpha-numeric, '.', '-', and any multi-byte character
    // characters a-z, A-Z are case-insensitive when discovering services
    WiFiDirectServiceAdvertiser^ advertiser = ref new WiFiDirectServiceAdvertiser(serviceName);

    // Auto-accept services will connect without interaction from advertiser
    // NOTE: if the config method used for a connection requires a PIN, then the advertiser will have to accept the connection
    advertiser->AutoAcceptSession = autoAccept;

    // Set the Group Owner intent to a large value so that the advertiser will try to become the group owner (GO)
    // NOTE: The GO of a P2P connection can connect to multiple clients while the client can connect to a single GO only
    advertiser->PreferGroupOwnerMode = preferGO;

    // Default status is "Available", but services may use a custom status code (value > 1) if applicable
    advertiser->ServiceStatus = status;
    advertiser->CustomServiceStatusCode = customStatus;

    // Service information can be up to 65000 bytes.
    // Service Seeker may explicitly discover this by specifying a short buffer that is a subset of this buffer.
    // If seeker portion matches, then entire buffer is returned, otherwise, the service information is not returned to the seeker
    // This sample uses a string for the buffer but it can be any data
    if (serviceInfo != nullptr && serviceInfo->Length() > 0)
    {
        auto serviceInfoDataWriter = ref new DataWriter(ref new InMemoryRandomAccessStream());
        serviceInfoDataWriter->WriteString(serviceInfo);
        advertiser->ServiceInfo = serviceInfoDataWriter->DetachBuffer();
    }
    else
    {
        advertiser->ServiceInfo = nullptr;
    }

    // This is a buffer of up to 144 bytes that is sent to the seeker in case the connection is "deferred" (i.e. not auto-accepted)
    // This buffer will be sent when auto-accept is false, or if a PIN is required to complete the connection
    // For the sample, we use a string, but it can contain any data
    if (deferredServiceInfo != nullptr && deferredServiceInfo->Length() > 0)
    {
        auto deferredSessionInfoDataWriter = ref new DataWriter(ref new InMemoryRandomAccessStream());
        deferredSessionInfoDataWriter->WriteString(deferredServiceInfo);
        advertiser->DeferredSessionInfo = deferredSessionInfoDataWriter->DetachBuffer();
    }
    else
    {
        advertiser->DeferredSessionInfo = nullptr;
    }

    // The advertiser supported configuration methods
    // Valid values are PIN-only (either keypad entry, display, or both), or PIN (keypad entry, display, or both) and WFD Services default
    // WFD Services Default config method does not require explicit PIN entry and offers a more seamless connection experience
    // Typically, an advertiser will support PIN display (and WFD Services Default), and a seeker will connect with either PIN entry or WFD Services Default
    if (configMethods != nullptr)
    {
        advertiser->PreferredConfigurationMethods->Clear();
        for (auto&& configMethod : configMethods)
        {
            advertiser->PreferredConfigurationMethods->Append(configMethod);
        }
    }

    // Advertiser may also be discoverable by a prefix of the service name. Must explicitly specify prefixes allowed here.
    if (prefixList != nullptr && prefixList->Size > 0)
    {
        advertiser->ServiceNamePrefixes->Clear();
        for (auto&& prefix : prefixList)
        {
            advertiser->ServiceNamePrefixes->Append(prefix);
        }
    }

    // For this sample, we wrap the advertiser in our own object which handles the advertiser events
    AdvertisementWrapper^ advertiserWrapper = ref new AdvertisementWrapper(advertiser, this, pin);

    AddAdvertiser(advertiserWrapper);

    RootPage->NotifyUser("Starting service...", NotifyType::StatusMessage);

    try
    {
        // This may fail if the driver is unable to handle the request or if services is not supported
        // NOTE: this must be called from the UI thread of the app
        advertiser->Start();
    }
    catch (Exception^ ex)
    {
        RootPage->NotifyUser("Failed to start service: " + ex->Message, NotifyType::ErrorMessage);
        throw;
    }
}

void WiFiDirectServiceManager::UnpublishService(unsigned int index)
{
    AdvertisementWrapper^ advertiser = nullptr;

    {
        lock_guard<mutex> lock(_managerMutex);
        if (index > _advertisers->Size - 1)
        {
            throw ref new OutOfBoundsException("Attempted to stop service not found in list");
        }

        advertiser = _advertisers->GetAt(index);
    }

    try
    {
        advertiser->Advertiser->Stop();
    }
    catch (Exception^ ex)
    {
        RootPage->NotifyUser("Stop Advertisement Failed: " + ex->Message, NotifyType::ErrorMessage);
    }
}

void WiFiDirectServiceManager::DiscoverServicesAsync(String^ serviceName, String^ requestedServiceInfo)
{
    RootPage->NotifyUser("Discover services... (name='" + serviceName + "', requestedInfo='" + requestedServiceInfo + "')", NotifyType::StatusMessage);

    // Clear old results
    _discoveredDevices->Clear();

    // Discovery depends on whether service information is requested
    String^ serviceSelector = "";
    if (requestedServiceInfo == "")
    {
        // Just search by name
        serviceSelector = WiFiDirectService::GetSelector(serviceName);
    }
    else
    {
        auto serviceInfoDataWriter = ref new DataWriter(ref new InMemoryRandomAccessStream());
        serviceInfoDataWriter->WriteString(requestedServiceInfo);
        // Discover by name and try to discover service information
        serviceSelector = WiFiDirectService::GetSelector(serviceName, serviceInfoDataWriter->DetachBuffer());
    }

    auto additionalProperties = ref new Vector<String^>();
    additionalProperties->Append("System.Devices.WiFiDirectServices.ServiceAddress");
    additionalProperties->Append("System.Devices.WiFiDirectServices.ServiceName");
    additionalProperties->Append("System.Devices.WiFiDirectServices.ServiceInformation");
    additionalProperties->Append("System.Devices.WiFiDirectServices.AdvertisementId");
    additionalProperties->Append("System.Devices.WiFiDirectServices.ServiceConfigMethods");

    // Note: This sample demonstrates finding services with FindAllAsync, which does a discovery and returns a list
    // It is also possible to use DeviceWatcher to receive updates as soon as services are found and to continue the discovery until it is stopped
    // See the DeviceWatcher sample for an example on how to use that class instead of DeviceInformation.FindAllAsync
    create_task(DeviceInformation::FindAllAsync(serviceSelector, additionalProperties)).then([this](DeviceInformationCollection^ deviceInfoCollection)
    {
        if (deviceInfoCollection != nullptr && deviceInfoCollection->Size > 0)
        {
            NotifyUser("Done discovering services, found " + deviceInfoCollection->Size + " services", NotifyType::StatusMessage);

            for (auto&& device : deviceInfoCollection)
            {
                _discoveredDevices->Append(ref new DiscoveredDeviceWrapper(device, this));
            }
        }
        else
        {
            NotifyUser("Done discovering services, No services found", NotifyType::StatusMessage);
        }

        // Update UI list
        if (_scenario3 != nullptr)
        {
            _scenario3->UpdateDiscoveryList(_discoveredDevices);
        }
    }).then([this](task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point. 
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            NotifyUser("FindAllAsync Failed: " + ex->Message, NotifyType::ErrorMessage);
        }
    });
}

void WiFiDirectServiceManager::RemoveAdvertiser(AdvertisementWrapper^ advertiser)
{
    unsigned int index = 0;
    bool fFound = false;

    {
        lock_guard<mutex> lock(_managerMutex);
        // Lookup the index
        for (auto&& a : _advertisers)
        {
            if (a->InternalId == advertiser->InternalId)
            {
                fFound = true;
                break;
            }
            ++index;
        }
    }

    if (fFound)
    {
        RemoveAdvertiser(index);
    }
    else
    {
        NotifyUser("Advertiser not found in list", NotifyType::ErrorMessage);
    }
}

void WiFiDirectServiceManager::AddSessionRequest(WiFiDirectServiceSessionRequest^ request, AdvertisementWrapper^ advertiser)
{
    // Update UI to add this to list
    if (_scenario2 != nullptr)
    {
        _scenario2->AddSessionRequest(request, advertiser);
    }
}

void WiFiDirectServiceManager::RemoveSessionRequest(WiFiDirectServiceSessionRequest^ request, AdvertisementWrapper^ advertiser)
{
    // Update UI to remove from list
    if (_scenario2 != nullptr)
    {
        _scenario2->RemoveSessionRequest(request, advertiser);
    }
}

void WiFiDirectServiceManager::AdvertiserStatusChanged(AdvertisementWrapper^ advertiser)
{
    if (advertiser->Advertiser->AdvertisementStatus == WiFiDirectServiceAdvertisementStatus::Aborted ||
        advertiser->Advertiser->AdvertisementStatus == WiFiDirectServiceAdvertisementStatus::Stopped)
    {
        RemoveAdvertiser(advertiser);
    }
    else
    {
        if (_scenario2 != nullptr)
        {
            _scenario2->UpdateAdvertiserStatus(advertiser);
        }
    }
}

void WiFiDirectServiceManager::AddSession(SessionWrapper^ session)
{
    {
        lock_guard<mutex> lock(_managerMutex);
        _connectedSessions->Append(session);
    }

    // Update UI to add this to list
    if (_scenario2 != nullptr)
    {
        _scenario2->AddSession(session);
    }
    if (_scenario4 != nullptr)
    {
        _scenario4->AddSession(session);
    }

    if (_scenario3 != nullptr)
    {
        _scenario3->SessionConnected();
    }
}

void WiFiDirectServiceManager::RemoveSession(unsigned int index)
{
    {
        lock_guard<mutex> lock(_managerMutex);
        // Invalidate selection
        _selectedSession = -1;
        _connectedSessions->RemoveAt(index);
    }

    // Update UI to remove from list
    if (_scenario2 != nullptr)
    {
        _scenario2->RemoveSession(index);
    }
    if (_scenario4 != nullptr)
    {
        _scenario4->RemoveSession(index);
    }
}

void WiFiDirectServiceManager::RemoveSession(SessionWrapper^ session)
{
    int index = 0;
    bool fFound = false;

    {
        lock_guard<mutex> lock(_managerMutex);
        // Lookup the index
        for (auto&& s : _connectedSessions)
        {
            if (s->Session->SessionId == session->Session->SessionId &&
                s->Session->SessionAddress == session->Session->SessionAddress &&
                s->Session->AdvertisementId == session->Session->AdvertisementId &&
                s->Session->ServiceAddress == session->Session->ServiceAddress)
            {
                fFound = true;
                break;
            }
            ++index;
        }
    }

    if (fFound)
    {
        RemoveSession(index);
    }
    else
    {
        NotifyUser("Session not found in list", NotifyType::ErrorMessage);
    }
}

void WiFiDirectServiceManager::CloseSession(unsigned int index)
{
    SessionWrapper^ session = nullptr;

    // First remove from list
    {
        lock_guard<mutex> lock(_managerMutex);
        if (index > _connectedSessions->Size - 1)
        {
            throw ref new OutOfBoundsException("Attempted to close session not found in list");
        }

        session = _connectedSessions->GetAt(index);
        // Session will remove self from list when its status changes
    }

    // Close session by disposing underlying WiFiDirectServiceSession
    session->Close();
    RootPage->NotifyUser("Closed Session", NotifyType::StatusMessage);
}

void WiFiDirectServiceManager::SelectSession(unsigned int index)
{
    {
        lock_guard<mutex> lock(_managerMutex);
        if (index > _connectedSessions->Size - 1)
        {
            throw ref new OutOfBoundsException("Attempted to select session not found in list");
        }
        _selectedSession = index;
    }
}

void WiFiDirectServiceManager::AddSocket(SocketWrapper^ socket)
{
    // Update UI to add this to list
    if (_scenario5 != nullptr)
    {
        _scenario5->AddSocket(socket);
    }
}

void WiFiDirectServiceManager::AddAdvertiser(AdvertisementWrapper^ advertiser)
{
    {
        lock_guard<mutex> lock(_managerMutex);
        _advertisers->Append(advertiser);
    }

    // Update UI to add this to list
    if (_scenario2 != nullptr)
    {
        _scenario2->AddAdvertiser(advertiser);
    }
}

void WiFiDirectServiceManager::RemoveAdvertiser(unsigned int index)
{
    {
        lock_guard<mutex> lock(_managerMutex);
        _advertisers->RemoveAt(index);
    }

    // Update UI to add this to list
    if (_scenario2 != nullptr)
    {
        _scenario2->RemoveAdvertiser(index);
    }
}
