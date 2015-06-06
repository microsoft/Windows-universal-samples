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

using namespace concurrency;
using namespace SDKTemplate::WebSocket;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Core;
using namespace Windows::System::Threading;
using namespace Windows::Security::Cryptography;

Scenario2::Scenario2()
{
    InitializeComponent();
}

Scenario2::~Scenario2()
{
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
}

void SDKTemplate::WebSocket::Scenario2::Start_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    // Make a local copy to avoid races with the Closed event.
    StreamWebSocket^ webSocket = this->streamWebSocket;

    // Have we connected yet?
    if (webSocket != nullptr)
    {
        rootPage->NotifyUser("Already connected", NotifyType::StatusMessage);
        return;
    }

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

    webSocket = ref new StreamWebSocket();
    webSocket->Closed += ref new TypedEventHandler<IWebSocket^, WebSocketClosedEventArgs^>(this, &Scenario2::Scenario2Closed);

    task<void>(webSocket->ConnectAsync(server)).then([this, webSocket] (task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();

            rootPage->NotifyUser("Connected", NotifyType::StatusMessage);

            streamWebSocket = webSocket; // Only store it after successfully connecting.

            this->readBuffer = ref new Buffer(1000);

            OutputField->Text += "Background read starting.\r\n";
            bytesReceived = 0;

            // Start a loop to continuously read for incoming data.
            Scenario2ReceiveData();

            // Start a loop to continuously write outgoing data.
            BYTE bytes[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
            auto data = ref new Platform::Array<byte>(ARRAYSIZE(bytes));
            memcpy(data->Data, bytes, ARRAYSIZE(bytes));
            sendBuffer = CryptographicBuffer::CreateFromByteArray(data);
            MarshalText(OutputField, "Background sending data in " + sendBuffer->Length.ToString()
                + " byte chunks each second.\r\n");
            dataSent = 0;
            Scenario2SendData();
        }
        catch(Exception^ exception)
        {
            HandleException(exception);
        }
    });
}

void SDKTemplate::WebSocket::Scenario2::HandleException(Exception^ exception)
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

void SDKTemplate::WebSocket::Scenario2::Scenario2SendData()
{
    // Make a local copy to avoid races with the Closed event.
    StreamWebSocket^ webSocket = this->streamWebSocket;
    if (webSocket == nullptr)
    {
        return;
    }

    try
    {
        IOutputStream^ writeStream = webSocket->OutputStream;
        task<UINT32>(writeStream->WriteAsync(sendBuffer)).then([this] (UINT32 sent)
        {
            dataSent += sent;
            MarshalText(DataSentField, dataSent.ToString(), false);

            // Delay so the user can watch what's going on.
            TimeSpan time = { 10000000 }; // 1 second.
            ThreadPoolTimer::CreateTimer(ref new TimerElapsedHandler([this](ThreadPoolTimer^ timer) {
                Scenario2SendData();
            }), time);
        }).then([this] (task<void> previousTask)
        {
            try
            {
                // Try getting all exceptions from the continuation chain above this point.
                previousTask.get();
            }
            catch (Exception^ exception)
            {
                MarshalText(OutputField, exception->Message + "\r\n");
            }
        });
    }
    catch (Exception^ exception)
    {
        if (exception->HResult == RO_E_CLOSED)
        {
            MarshalText(OutputField, "Output stream already closed by the user\r\n");
        }
        else
        {
            throw;
        }
    }
}

// Continuously read incoming data. For reading data we'll show how to use webSocket.InputStream.AsStream()
// to get a .NET stream. Alternatively you could call readBuffer.AsBuffer() to use IBuffer with
// webSocket->InputStream->ReadAsync.
void SDKTemplate::WebSocket::Scenario2::Scenario2ReceiveData()
{
    // Make a local copy to avoid races with the Closed event.
    StreamWebSocket^ webSocket = this->streamWebSocket;
    if (webSocket == nullptr)
    {
        return;
    }

    try
    {
        IInputStream^ readStream = webSocket->InputStream;

        task<IBuffer^>(readStream->ReadAsync(readBuffer, readBuffer->Capacity, InputStreamOptions::Partial)).then([this] (IBuffer^ buffer)
        {
            bytesReceived += buffer->Length;
            DataReceivedField->Text = bytesReceived.ToString();

            // Do something with the data.
        }).then([this] (task<void> previousTask)
        {
            try
            {
                // Try getting all exceptions from the continuation chain above this point.
                previousTask.get();
            }
            catch (Exception^ exception)
            {
                WebErrorStatus status = WebSocketError::GetStatus(exception->HResult);
                if (status == WebErrorStatus::OperationCanceled)
                {
                    OutputField->Text += "Background read canceled.\r\n";
                }
                else
                {
                    HandleException(exception);
                }
            }

            // Continue reading until closed and ReadAsync fails.
            Scenario2ReceiveData();
        });
    }
    catch (Exception^ exception)
    {
        if (exception->HResult == RO_E_CLOSED)
        {
            OutputField->Text += "Input stream already closed by the user\r\n";
        }
        else
        {
            throw;
        }
    }
}

void SDKTemplate::WebSocket::Scenario2::Stop_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    try
    {
        if (streamWebSocket != nullptr)
        {
            rootPage->NotifyUser("Stopping", NotifyType::StatusMessage);
            streamWebSocket->Close(1000, "Closed due to user request.");
            streamWebSocket = nullptr;
        }
        else
        {
            rootPage->NotifyUser("There is no active socket to stop.", NotifyType::StatusMessage);
        }
    }
    catch (Exception^ exception)
    {
        HandleException(exception);
    }
}

void SDKTemplate::WebSocket::Scenario2::Scenario2Closed(IWebSocket^ sender, WebSocketClosedEventArgs^ args)
{
    // The method may be triggered remotely by the server sending unsolicited close frame or locally by Close()/delete operator.
    // Dispatch the event to the UI thread so we do not need to synchronize access to streamWebSocket.
    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args] ()
    {
        OutputField->Text += "Closed; Code: " + args->Code.ToString() + ", Reason: " + args->Reason + "\r\n";

        if (streamWebSocket != nullptr)
        {
            delete streamWebSocket;
            streamWebSocket = nullptr;
        }
    }));
}

void SDKTemplate::WebSocket::Scenario2::MarshalText(TextBox^ output, String^ value)
{
    MarshalText(output, value, true);
}

// When operations happen on a background thread we have to marshal UI updates back to the UI thread.
void SDKTemplate::WebSocket::Scenario2::MarshalText(TextBox^ output, String^ value, bool append)
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
    }, CallbackContext::Any));
}
