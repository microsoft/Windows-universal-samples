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
// Scenario4.xaml.cpp
// Implementation of the Scenario4 class
//

#include "pch.h"
#include "Scenario4_PartialReadWrite.xaml.h"

using namespace concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Sockets;
using namespace Windows::Security::Cryptography::Certificates;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web;

Scenario4::Scenario4()
{
    InitializeComponent();
    UpdateVisualState();
}

void Scenario4::OnNavigatedFrom(NavigationEventArgs^ e)
{
    CloseSocket();
}

void Scenario4::UpdateVisualState()
{
    if (busy)
    {
        VisualStateManager::GoToState(this, "Busy", false);
    }
    else
    {
        bool connected = (messageWebSocket != nullptr);
        VisualStateManager::GoToState(this, connected ? "Connected" : "Disconnected", false);
    }
}

void Scenario4::SetBusy(bool value)
{
    busy = value;
    UpdateVisualState();
}

void Scenario4::OnConnect()
{
    SetBusy(true);
    ConnectAsync().then([this]()
    {
        SetBusy(false);
    });
}

task<void> Scenario4::ConnectAsync()
{
    if (ServerAddressField->Text->IsEmpty())
    {
        rootPage->NotifyUser("Please specify a URI to connect to", NotifyType::ErrorMessage);
        return task_from_result();
    }

    // Validating the URI is required since it was received from an untrusted source (user input).
    // The URI is validated by calling TryGetUri() that will return 'nullptr' for strings that are not
    // valid WebSocket URIs.
    // Note that when enabling the text box users may provide URIs to machines on the intrAnet or intErnet. In
    // these cases the app requires the "Private Networks (Client and Server)" or "Internet (Client)" capability respectively.
    Uri^ server = rootPage->TryGetUri(ServerAddressField->Text);
    if (!server)
    {
        return task_from_result();
    }

    messageWebSocket = ref new MessageWebSocket();
    messageWebSocket->Control->MessageType = SocketMessageType::Utf8;

    // To support receiving event notifications for partial messages, you must set this receive mode.
    // If you do not set this mode, you will only receive notifications for complete messages; which is
    // the default behavior. Setting to Partial allows us to process partial data as soon as it arrives,
    // as opposed to waiting until the EndOfMessage to process the entire data.
    messageWebSocket->Control->ReceiveMode = MessageWebSocketReceiveMode::PartialMessage;

    messageWebSocket->MessageReceived +=
        ref new TypedEventHandler<
        MessageWebSocket^,
        MessageWebSocketMessageReceivedEventArgs^>(this, &Scenario4::MessageReceived);
    messageWebSocket->Closed += ref new TypedEventHandler<IWebSocket^, WebSocketClosedEventArgs^>(this, &Scenario4::OnClosed);

    if (server->SchemeName == "wss")
    {
        // WARNING: Only test applications should ignore SSL errors.
        // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
        // attacks. (Although the connection is secure, the server is not authenticated.)
        // Note that not all certificate validation errors can be ignored.
        // In this case, we are ignoring these errors since the certificate assigned to the localhost
        // URI is self-signed and has subject name = fabrikam.com
        messageWebSocket->Control->IgnorableServerCertificateErrors->Append(ChainValidationResult::Untrusted);
        messageWebSocket->Control->IgnorableServerCertificateErrors->Append(ChainValidationResult::InvalidName);
    }

    AppendOutputLine("Connecting to " + server->DisplayUri + "...");

    return create_task(messageWebSocket->ConnectAsync(server))
        .then([this](task<void> previousTask)
    {
        try
        {
            // Reraise any exception that occurred in the task.
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            // Error happened during connect operation.
            delete messageWebSocket;
            messageWebSocket = nullptr;

            AppendOutputLine(MainPage::BuildWebSocketError(ex));
            AppendOutputLine(ex->Message);
            return;
        }

        rootPage->NotifyUser("Connected", NotifyType::StatusMessage);
    });
}

void Scenario4::OnSend()
{
    SetBusy(true);
    SendAsync().then([this]()
    {
        SetBusy(false);
    });
}

task<void> Scenario4::SendAsync()
{
    String^ message = InputField->Text;
    if (message->IsEmpty())
    {
        rootPage->NotifyUser("Please specify text to send", NotifyType::ErrorMessage);
        return task_from_result();
    }

    // Buffer any data we want to send.

    Windows::Storage::Streams::DataWriter^ messageWriter = ref new DataWriter();
    messageWriter->WriteString(message);
    IBuffer^ buffer = messageWriter->DetachBuffer();

    task<unsigned int> asyncTask;

    if (EndOfMessageCheckBox->IsChecked->Value == true)
    {
        AppendOutputLine("Sending end of message: " + message);
        asyncTask = create_task(messageWebSocket->SendFinalFrameAsync(buffer));
    }
    else
    {
        AppendOutputLine("Sending partial message: " + message);
        asyncTask = create_task(messageWebSocket->SendNonfinalFrameAsync(buffer));
    }

    return asyncTask.then([this](task<unsigned int> previousTask)
    {
        try
        {
            // Reraise any exception that occurred in the task.
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            AppendOutputLine(MainPage::BuildWebSocketError(ex));
            AppendOutputLine(ex->Message);
            return;
        }
    });
}

void Scenario4::MessageReceived(MessageWebSocket^ sender, MessageWebSocketMessageReceivedEventArgs^ args)
{
    // Dispatch the event to the UI thread so we can update UI.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        String^ partialOrCompleted = "Partial";

        if (args->IsMessageComplete)
        {
            partialOrCompleted = "Complete";
        }

        AppendOutputLine(partialOrCompleted + " message received; Type: " + args->MessageType.ToString());

        DataReader^ reader = args->GetDataReader();
        reader->UnicodeEncoding = UnicodeEncoding::Utf8;
        try
        {
            // Note that it may be possible for partial UTF8 messages to be split between a character if it
            // extends multiple bytes. We avoid this by using only ASCII characters in this example. 
            // Should your application use multi-byte characters, you will need to do checks for broken characters.
            String^ read = reader->ReadString(reader->UnconsumedBufferLength);
            AppendOutputLine(read);
        }
        catch (Exception^ ex)
        {
            AppendOutputLine(MainPage::BuildWebSocketError(ex));
            AppendOutputLine(ex->Message);
        }
        delete reader;
    }));
}

void Scenario4::OnDisconnect()
{
    SetBusy(true);
    rootPage->NotifyUser("Closing", NotifyType::StatusMessage);
    CloseSocket();
    SetBusy(false);
    rootPage->NotifyUser("Closed", NotifyType::StatusMessage);
}

// The method may be triggered remotely by the server sending unsolicited close frame or locally by Close()/delete operator.
void Scenario4::OnClosed(IWebSocket^ sender, WebSocketClosedEventArgs^ args)
{
    // Dispatch the event to the UI thread so we do not need to synchronize access to messageWebSocket.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, sender, args] ()
    {
        AppendOutputLine("Closed; Code: " + args->Code.ToString() + ", Reason: " + args->Reason);

        if (messageWebSocket == sender)
        {
            CloseSocket();
            UpdateVisualState();
        }
    }));
}

void Scenario4::CloseSocket()
{
    if (messageWebSocket != nullptr)
    {
        try
        {
            messageWebSocket->Close(1000, "Closed due to user request.");
        }
        catch (Exception^ ex)
        {
            AppendOutputLine(MainPage::BuildWebSocketError(ex));
            AppendOutputLine(ex->Message);
        }
        messageWebSocket = nullptr;
    }
}

void Scenario4::AppendOutputLine(String^ value)
{
    OutputField->Text += value + "\r\n";
}
