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
using namespace SDKTemplate::DatagramSocketSample;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2::Scenario2()
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
}

void Scenario2::ConnectSocket_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (ServiceNameForConnect->Text == nullptr)
    {
        rootPage->NotifyUser("Please provide a service name.", NotifyType::ErrorMessage);
        return;
    }

    // By default 'HostNameForConnect' is disabled and host name validation is not required. When enabling the text
    // box validating the host name is required since it was received from an untrusted source (user input).
    // The host name is validated by catching ArgumentExceptions thrown by the HostName constructor for
    // invalid input.
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

    if (CoreApplication::Properties->HasKey("clientSocket"))
    {
        rootPage->NotifyUser(
            "This step has already been executed. Please move to the next one.", 
            NotifyType::ErrorMessage);
        return;
    }

    DatagramSocket^ socket = ref new DatagramSocket();

    if (DontFragment->IsOn)
    {
        // Set the IP DF (Don't Fragment) flag.
        // This won't have any effect when running both client and server on localhost.
        // Refer to the DatagramSocketControl class' MSDN documentation for the full list of control options.
        socket->Control->DontFragment = true;
    }

    SocketContext^ socketContext = ref new SocketContext(rootPage, socket);
    socket->MessageReceived += ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(
        socketContext, 
        &SocketContext::OnMessage);

    // Events cannot be hooked up directly to the ScenarioInput2 object, as the object can fall out-of-scope and be
    // deleted. This would render any event hooked up to the object ineffective. The SocketContext guarantees that
    // both the socket and object that serves its events have the same lifetime.
    CoreApplication::Properties->Insert("clientSocket", socketContext);

    rootPage->NotifyUser("Connecting to: " + HostNameForConnect->Text, NotifyType::StatusMessage);

    // Connect to the server (by default, the listener we created in the previous step).
    create_task(socket->ConnectAsync(hostName, ServiceNameForConnect->Text)).then(
        [this, socketContext] (task<void> previousTask)
    {
        try
        {
            // Try getting an exception.
            previousTask.get();
            rootPage->NotifyUser("Connected", NotifyType::StatusMessage);
            socketContext->SetConnected();
        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser("Connect failed with error: " + exception->Message, NotifyType::ErrorMessage);
        }
    });
}

SocketContext::SocketContext(MainPage^ rootPage, DatagramSocket^ socket)
{
    this->rootPage = rootPage;
    this->socket = socket;
    this->connected = false;
}

SocketContext::~SocketContext()
{
    // A DatagramSocket can be closed in two ways:
    //  - explicitly: using the 'delete' keyword (listener is closed even if there are outstanding references to it).
    //  - implicitly: removing the last reference to it (i.e., falling out-of-scope).
    //
    // When a DatagramSocket is closed implicitly, it can take several seconds for the local UDP port being used
    // by it to be freed/reclaimed by the lower networking layers. During that time, other UDP sockets on the machine
    // will not be able to use the port. Thus, it is strongly recommended that DatagramSocket instances be explicitly
    // closed before they go out of scope(e.g., before application exit). The call below explicitly closes the socket.
    delete socket;
    socket = nullptr;

    if (writer != nullptr)
    {
        delete writer;
        writer = nullptr;
    }
}

void SocketContext::OnMessage(DatagramSocket^ socket, DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
    try
    {
        // Interpret the incoming datagram's entire contents as a string.
        unsigned int stringLength = eventArguments->GetDataReader()->UnconsumedBufferLength;
        String^ receivedMessage = eventArguments->GetDataReader()->ReadString(stringLength);

        NotifyUserFromAsyncThread(
            "Received data from remote peer: \"" + receivedMessage + "\"", 
            NotifyType::StatusMessage);
    }
    catch (Exception^ exception)
    {
        SocketErrorStatus socketError = SocketError::GetStatus(exception->HResult);
        if (socketError == SocketErrorStatus::ConnectionResetByPeer)
        {
            // This error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
            NotifyUserFromAsyncThread(
                "Peer does not listen on the specific port. Please make sure that you run step 1 first " +
                "or you have a server properly working on a remote server.", 
                NotifyType::ErrorMessage);
        }
        else if (socketError != SocketErrorStatus::Unknown)
        {
            NotifyUserFromAsyncThread(
                "Error happened when receiving a datagram: " + socketError.ToString(), 
                NotifyType::ErrorMessage);
        }
        else
        {
            throw;
        }
    }
}

DataWriter^ SocketContext::GetWriter()
{
    if (writer == nullptr)
    {
        writer = ref new DataWriter(socket->OutputStream);
    }

    return writer;
}

boolean SocketContext::IsConnected()
{
    return connected;
}

void SocketContext::SetConnected()
{
    connected = true;
}

void SocketContext::NotifyUserFromAsyncThread(String^ message, NotifyType type)
{
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type] ()
    {
        rootPage->NotifyUser(message, type);
    }));
}
