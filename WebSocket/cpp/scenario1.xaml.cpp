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
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include <strsafe.h>
#include "Scenario1.xaml.h"

using namespace concurrency;
using namespace SDKTemplate::WebSocket;
using namespace Windows::UI::Core;

Scenario1::Scenario1()
{
    InitializeComponent();
}

Scenario1::~Scenario1()
{
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void SDKTemplate::WebSocket::Scenario1::Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Make a local copy to avoid races with Closed events.
    MessageWebSocket^ webSocket = messageWebSocket;

    if (InputField->Text == "")
    {
        rootPage->NotifyUser("Please specify text to send", NotifyType::ErrorMessage);
        return;
    }

    // Have we connected yet?
    if (webSocket == nullptr)
    {
        // Validating the URI is required since it was received from an untrusted source (user input).
        // The URI is validated by calling TryGetUri() that will return 'false' for strings that are not
        // valid WebSocket URIs.
        // Note that when enabling the text box users may provide URIs to machines on the intrAnet or intErnet. In
        // these cases the app requires the "Home or Work Networking" or "Internet (Client)" capability respectively.
        Uri^ server;
        if (!rootPage->TryGetUri(ServerAddressField->Text, &server))
        {
            return;
        }

        rootPage->NotifyUser("Connecting to: " + server->DisplayUri, NotifyType::StatusMessage);

        webSocket = ref new MessageWebSocket();
        webSocket->Control->MessageType = SocketMessageType::Utf8;
        webSocket->MessageReceived += ref new TypedEventHandler<MessageWebSocket^, MessageWebSocketMessageReceivedEventArgs^>(this, &Scenario1::MessageReceived);
        webSocket->Closed += ref new TypedEventHandler<IWebSocket^, WebSocketClosedEventArgs^>(this, &Scenario1::Closed);

        task<void>(webSocket->ConnectAsync(server)).then([this, webSocket] (task<void> previousTask)
        {
            try
            {
                // Try getting all exceptions from the continuation chain above this point.
                previousTask.get();

                messageWebSocket = webSocket; // Only store it after successfully connecting.
                messageWriter = ref new DataWriter(webSocket->OutputStream);

                rootPage->NotifyUser("Connected", NotifyType::StatusMessage);
                SendMessage();
            }
            catch (Exception^ exception)
            {
                HandleException(exception);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Already connected", NotifyType::StatusMessage);
        SendMessage();
    }
}

void SDKTemplate::WebSocket::Scenario1::HandleException(Exception^ exception)
{
    WebErrorStatus status = WebSocketError::GetStatus(exception->HResult);

    if (status == WebErrorStatus::CannotConnect ||
        status == WebErrorStatus::NotFound ||
        status == WebErrorStatus::RequestTimeout)
    {
        rootPage->NotifyUser("Cannot connect to the server. Please make sure "
            "to run the server setup script before running the sample.", NotifyType::ErrorMessage);
    }
    else
    {
        rootPage->NotifyUser("Error: " + status.ToString(), NotifyType::ErrorMessage);
    }

    OutputField->Text += exception->Message + "\r\n";
}

void SDKTemplate::WebSocket::Scenario1::SendMessage()
{
    String^ message = InputField->Text;

    OutputField->Text += "Sending Message:\r\n" + message + "\r\n";

    // Buffer any data we want to send.
    messageWriter->WriteString(message);

    // Send the data as one complete message.
    task<unsigned int>(messageWriter->StoreAsync()).then([this] (unsigned int)
    {
        rootPage->NotifyUser("Send Complete", NotifyType::StatusMessage);
    }).then([this] (task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();
        }
        catch (Exception^ exception)
        {
            HandleException(exception);
        }
    });
}

void SDKTemplate::WebSocket::Scenario1::MessageReceived(MessageWebSocket^ sender, MessageWebSocketMessageReceivedEventArgs^ args)
{
    MarshalText(OutputField, "Message Received; Type: " + args->MessageType.ToString() + "\r\n");
    DataReader^ reader = args->GetDataReader();
    reader->UnicodeEncoding = UnicodeEncoding::Utf8;

    String^ read = reader->ReadString(reader->UnconsumedBufferLength);
    MarshalText(OutputField, read + "\r\n");
}

void SDKTemplate::WebSocket::Scenario1::Close_Click(Object^ sender, RoutedEventArgs^ e)
{
    try
    {
        if (messageWebSocket != nullptr)
        {
            rootPage->NotifyUser("Closing", NotifyType::StatusMessage);
            messageWebSocket->Close(1000, "Closed due to user request.");
            messageWebSocket = nullptr;
        }
        else
        {
            rootPage->NotifyUser("No active WebSocket, send something first", NotifyType::StatusMessage);
        }
    }
    catch (Exception^ exception)
    {
        HandleException(exception);
    }
}

void SDKTemplate::WebSocket::Scenario1::Closed(IWebSocket^ sender, WebSocketClosedEventArgs^ args)
{
    // The method may be triggered remotely by the server sending unsolicited close frame or locally by Close()/delete operator.
    // Dispatch the event to the UI thread so we do not need to synchronize access to messageWebSocket.
    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args] ()
    {
        OutputField->Text += "Closed; Code: " + args->Code.ToString() + ", Reason: " + args->Reason + "\r\n";

        if (messageWebSocket != nullptr)
        {
            delete messageWebSocket;
            messageWebSocket = nullptr;
        }
    }));
}

void SDKTemplate::WebSocket::Scenario1::MarshalText(TextBox^ output, String^ value)
{
    MarshalText(output, value, true);
}

// When operations happen on a background thread we have to marshal UI updates back to the UI thread.
void SDKTemplate::WebSocket::Scenario1::MarshalText(TextBox^ output, String^ value, bool append)
{
    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, output, value, append] ()
    {
        if (append)
        {
            output->Text += value;
        }
        else
        {
            output->Text = value;
        }
    }));
}

