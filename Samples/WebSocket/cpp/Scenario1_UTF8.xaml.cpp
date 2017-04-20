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
#include "Scenario1_UTF8.xaml.h"

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

Scenario1::Scenario1()
{
    InitializeComponent();
    UpdateVisualState();
}

void Scenario1::OnNavigatedFrom(NavigationEventArgs^ e)
{
    CloseSocket();
}

void Scenario1::UpdateVisualState()
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

void Scenario1::SetBusy(bool value)
{
    busy = value;
    UpdateVisualState();
}

void Scenario1::OnConnect()
{
    SetBusy(true);
    ConnectAsync().then([this]()
    {
        SetBusy(false);
    });
}

task<void> Scenario1::ConnectAsync()
{
    if (InputField->Text == "")
    {
        rootPage->NotifyUser("Please specify text to send", NotifyType::ErrorMessage);
        return task_from_result();
    }

    // Validating the URI is required since it was received from an untrusted source (user input).
    // The URI is validated by calling TryGetUri() that will return 'nullptr' for strings that are not
    // valid WebSocket URIs.
    // Note that when enabling the text box users may provide URIs to machines on the intrAnet or intErnet. In
    // these cases the app requires the "Home or Work Networking" or "Internet (Client)" capability respectively.
    Uri^ server = rootPage->TryGetUri(ServerAddressField->Text);
    if (!server)
    {
        return task_from_result();
    }

    messageWebSocket = ref new MessageWebSocket();
    messageWebSocket->Control->MessageType = SocketMessageType::Utf8;
    messageWebSocket->MessageReceived +=
        ref new TypedEventHandler<
        MessageWebSocket^,
        MessageWebSocketMessageReceivedEventArgs^>(this, &Scenario1::MessageReceived);
    messageWebSocket->Closed += ref new TypedEventHandler<IWebSocket^, WebSocketClosedEventArgs^>(this, &Scenario1::OnClosed);

    // When connecting to wss:// endpoint, the OS by default performs validation of
    // the server certificate based on well-known trusted CAs. We can perform additional custom
    // validation if needed.
    if (SecureWebSocketCheckBox->IsChecked->Value)
    {
        // WARNING: Only test applications should ignore SSL errors.
        // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
        // attacks. (Although the connection is secure, the server is not authenticated.)
        // Note that not all certificate validation errors can be ignored.
        // In this case, we are ignoring these errors since the certificate assigned to the localhost
        // URI is self-signed and has subject name = fabrikam.com
        messageWebSocket->Control->IgnorableServerCertificateErrors->Append(ChainValidationResult::Untrusted);
        messageWebSocket->Control->IgnorableServerCertificateErrors->Append(ChainValidationResult::InvalidName);

        // Add event handler to listen to the ServerCustomValidationRequested event. This enables performing
        // custom validation of the server certificate. The event handler must implement the desired 
        // custom certificate validation logic.
        messageWebSocket->ServerCustomValidationRequested +=
            ref new TypedEventHandler<
            MessageWebSocket^,
            WebSocketServerCustomValidationRequestedEventArgs^>(
                this,
                &Scenario1::OnServerCustomValidationRequested);

        // Certificate validation occurs only for secure connections.
        if (server->SchemeName != "wss")
        {
            AppendOutputLine("Note: Certificate validation is performed only for the wss: scheme.");
        }
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

        // The default DataWriter encoding is Utf8.
        messageWriter = ref new DataWriter(messageWebSocket->OutputStream);
        rootPage->NotifyUser("Connected", NotifyType::StatusMessage);
    });
}

void Scenario1::OnServerCustomValidationRequested(
    MessageWebSocket^ sender,
    WebSocketServerCustomValidationRequestedEventArgs^ args)
{
    // In order to call async APIs in this handler, you must first take a deferral and then
    // complete it once you are done validating the certificate.
    auto deferral = args->GetDeferral();

    // Get the server certificate and certificate chain from the args parameter.
    MainPage::AreCertificateAndCertChainValidAsync(args->ServerCertificate, args->ServerIntermediateCertificates)
        .then([this, args, deferral](bool isValid)
    {
        if (!isValid)
        {
            args->Reject();
        }
        deferral->Complete();

        // Continue on the UI thread so we can update UI.
        Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, isValid]()
        {
            if (isValid)
            {
                AppendOutputLine("Custom validation of server certificate passed.");
            }
            else
            {
                AppendOutputLine("Custom validation of server certificate failed.");
            }
        }));
    });
}

void Scenario1::OnSend()
{
    SetBusy(true);
    SendAsync().then([this]()
    {
        SetBusy(false);
    });
}

task<void> Scenario1::SendAsync()
{
    String^ message = InputField->Text;
    if (message == "")
    {
        rootPage->NotifyUser("Please specify text to send", NotifyType::ErrorMessage);
        return task_from_result();
    }

    AppendOutputLine("Sending Message: " + message);

    // Buffer any data we want to send.
    messageWriter->WriteString(message);

    // Send the data as one complete message.
    return create_task(messageWriter->StoreAsync())
        .then([this](task<unsigned int> previousTask)
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

        rootPage->NotifyUser("Send Complete", NotifyType::StatusMessage);
    });
}

void Scenario1::MessageReceived(MessageWebSocket^ sender, MessageWebSocketMessageReceivedEventArgs^ args)
{
    // Dispatch the event to the UI thread so we can update UI.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        AppendOutputLine("Message Received; Type: " + args->MessageType.ToString());
        DataReader^ reader = args->GetDataReader();
        reader->UnicodeEncoding = UnicodeEncoding::Utf8;
        try
        {
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

void Scenario1::OnDisconnect()
{
    SetBusy(true);
    rootPage->NotifyUser("Closing", NotifyType::StatusMessage);
    CloseSocket();
    SetBusy(false);
}

// The method may be triggered remotely by the server sending unsolicited close frame or locally by Close()/delete operator.
void Scenario1::OnClosed(IWebSocket^ sender, WebSocketClosedEventArgs^ args)
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

void Scenario1::CloseSocket()
{
    if (messageWriter != nullptr)
    {
        // In order to reuse the socket with another DataWriter, the socket's output stream needs to be detached.
        // Otherwise, the DataWriter's destructor will automatically close the stream and all subsequent I/O operations
        // invoked on the socket's output stream will fail with ObjectDisposedException.
        //
        // This is only added for completeness, as this sample closes the socket in the very next code block.
        messageWriter->DetachStream();
        delete messageWriter;
        messageWriter = nullptr;
    }

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

void Scenario1::AppendOutputLine(String^ value)
{
    OutputField->Text += value + "\r\n";
}
