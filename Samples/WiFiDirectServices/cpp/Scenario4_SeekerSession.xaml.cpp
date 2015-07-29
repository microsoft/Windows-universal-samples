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
#include "Scenario4_SeekerSession.xaml.h"

using namespace SDKTemplate::WiFiDirectServices;

using namespace Platform;
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

Scenario4::Scenario4() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario4::AddSession(SessionWrapper^ session)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, session]()
        {
            lock_guard<mutex> lock(_mutex);
            
            ConnectedSessions->Items->Append(session);
        }));
}

void Scenario4::RemoveSession(unsigned int index)
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

void Scenario4::OnNavigatedTo(NavigationEventArgs^ e)
{
    lock_guard<mutex> lock(_mutex);

    MainUI->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    PlaceholderUI->Visibility = Windows::UI::Xaml::Visibility::Visible;

    // Cleanup old entries
    ConnectedSessions->Items->Clear();
    for (auto&& session : WiFiDirectServiceManager::Instance->ConnectedSessionList)
    {
        ConnectedSessions->Items->Append(session);
    }

    if (ConnectedSessions->Items->Size > 0)
    {
        MainUI->Visibility = Windows::UI::Xaml::Visibility::Visible;
        PlaceholderUI->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }

    WiFiDirectServiceManager::Instance->CurrentScenario4 = this;
}

void Scenario4::OnNavigatingFrom(NavigatingCancelEventArgs ^ e)
{
    WiFiDirectServiceManager::Instance->CurrentScenario4 = nullptr;
}

void Scenario4::SelectSession_Click(Object^ sender, RoutedEventArgs^ e)
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

void Scenario4::CloseSession_Click(Object^ sender, RoutedEventArgs^ e)
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
