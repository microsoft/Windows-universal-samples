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
#include "Scenario5_SendData.xaml.h"

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

Scenario5::Scenario5() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario5::AddSocket(SocketWrapper^ socket)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, socket]()
        {
            lock_guard<mutex> lock(_mutex);

            ConnectedSockets->Items->Append(socket);
        }));
}

void Scenario5::RemoveSocket(unsigned int index)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, index]()
        {
            lock_guard<mutex> lock(_mutex);
            
            if (index > ConnectedSockets->Items->Size - 1)
            {
                throw ref new OutOfBoundsException("Attempted to remove socket no longer in list");
            }

            ConnectedSockets->Items->RemoveAt(index);
        }));
}

void Scenario5::OnNavigatedTo(NavigationEventArgs^ e)
{
    lock_guard<mutex> lock(_mutex);

    MainUI->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    PlaceholderUI->Visibility = Windows::UI::Xaml::Visibility::Visible;

    _session = WiFiDirectServiceManager::Instance->CurrentSession;

    if (_session != nullptr)
    {
        AdvertisementId->Text = _session->AdvertisementId.ToString();
        ServiceAddress->Text = _session->ServiceAddress;
        SessionId->Text = _session->SessionId.ToString();
        SessionAddress->Text = _session->SessionAddress;
        SessionStatus->Text = _session->Session->Status.ToString();

        ConnectedSockets->Items->Clear();
        for (auto&& socket : _session->SocketList)
        {
            ConnectedSockets->Items->Append(socket);
        }

        MainUI->Visibility = Windows::UI::Xaml::Visibility::Visible;
        PlaceholderUI->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    else
    {
        AdvertisementId->Text = "##";
        ServiceAddress->Text = "XX:XX:XX:XX:XX:XX";
        SessionId->Text = "##";
        SessionAddress->Text = "XX:XX:XX:XX:XX:XX";
        SessionStatus->Text = "???";

        ConnectedSockets->Items->Clear();
    }

    WiFiDirectServiceManager::Instance->CurrentScenario5 = this;
}

void Scenario5::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    WiFiDirectServiceManager::Instance->CurrentScenario5 = nullptr;
}

void Scenario5::AddPort_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (_session != nullptr)
    {
        unsigned short port = 0;
        if (Port->Text->Length() > 0)
        {
            wistringstream ss(Port->Text->Data());
            ss >> port;

            // Create socket asynchronously, will get callback when it is complete to add the socket to the list
            Concurrency::task<void> addPortTask;
            if ((dynamic_cast<ComboBoxItem^>(Protocol->SelectedItem))->Content->ToString() == "TCP")
            {
                addPortTask = _session->AddStreamSocketListenerAsync(port);
            }
            else
            {
                addPortTask = _session->AddDatagramSocketAsync(port);
            }

            addPortTask.then([this](Concurrency::task<void> previousTask)
            {
                try
                {
                    // Try getting all exceptions from the continuation chain above this point. 
                    previousTask.get();
                }
                catch (Exception^ ex)
                {
                    _rootPage->NotifyUser("Add Socket Failed: " + ex->Message, NotifyType::ErrorMessage);
                }
            });
        }
    }
}

void Scenario5::DoSend_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (ConnectedSockets->Items->Size > 0 && _session != nullptr)
    {
        int index = ConnectedSockets->SelectedIndex;
        if (index >= 0)
        {
            String^ message = SendData->Text;
            SendData->Text = "";

            _session->SocketList->GetAt(index)->SendMessageAsync(message)
            .then([this](Concurrency::task<void> previousTask)
            {
                try
                {
                    // Try getting all exceptions from the continuation chain above this point. 
                    previousTask.get();
                }
                catch (Exception^ ex)
                {
                    _rootPage->NotifyUser("SendMessageAsync Failed: " + ex->Message, NotifyType::ErrorMessage);
                }
            });
        }
    }
}
