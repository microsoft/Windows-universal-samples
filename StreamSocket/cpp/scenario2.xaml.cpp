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

//
// Scenario2.xaml.cpp
// Implementation of the Scenario2 class
//

#include "pch.h"
#include "Scenario2.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::StreamSocketSample;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2::Scenario2() : adapter(nullptr)
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    if (CoreApplication::Properties->HasKey("serverAddress"))
    {
        Object^ serverAddressObject = CoreApplication::Properties->Lookup("serverAddress");
        String^ serverAddress = dynamic_cast<String^>(serverAddressObject);
        if (serverAddress)
        {
            HostNameForConnect->Text = serverAddress;
        }
    }

    adapter = nullptr;
    if (CoreApplication::Properties->HasKey("adapter"))
    {
        Object^ networkAdapter = CoreApplication::Properties->Lookup("adapter");
        adapter = dynamic_cast<NetworkAdapter^>(networkAdapter);
    }
}


void Scenario2::ConnectSocket_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Overriding the socket here is safe as it will be deleted once all references to it are gone. However, in many
    // cases this is a dangerous pattern to override data semi-randomly (each time user clicked the button) so we 
    // block it here.
    if (CoreApplication::Properties->HasKey("clientSocket"))
    {
        rootPage->NotifyUser(
            "This step has already been executed. Please move to the next one.", 
            NotifyType::ErrorMessage);
        return;
    }

    if (ServiceNameForConnect->Text == nullptr)
    {
        rootPage->NotifyUser("Please provide a service name.", NotifyType::ErrorMessage);
        return;
    }

    // By default 'HostNameForConnect' is disabled and host name validation is not required. When enabling the text
    // box validating the host name is required since it was received from an untrusted source (user input).
    // The host name is validated by catching InvalidArgumentExceptions thrown by the HostName constructor for invalid
    // input.
    // Note that when enabling the text box users may provide names for hosts on the intErnet that require the
    // "Internet (Client)" capability.
    HostName^ hostName;
    try
    {
        hostName = ref new HostName(HostNameForConnect->Text);
    }
    catch (InvalidArgumentException^ e)
    {
        rootPage->NotifyUser("Error: Invalid host name.", NotifyType::ErrorMessage);
        return;
    }

    StreamSocket^ socket = ref new StreamSocket();

    // Save the socket, so subsequent steps can use it.
    CoreApplication::Properties->Insert("clientSocket", socket);

    rootPage->NotifyUser("Connecting to: " + HostNameForConnect->Text, NotifyType::StatusMessage);

    if (adapter == nullptr)
    {
        // Connect to the server (in our case the listener we created in previous step).
        create_task(socket->ConnectAsync(
            hostName,
            ServiceNameForConnect->Text,
            SocketProtectionLevel::PlainSocket)).then([this] (task<void> previousTask)
        {
            try
            {
                // Try getting all exceptions from the continuation chain above this point.
                previousTask.get();
                rootPage->NotifyUser("Connected", NotifyType::StatusMessage);

                // Mark the socket as connected. We do not really care about the value of the property, the mere 
                // existance if it means that we are connected.
                CoreApplication::Properties->Insert("connected", nullptr);
            }
            catch (Exception^ exception)
            {
                rootPage->NotifyUser("Connect failed with error: " + exception->Message, NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        // Connect to the server (in our case the listener we created in previous step)
        // limiting traffic to the same adapter that the user specified in the previous step.
        // This option will be overridden by interfaces with weak-host or forwarding modes enabled.
        create_task(socket->ConnectAsync(
            hostName,
            ServiceNameForConnect->Text,
            SocketProtectionLevel::PlainSocket,
            adapter)).then([this] (task<void> previousTask)
        {
            try
            {
                // Try getting all exceptions from the continuation chain above this point.
                previousTask.get();
                rootPage->NotifyUser(
                    "Connected using network adapter" + adapter->NetworkAdapterId, 
                    NotifyType::StatusMessage);

                // Mark the socket as connected. We do not really care about the value of the property, the mere 
                // existance if it means that we are connected.
                CoreApplication::Properties->Insert("connected", nullptr);
            }
            catch (Exception^ exception)
            {
                rootPage->NotifyUser("Connect failed with error: " + exception->Message, NotifyType::ErrorMessage);
            }
        });
    }
}
