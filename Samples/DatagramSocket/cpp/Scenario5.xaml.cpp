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
#include "Scenario5.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::DatagramSocketSample;

using namespace Platform;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario5::Scenario5() :
    listenerSocket(nullptr)
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario5::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
    MulticastRadioButton->IsChecked = true;
}

/// <summary>
/// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
/// </summary>
/// <param name="e">Event data representative of the navigation that will unload the current Page.</param>
void Scenario5::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
    CloseListenerSocket();
}

void Scenario5::CloseListenerSocket()
{
    if (listenerSocket != nullptr)
    {
        // DatagramSocket.Close() is exposed through the 'delete' keyword in C++/CX.
        // The call below explicitly closes the socket, freeing the UDP port that it is currently bound to.
        delete listenerSocket;
        listenerSocket = nullptr;
    }
}

// Sets up the UI to display the multicast scenario options.
void Scenario5::SetupMulticastScenarioUI()
{
    RemoteAddressLabel->Text = "Multicast Group:";
    StartListener->Content = "Start listener and join multicast group";
    RemoteAddress->Text = "224.3.0.5";
    RemoteAddress->IsEnabled = false;
    SendMessageButton->IsEnabled = false;
    CloseListenerButton->IsEnabled = false;
    SendOutput->Text = "";
}

// Sets up the UI to display the broadcast scenario options.
void Scenario5::SetupBroadcastScenarioUI()
{
    RemoteAddressLabel->Text = "Broadcast Address:";
    StartListener->Content = "Start listener";
    RemoteAddress->Text = "255.255.255.255";
    RemoteAddress->IsEnabled = false;
    SendMessageButton->IsEnabled = false;
    CloseListenerButton->IsEnabled = false;
    SendOutput->Text = "";
}

void Scenario5::MulticastRadioButton_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CloseListenerSocket();
    SetupMulticastScenarioUI();
}

void Scenario5::MulticastRadioButton_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CloseListenerSocket();
    SetupBroadcastScenarioUI();
}

/// <summary>
/// This is the click handler for the 'StartListener' button.
/// </summary>
/// <param name="sender">Object for which the event was generated.</param>
/// <param name="e">Event's parameters.</param>
void Scenario5::StartListener_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (ServiceName->Text == nullptr)
    {
        rootPage->NotifyUser("Please provide a service name.", NotifyType::ErrorMessage);
        return;
    }

    if (listenerSocket != nullptr)
    {
        rootPage->NotifyUser("A listener socket is already set up.", NotifyType::ErrorMessage);
        return;
    }

    bool isMulticastSocket = MulticastRadioButton->IsChecked->Value;
    listenerSocket = ref new DatagramSocket();
    listenerSocket->MessageReceived += ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(
        this,
        &Scenario5::MessageReceived);

    if (isMulticastSocket)
    {
        // DatagramSockets conduct exclusive (SO_EXCLUSIVEADDRUSE) binds by default, effectively blocking
        // any other UDP socket on the system from binding to the same local port. This is done to prevent
        // other applications from eavesdropping or hijacking a DatagramSocket's unicast traffic.
        //
        // Setting the MulticastOnly control option to 'true' enables a DatagramSocket instance to share its
        // local port with any Win32 sockets that are bound using SO_REUSEADDR/SO_REUSE_MULTICASTPORT and
        // with any other DatagramSocket instances that have MulticastOnly set to true. However, note that any
        // attempt to use a multicast-only DatagramSocket instance to send or receive unicast data will result
        // in an exception being thrown.
        //
        // This control option is particularly useful when implementing a well-known multicast-based protocol,
        // such as mDNS and UPnP, since it enables a DatagramSocket instance to coexist with other applications
        // running on the system that also implement that protocol.
        listenerSocket->Control->MulticastOnly = true;
    }

    // Start listen operation.
    create_task(listenerSocket->BindServiceNameAsync(ServiceName->Text)).then(
        [=](task<void> bindTask)
    {
        try
        {
            // Try getting an exception.
            bindTask.get();

            if (isMulticastSocket)
            {
                // Join the multicast group to start receiving datagrams being sent to that group.
                listenerSocket->JoinMulticastGroup(ref new HostName(RemoteAddress->Text));

                rootPage->NotifyUser(
                    "Listening on port " + listenerSocket->Information->LocalPort + " and joined to multicast group",
                    NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser(
                    "Listening on port " + listenerSocket->Information->LocalPort,
                    NotifyType::StatusMessage);
            }

            // Enable scenario steps that require us to have an active listening socket.
            SendMessageButton->IsEnabled = true;
            CloseListenerButton->IsEnabled = true;
        }
        catch (Exception^ exception)
        {
            delete listenerSocket;
            listenerSocket = nullptr;

            rootPage->NotifyUser(
                "Start listening failed with error: " + exception->Message,
                NotifyType::ErrorMessage);
        }
    });
}

/// <summary>
/// This is the click handler for the 'SendMessage' button.
/// </summary>
/// <param name="sender">Object for which the event was generated.</param>
/// <param name="e">Event's parameters.</param>
void Scenario5::SendMessage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    SendOutput->Text = "";
    HostName^ remoteHostname = ref new HostName(RemoteAddress->Text);

    // GetOutputStreamAsync can be called multiple times on a single DatagramSocket instance to obtain
    // IOutputStreams pointing to various different remote endpoints. The remote hostname given to
    // GetOutputStreamAsync can be a unicast, multicast or broadcast address.
    create_task(listenerSocket->GetOutputStreamAsync(remoteHostname, ServiceName->Text)).then(
        [&](task<IOutputStream^> getOutputStreamTask)
    {
        try
        {
            // Try getting an exception.
            IOutputStream^ outputStream = getOutputStreamTask.get();

            // Send out some multicast or broadcast data. Datagrams generated by the IOutputStream will use
            // <source host, source port> information obtained from the parent socket (i.e., 'listenSocket' in
            // this case).
            String^ stringToSend = "Hello";
            DataWriter^ writer = ref new DataWriter(outputStream);
            writer->WriteString(stringToSend);

            create_task(writer->StoreAsync()).then(
                [this, stringToSend](task<unsigned int> writeTask)
            {
                try
                {
                    // Try getting an exception.
                    writeTask.get();
                    SendOutput->Text = "\"" + stringToSend + "\" sent successfully";
                }
                catch (Exception^ exception)
                {
                    rootPage->NotifyUser("Send failed with error: " + exception->Message, NotifyType::ErrorMessage);
                }
            });
        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser("Send failed with error: " + exception->Message, NotifyType::ErrorMessage);
        }
    });
}

/// <summary>
/// This is the click handler for the 'CloseListener' button.
/// </summary>
/// <param name="sender">Object for which the event was generated.</param>
/// <param name="e">Event's parameters.</param>
void Scenario5::CloseListener_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CloseListenerSocket();

    // Disable scenario steps that require us to have an active listening socket.
    SendMessageButton->IsEnabled = false;
    CloseListenerButton->IsEnabled = false;
    SendOutput->Text = "";

    rootPage->NotifyUser("Listener closed", NotifyType::StatusMessage);
}

/// <summary>
/// Message received handler
/// </summary>
/// <param name="socket">The socket object</param>
/// <param name="eventArguments">The datagram event information</param>
void Scenario5::MessageReceived(
    Windows::Networking::Sockets::DatagramSocket^ socket,
    Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
    try
    {
        // Interpret the incoming datagram's entire contents as a string.
        unsigned int stringLength = eventArguments->GetDataReader()->UnconsumedBufferLength;
        String^ receivedMessage = eventArguments->GetDataReader()->ReadString(stringLength);

        NotifyUserFromAsyncThread(
            "Received data from remote peer (Remote Address: " +
            eventArguments->RemoteAddress->CanonicalName +
            ", Remote Port: " +
            eventArguments->RemotePort + "): \"" +
            receivedMessage + "\"",
            NotifyType::StatusMessage);
    }
    catch (Exception^ exception)
    {
        rootPage->NotifyUser(
            "Error happened when receiving a datagram:" + exception->Message,
            NotifyType::ErrorMessage);
    }
}

/// <summary>
/// Notifies the user from a non-UI thread
/// </summary>
/// <param name="message">The message</param>
/// <param name="type">The type of notification</param>
void Scenario5::NotifyUserFromAsyncThread(Platform::String^ message, NotifyType type)
{
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type]()
    {
        rootPage->NotifyUser(message, type);
    }));
}
