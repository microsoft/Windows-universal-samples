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
#include "Scenario2_AdvertiserAccept.xaml.h"

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

SessionRequestWrapper::SessionRequestWrapper(
    WiFiDirectServiceSessionRequest^ request,
    AdvertisementWrapper^ advertisement
    )
{
    Id = request->DeviceInformation->Id;
    Advertisement = advertisement;
    Pin = "";
    ProvisioningInfo = "GroupFormation? " + (request->ProvisioningInfo->IsGroupFormationNeeded ? "Yes" : "No")
        + ", ConfigMethod: " + request->ProvisioningInfo->SelectedConfigurationMethod.ToString();

    if (request->ProvisioningInfo->SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod::PinDisplay)
    {
        Pin = advertisement->Pin;
    }

    SessionInfo = request->SessionInfo;
}

Scenario2::Scenario2() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario2::AddSessionRequest(WiFiDirectServiceSessionRequest^ request, AdvertisementWrapper^ advertiser)
{
    SessionRequestWrapper^ sessionRequest = ref new SessionRequestWrapper(request, advertiser);
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, sessionRequest]() {
            lock_guard<mutex> lock(_mutex);
            SessionRequests->Items->Append(sessionRequest);
    }));
}

void Scenario2::RemoveSessionRequest(WiFiDirectServiceSessionRequest^ request, AdvertisementWrapper^ advertiser)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, request, advertiser]()
        {
            lock_guard<mutex> lock(_mutex);
            for (unsigned int i = 0; i < SessionRequests->Items->Size; ++i)
            {
                SessionRequestWrapper^ wrapper = dynamic_cast<SessionRequestWrapper^>(SessionRequests->Items->GetAt(i));
                if (wrapper->Advertisement->InternalId == advertiser->InternalId &&
                    wrapper->Id == request->DeviceInformation->Id)
                {
                    SessionRequests->Items->RemoveAt(i);
                    break;
                }
            }
        }));
}

void Scenario2::AddAdvertiser(AdvertisementWrapper ^ advertiser)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, advertiser]()
        {
            lock_guard<mutex> lock(_mutex);
            Advertisements->Items->Append(advertiser);
        }));
}

void Scenario2::RemoveAdvertiser(unsigned int index)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, index]()
        {
            lock_guard<mutex> lock(_mutex);
            
            if (index > Advertisements->Items->Size - 1)
            {
                throw ref new OutOfBoundsException("Attempted to remove advertiser no longer in list");
            }

            Advertisements->Items->RemoveAt(index);

            if (Advertisements->Items->Size == 0)
            {
                // Force navigation to the previous page for smoother experience
                // This takes us to the scenario to create a new advertisement
                _rootPage->GoToScenario(0);
            }
        }));
}

void Scenario2::AddSession(SessionWrapper ^ session)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, session]()
        {
            lock_guard<mutex> lock(_mutex);
            ConnectedSessions->Items->Append(session);
        }));
}

void Scenario2::RemoveSession(unsigned int index)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, index]()
        {
            lock_guard<mutex> lock(_mutex);
            
            if (index > ConnectedSessions->Items->Size - 1)
            {
                throw ref new OutOfBoundsException("Attempted to remove session no longer in list");
            }

            ConnectedSessions->Items->RemoveAt(index);
        }));
}

void Scenario2::UpdateAdvertiserStatus(AdvertisementWrapper^ advertiser)
{
    // Update list
}

void Scenario2::OnNavigatedTo(NavigationEventArgs^ e)
{
    lock_guard<mutex> lock(_mutex);

    MainUI->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    PlaceholderUI->Visibility = Windows::UI::Xaml::Visibility::Visible;

    // Fill in all existing data
    if (WiFiDirectServiceManager::Instance->Advertisers->Size > 0)
    {
        // Session Requests
        SessionRequests->Items->Clear();

        // Advertisers
        Advertisements->Items->Clear();
        for (auto&& advertiser : WiFiDirectServiceManager::Instance->Advertisers)
        {
            Advertisements->Items->Append(advertiser);

            for (auto&& request : advertiser->SessionRequestList)
            {
                SessionRequestWrapper^ sessionRequest = ref new SessionRequestWrapper(request, advertiser);
                SessionRequests->Items->Append(sessionRequest);
            }
        }

        // Connected Sessions
        ConnectedSessions->Items->Clear();
        for (auto&& session : WiFiDirectServiceManager::Instance->ConnectedSessionList)
        {
            ConnectedSessions->Items->Append(session);
        }

        MainUI->Visibility = Windows::UI::Xaml::Visibility::Visible;
        PlaceholderUI->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    WiFiDirectServiceManager::Instance->CurrentScenario2 = this;
}

void Scenario2::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    WiFiDirectServiceManager::Instance->CurrentScenario2 = nullptr;
}

void Scenario2::Accept_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (SessionRequests->Items->Size > 0)
    {
        int requestIndex = SessionRequests->SelectedIndex;
        if (requestIndex >= 0)
        {
            SessionRequestWrapper^ request = dynamic_cast<SessionRequestWrapper^>(SessionRequests->Items->GetAt(requestIndex));
            request->Advertisement->AcceptSessionRequest(request->Id, AcceptPin->Text);
        }
    }
}

void Scenario2::Decline_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (SessionRequests->Items->Size > 0)
    {
        int requestIndex = SessionRequests->SelectedIndex;
        if (requestIndex >= 0)
        {
            SessionRequestWrapper^ request = dynamic_cast<SessionRequestWrapper^>(SessionRequests->Items->GetAt(requestIndex));
            request->Advertisement->DeclineSessionRequest(request->Id);
        }
    }
}

void Scenario2::CloseSession_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (ConnectedSessions->Items->Size > 0)
    {
        int index = ConnectedSessions->SelectedIndex;
        if (index >= 0)
        {
            WiFiDirectServiceManager::Instance->CloseSession(index);
        }
    }
}

void Scenario2::SelectSession_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (ConnectedSessions->Items->Size > 0)
    {
        int index = ConnectedSessions->SelectedIndex;
        if (index >= 0)
        {
            WiFiDirectServiceManager::Instance->SelectSession(index);

            // Force navigation to the next page for smoother experience
            // This takes us to the scenario to send and receive data for a selected session
            _rootPage->GoToScenario(4);
        }
    }
}

void Scenario2::StopAdvertisement_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (Advertisements->Items->Size > 0)
    {
        int index = Advertisements->SelectedIndex;
        if (index >= 0)
        {
            WiFiDirectServiceManager::Instance->UnpublishService(index);
        }
    }
}
