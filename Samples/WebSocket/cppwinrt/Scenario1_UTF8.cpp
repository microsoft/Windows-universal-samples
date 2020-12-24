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
#include "Scenario1_UTF8.h"
#include "Scenario1_UTF8.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Security::Cryptography::Certificates;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_UTF8::Scenario1_UTF8()
    {
        InitializeComponent();
        UpdateVisualState();
    }

    void Scenario1_UTF8::OnNavigatedFrom(NavigationEventArgs const&)
    {
        CloseSocket();
    }

    void Scenario1_UTF8::UpdateVisualState()
    {
        if (m_busy)
        {
            VisualStateManager::GoToState(*this, L"Busy", false);
        }
        else if (m_messageWebSocket != nullptr)
        {
            VisualStateManager::GoToState(*this, L"Connected", false);
        }
        else
        {
            VisualStateManager::GoToState(*this, L"Disconnected", false);
        }
    }

    void Scenario1_UTF8::SetBusy(bool value)
    {
        m_busy = value;
        UpdateVisualState();
    }

    fire_and_forget Scenario1_UTF8::OnConnect(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        SetBusy(true);
        co_await ConnectAsync();
        SetBusy(false);
    }

    IAsyncAction Scenario1_UTF8::ConnectAsync()
    {
        auto lifetime = get_strong();

        if (InputField().Text().empty())
        {
            m_rootPage.NotifyUser(L"Please specify text to send", NotifyType::ErrorMessage);
            co_return;
        }

        // Validating the URI is required since it was received from an untrusted source (user input).
        // The URI is validated by calling TryGetUri() that will return null for strings that are not
        // valid WebSocket URIs.
        // Note that when enabling the text box users may provide URIs to machines on the intrAnet
        // or intErnet. In these cases the app requires the "Private Networks (Client and Server)" or
        // "Internet (Client)" capability respectively.
        Uri server = TryGetUri(m_rootPage, ServerAddressField().Text());
        if (server == nullptr)
        {
            co_return;
        }

        m_messageWebSocket = MessageWebSocket();
        m_messageWebSocket.Control().MessageType(SocketMessageType::Utf8);
        m_messageWebSocket.MessageReceived({ get_weak(), &Scenario1_UTF8::MessageReceived });
        m_messageWebSocket.Closed({ get_weak(), &Scenario1_UTF8::OnClosed });

        // If we are connecting to wss:// endpoint, by default, the OS performs validation of
        // the server certificate based on well-known trusted CAs. We can perform additional custom
        // validation if needed.
        if (SecureWebSocketCheckBox().IsChecked().Value())
        {
            // WARNING: Only test applications should ignore SSL errors.
            // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
            // attacks. (Although the connection is secure, the server is not authenticated.)
            // Note that not all certificate validation errors can be ignored.
            // In this case, we are ignoring these errors since the certificate assigned to the localhost
            // URI is self-signed and has subject name = fabrikam.com
            m_messageWebSocket.Control().IgnorableServerCertificateErrors().Append(ChainValidationResult::Untrusted);
            m_messageWebSocket.Control().IgnorableServerCertificateErrors().Append(ChainValidationResult::InvalidName);

            // Add event handler to listen to the ServerCustomValidationRequested event. This enables performing
            // custom validation of the server certificate. The event handler must implement the desired
            // custom certificate validation logic.
            m_messageWebSocket.ServerCustomValidationRequested({ get_weak(), &Scenario1_UTF8::OnServerCustomValidationRequested });

            // Certificate validation occurs only for secure connections.
            if (server.SchemeName() != L"wss")
            {
                AppendOutputLine(L"Note: Certificate validation is performed only for the wss: scheme.");
            }
        }

        AppendOutputLine(L"Connecting to " + server.AbsoluteCanonicalUri() + L"...");
        try
        {
            co_await m_messageWebSocket.ConnectAsync(server);
        }
        catch (winrt::hresult_error const& ex) // For debugging
        {
            // Error happened during connect operation.
            m_messageWebSocket.Close();
            m_messageWebSocket = nullptr;

            AppendOutputLine(BuildWebSocketError(ex));
            AppendOutputLine(ex.message());
            co_return;
        }

        // The default DataWriter encoding is Utf8.
        m_messageWriter = DataWriter(m_messageWebSocket.OutputStream());
        m_rootPage.NotifyUser(L"Connected", NotifyType::StatusMessage);
    }

    fire_and_forget Scenario1_UTF8::OnServerCustomValidationRequested(MessageWebSocket const&, WebSocketServerCustomValidationRequestedEventArgs e)
    {
        auto lifetime = get_strong();

        // In order to call async APIs in this handler, you must first take a deferral and then
        // complete it once you are done with the operation. The Deferral destructor
        // ensures that the deferral completes when control leaves the function.
        Deferral deferral = e.GetDeferral();

        // Get the server certificate and certificate chain from the args parameter.
        const bool isValid = co_await AreCertificateAndCertChainValidAsync(e.ServerCertificate(), e.ServerIntermediateCertificates());

        if (!isValid)
        {
            e.Reject();
        }

        // Continue on the UI thread so we can update UI.
        co_await resume_foreground(Dispatcher());

        if (isValid)
        {
            AppendOutputLine(L"Custom validation of server certificate passed.");
        }
        else
        {
            AppendOutputLine(L"Custom validation of server certificate failed.");
        }
    }

    fire_and_forget Scenario1_UTF8::OnSend(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        SetBusy(true);
        co_await SendAsync();
        SetBusy(false);
    }

    IAsyncAction Scenario1_UTF8::SendAsync()
    {
        auto lifetime = get_strong();

        hstring message = InputField().Text();
        if (message.empty())
        {
            m_rootPage.NotifyUser(L"Please specify text to send", NotifyType::ErrorMessage);
            co_return;
        }

        AppendOutputLine(L"Sending Message: " + message);

        // Buffer any data we want to send.
        m_messageWriter.WriteString(message);

        try
        {
            // Send the data as one complete message.
            co_await m_messageWriter.StoreAsync();
        }
        catch (winrt::hresult_error const& ex)
        {
            AppendOutputLine(BuildWebSocketError(ex));
            AppendOutputLine(ex.message());
            co_return;
        }

        m_rootPage.NotifyUser(L"Send Complete", NotifyType::StatusMessage);
    }

    fire_and_forget Scenario1_UTF8::MessageReceived(MessageWebSocket const&, MessageWebSocketMessageReceivedEventArgs e)
    {
        auto lifetime = get_strong();

        // Continue on the UI thread so we can update UI.
        co_await resume_foreground(Dispatcher());
        
        AppendOutputLine(L"Message Received; Type: " + to_hstring(e.MessageType()));
        DataReader reader = e.GetDataReader();

        reader.UnicodeEncoding(UnicodeEncoding::Utf8);

        try
        {
            hstring read = reader.ReadString(reader.UnconsumedBufferLength());
            AppendOutputLine(read);
        }
        catch (winrt::hresult_error const& ex)
        {
            AppendOutputLine(BuildWebSocketError(ex));
            AppendOutputLine(ex.message());
        }
        reader.Close();
    }

    void Scenario1_UTF8::OnDisconnect(IInspectable const&, RoutedEventArgs const&)
    {
        SetBusy(true);
        m_rootPage.NotifyUser(L"Closing", NotifyType::StatusMessage);
        CloseSocket();
        SetBusy(false);
        m_rootPage.NotifyUser(L"Closed", NotifyType::StatusMessage);
    }

    // This may be triggered remotely by the server or locally by Close()
    fire_and_forget Scenario1_UTF8::OnClosed(IWebSocket sender, WebSocketClosedEventArgs e)
    {
        auto lifetime = get_strong();

        // Continue on the UI thread so we do not need to synchronize access to messageWebSocket.
        co_await resume_foreground(Dispatcher());

        AppendOutputLine(L"Closed; Code: " + to_hstring(e.Code()) + L", Reason: " + e.Reason());

        if (m_messageWebSocket == sender)
        {
            CloseSocket();
            UpdateVisualState();
        }
    }

    void Scenario1_UTF8::CloseSocket()
    {
        if (m_messageWriter)
        {
            // In order to reuse the socket with another DataWriter, the socket's output stream needs to be detached.
            // Otherwise, the DataWriter's destructor will automatically close the stream and all subsequent I/O operations
            // invoked on the socket's output stream will fail with ObjectDisposedException.
            //
            // This is only added for completeness, as this sample closes the socket in the very next code block.
            m_messageWriter.DetachStream();
            m_messageWriter.Close();
            m_messageWriter = nullptr;
        }

        if (m_messageWebSocket)
        {
            try
            {
                m_messageWebSocket.Close(1000, L"Closed due to user request.");
            }
            catch (winrt::hresult_error const& ex)
            {
                AppendOutputLine(BuildWebSocketError(ex));
                AppendOutputLine(ex.message());
            }
            m_messageWebSocket = nullptr;
        }
    }

    void Scenario1_UTF8::AppendOutputLine(hstring const& value)
    {
        OutputField().Text(OutputField().Text() + value + L"\r\n");
    }
}
