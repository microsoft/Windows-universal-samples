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
#include "Scenario2_Binary.xaml.h"

using namespace concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Sockets;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::Certificates;
using namespace Windows::Storage::Streams;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web;

Scenario2::Scenario2()
{
    InitializeComponent();
    UpdateVisualState();
}

void Scenario2::OnNavigatedFrom(NavigationEventArgs^ e)
{
    CloseSocket();
}

void Scenario2::UpdateVisualState()
{
    if (busy)
    {
        VisualStateManager::GoToState(this, "Busy", false);
    }
    else
    {
        bool connected = (streamWebSocket != nullptr);
        VisualStateManager::GoToState(this, connected ? "Connected" : "Disconnected", false);
    }
}

void Scenario2::SetBusy(bool value)
{
    busy = value;
    UpdateVisualState();
}

void Scenario2::OnStart()
{
    SetBusy(true);
    StartAsync().then([this]()
    {
        SetBusy(false);
    });
}

task<void> Scenario2::StartAsync()
{
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

    streamWebSocket = ref new StreamWebSocket();
    streamWebSocket->Closed += ref new TypedEventHandler<IWebSocket^, WebSocketClosedEventArgs^>(this, &Scenario2::OnClosed);

    // If we are connecting to wss:// endpoint, by default, the OS performs validation of
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
        streamWebSocket->Control->IgnorableServerCertificateErrors->Append(ChainValidationResult::Untrusted);
        streamWebSocket->Control->IgnorableServerCertificateErrors->Append(ChainValidationResult::InvalidName);

        // Add event handler to listen to the ServerCustomValidationRequested event. This enables performing
        // custom validation of the server certificate. The event handler must implement the desired 
        // custom certificate validation logic.
        streamWebSocket->ServerCustomValidationRequested +=
            ref new TypedEventHandler<
            StreamWebSocket^,
            WebSocketServerCustomValidationRequestedEventArgs^>(
                this,
                &Scenario2::OnServerCustomValidationRequested);

        // Certificate validation occurs only for secure connections.
        if (server->SchemeName != "wss")
        {
            AppendOutputLine("Note: Certificate validation is performed only for the wss: scheme.");
        }
    }

    AppendOutputLine("Connecting to " + server->DisplayUri + "...");

    return create_task(streamWebSocket->ConnectAsync(server))
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
            delete streamWebSocket;
            streamWebSocket = nullptr;

            AppendOutputLine(MainPage::BuildWebSocketError(ex));
            AppendOutputLine(ex->Message);
            return;
        }

        // Start a task to continuously read for incoming data
        StartReceiveData(streamWebSocket);

        // Start a task to continuously write outgoing data
        StartSendData(streamWebSocket);
    });
}

void Scenario2::OnServerCustomValidationRequested(
    StreamWebSocket^ sender,
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

void Scenario2::StartSendData(StreamWebSocket^ activeSocket)
{
    // Start a loop to continuously write outgoing data.
    BYTE bytes[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
    auto data = ref new Platform::Array<byte>(ARRAYSIZE(bytes));
    memcpy(data->Data, bytes, ARRAYSIZE(bytes));
    IBuffer^ sendBuffer = CryptographicBuffer::CreateFromByteArray(data);

    AppendOutputLine("Background write starting.");

    SendDataLoop(streamWebSocket, sendBuffer, 0);
}

void Scenario2::SendDataLoop(StreamWebSocket^ activeSocket, IBuffer^ sendBuffer, UINT32 bytesSentSoFar)
{
    if (streamWebSocket != activeSocket) {
        // Our socket is no longer active. Stop sending.
        AppendOutputLine("Background write stopped.");
        return;
    }

    create_task(activeSocket->OutputStream->WriteAsync(sendBuffer))
        .then([this, activeSocket, sendBuffer, bytesSentSoFar](task<UINT32> previousTask)
    {
        UINT32 bytesSentThisCall = 0;
        try
        {
            // Reraise any exception that occurred in the task.
            bytesSentThisCall = previousTask.get();
        }
        catch (Exception^ ex)
        {
            WebErrorStatus status = WebSocketError::GetStatus(ex->HResult);
            if (status == WebErrorStatus::OperationCanceled)
            {
                AppendOutputLine("Background write canceled.");
            }
            else
            {
                AppendOutputLine("Error: " + status.ToString());
                AppendOutputLine(ex->Message);
            }
            return;
        }

        UINT32 totalBytesSent = bytesSentSoFar + bytesSentThisCall;
        DataSentField->Text = totalBytesSent.ToString();

        // Delay so the user can watch what's going on.
        ThreadPoolTimer::CreateTimer(ref new TimerElapsedHandler(
            [this, activeSocket, sendBuffer, totalBytesSent](ThreadPoolTimer^ timer)
        {
            SendDataLoop(activeSocket, sendBuffer, totalBytesSent);
        }, CallbackContext::Same), TimeSpan{ 10000000 }); // 1 second
    });
}

// Continuously read incoming data.
void Scenario2::StartReceiveData(StreamWebSocket^ activeSocket)
{
    AppendOutputLine("Background read starting.");
    ReceiveDataLoop(activeSocket, ref new Buffer(1000), 0);
}

void Scenario2::ReceiveDataLoop(StreamWebSocket^ activeSocket, IBuffer^ readBuffer, UINT32 bytesReceivedSoFar)
{
    if (streamWebSocket != activeSocket) {
        // Our socket is no longer active. Stop reading.
        AppendOutputLine("Background read stopped.");
        return;
    }

    create_task(activeSocket->InputStream->ReadAsync(readBuffer, readBuffer->Capacity, InputStreamOptions::Partial))
        .then([this, activeSocket, readBuffer, bytesReceivedSoFar](task<IBuffer^> previousTask)
    {
        IBuffer^ resultBuffer;
        try
        {
            // Reraise any exception that occurred in the task.
            resultBuffer = previousTask.get();
        }
        catch (Exception^ ex)
        {
            AppendOutputLine("During read: " + rootPage->BuildWebSocketError(ex));
            AppendOutputLine(ex->Message);
            return;
        }

        UINT32 totalBytesReceived = bytesReceivedSoFar + resultBuffer->Length;
        DataReceivedField->Text = totalBytesReceived.ToString();

        // Do something with the data.

        // Continue reading until closed and ReadAsync fails.
        ReceiveDataLoop(activeSocket, readBuffer, totalBytesReceived);
    });
}

void Scenario2::OnStop()
{
    SetBusy(true);
    rootPage->NotifyUser("Stopping", NotifyType::StatusMessage);
    CloseSocket();
    SetBusy(false);
}

// The method may be triggered remotely by the server sending unsolicited close frame or locally by Close()/delete operator.
void Scenario2::OnClosed(IWebSocket^ sender, WebSocketClosedEventArgs^ args)
{
    // Dispatch the event to the UI thread so we do not need to synchronize access to streamWebSocket.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, sender, args]()
    {
        AppendOutputLine("Closed; Code: " + args->Code.ToString() + ", Reason: " + args->Reason);

        if (streamWebSocket == sender)
        {
            CloseSocket();
            UpdateVisualState();
        }
    }));
}

void Scenario2::CloseSocket()
{
    if (streamWebSocket != nullptr)
    {
        try
        {
            streamWebSocket->Close(1000, "Closed due to user request.");
        }
        catch (Exception^ ex)
        {
            AppendOutputLine(MainPage::BuildWebSocketError(ex));
            AppendOutputLine(ex->Message);
        }
        streamWebSocket = nullptr;
    }
}

void Scenario2::AppendOutputLine(String^ value)
{
    OutputField->Text += value + "\r\n";
}
