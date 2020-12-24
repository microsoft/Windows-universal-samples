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
#include "Scenario2_Binary.h"
#include "Scenario2_Binary.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Security::Cryptography::Certificates;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web;
using namespace std::literals::chrono_literals;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Binary::Scenario2_Binary()
    {
        InitializeComponent();
        UpdateVisualState();
    }

    void Scenario2_Binary::OnNavigatedFrom(NavigationEventArgs const&)
    {
        CloseSocket();
    }

    void Scenario2_Binary::UpdateVisualState()
    {
        if (m_busy)
        {
            VisualStateManager::GoToState(*this, L"Busy", false);
        }
        else if (m_streamWebSocket != nullptr)
        {
                VisualStateManager::GoToState(*this, L"Connected", false);
            }
        else
        {
            VisualStateManager::GoToState(*this, L"Disconnected", false);
        }
   }

    void Scenario2_Binary::SetBusy(bool value)
    {
        m_busy = value;
        UpdateVisualState();
    }

    fire_and_forget Scenario2_Binary::OnStart(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        SetBusy(true);
        co_await StartAsync();
        SetBusy(false);
    }

    IAsyncAction Scenario2_Binary::StartAsync()
    {
        auto lifetime = get_strong();

        // Validating the URI is required since it was received from an untrusted source (user input).
        // The URI is validated by calling TryGetUri() that will return null for strings that are not
        // valid WebSocket URIs.
        // Note that when enabling the text box users may provide URIs to machines on the intrAnet
        // or intErnet. In these cases the app requires the "Home or Work Networking" or
        // "Internet (Client)" capability respectively.
        Uri server = TryGetUri(m_rootPage, ServerAddressField().Text());
        if (server == nullptr)
        {
            co_return;
        }

        m_streamWebSocket = StreamWebSocket();
        m_streamWebSocket.Closed({ get_weak(), &Scenario2_Binary::OnClosed });

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
            m_streamWebSocket.Control().IgnorableServerCertificateErrors().Append(ChainValidationResult::Untrusted);
            m_streamWebSocket.Control().IgnorableServerCertificateErrors().Append(ChainValidationResult::InvalidName);

            // Add event handler to listen to the ServerCustomValidationRequested event. This enables performing
            // custom validation of the server certificate. The event handler must implement the desired
            // custom certificate validation logic.
            m_streamWebSocket.ServerCustomValidationRequested({ get_weak(), &Scenario2_Binary::OnServerCustomValidationRequested });

            // Certificate validation is meaningful only for secure connections.
            if (server.SchemeName() != L"wss")
            {
                AppendOutputLine(L"Note: Certificate validation is performed only for the wss: scheme.");
            }
        }

        AppendOutputLine(L"Connecting to " + server.AbsoluteCanonicalUri() + L"...");
        try
        {
            co_await m_streamWebSocket.ConnectAsync(server);
        }
        catch (winrt::hresult_error const& ex) // For debugging
        {
            m_streamWebSocket.Close();
            m_streamWebSocket = nullptr;

            AppendOutputLine(BuildWebSocketError(ex));
            AppendOutputLine(ex.message());
            co_return;
        }
        m_rootPage.NotifyUser(L"Connected", NotifyType::StatusMessage);

        // Start a task to continuously read for incoming data
        auto receiving = ReceiveDataAsync(m_streamWebSocket);

        // Start a task to continuously write outgoing data
        auto sending = SendDataAsync(m_streamWebSocket);
    }

    fire_and_forget Scenario2_Binary::OnServerCustomValidationRequested(StreamWebSocket const&, WebSocketServerCustomValidationRequestedEventArgs e)
    {
        auto lifetime = get_strong();

        // In order to call async APIs in this handler, you must first take a deferral and then
        // complete it once you are done with the operation. The Deferral destructor
        // ensures that the deferral completes when control leaves the function.
        bool isValid;
        Deferral deferral = e.GetDeferral();

        // Get the server certificate and certificate chain from the args parameter.
        isValid = co_await AreCertificateAndCertChainValidAsync(e.ServerCertificate(), e.ServerIntermediateCertificates());

        if (!isValid)
        {
            e.Reject();
        }

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

    IAsyncAction Scenario2_Binary::SendDataAsync(StreamWebSocket activeSocket)
    {
        auto lifetime = get_strong();

        int bytesSent = 0;
        const uint8_t data[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

        AppendOutputLine(L"Background sending data in " + to_hstring(sizeof(data)) + L" byte chunks each second.");

        try
        {
            // Send until the socket gets closed/stopped
            while (true)
            {
                co_await resume_foreground(Dispatcher());

                if (m_streamWebSocket != activeSocket)
                {
                    // Our socket is no longer active. Stop sending.
                    AppendOutputLine(L"Background write stopped.");
                    co_return;
                }

                auto buffer = CryptographicBuffer::CreateFromByteArray(data);
                auto bytesSentThisCall = co_await activeSocket.OutputStream().WriteAsync(buffer);
                bytesSent += bytesSentThisCall;

                DataSentField().Text(to_hstring(bytesSent));

                // Delay so the user can watch what's going on.
                co_await winrt::resume_after(1s);
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            WebErrorStatus status = WebSocketError::GetStatus(ex.code());

            switch (status)
            {
            case WebErrorStatus::OperationCanceled:
                AppendOutputLine(L"Background write canceled.");
                break;

            default:
                AppendOutputLine(L"Error: " + to_hstring(status));
                AppendOutputLine(ex.message());
                break;
            }
        }
    }

    IAsyncAction Scenario2_Binary::ReceiveDataAsync(StreamWebSocket activeSocket)
    {
        auto lifetime = get_strong();

        int bytesReceived = 0;
        try
        {
            AppendOutputLine(L"Background read starting.");

            Buffer readBuffer(1000);

            while (true)
            {
                co_await resume_foreground(Dispatcher());

                if (m_streamWebSocket != activeSocket)
                {
                    // Our socket is no longer active. Stop reading.
                    AppendOutputLine(L"Background read stopped.");
                    co_return;
                }

                IBuffer resultBuffer = co_await m_streamWebSocket.InputStream().ReadAsync(readBuffer, 1000, InputStreamOptions::Partial);

                // Do something with the data.
                // This sample merely reports that the data was received.

                bytesReceived += resultBuffer.Length();
                DataReceivedField().Text(to_hstring(bytesReceived));
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            WebErrorStatus status = WebSocketError::GetStatus(ex.code());

            switch (status)
            {
            case WebErrorStatus::OperationCanceled:
                AppendOutputLine(L"Background read canceled.");
                break;

            default:
                AppendOutputLine(L"Error: " + to_hstring(status));
                AppendOutputLine(ex.message());
                break;
            }
        }
    }

    void Scenario2_Binary::OnStop(IInspectable const&, RoutedEventArgs const&)
    {
        SetBusy(true);
        m_rootPage.NotifyUser(L"Stopping", NotifyType::StatusMessage);
        CloseSocket();
        SetBusy(false);
        m_rootPage.NotifyUser(L"Stopped", NotifyType::StatusMessage);
    }

    // This may be triggered remotely by the server or locally by Close/Dispose()
    fire_and_forget Scenario2_Binary::OnClosed(IWebSocket sender, WebSocketClosedEventArgs e)
    {
        auto lifetime = get_strong();

        // Continue on the UI thread so we do not need to synchronize access to messageWebSocket.
        co_await resume_foreground(Dispatcher());

        AppendOutputLine(L"Closed; Code: " + to_hstring(e.Code()) + L", Reason: " + e.Reason());

        if (m_streamWebSocket == sender)
        {
            CloseSocket();
            UpdateVisualState();
        }
    }

    void Scenario2_Binary::CloseSocket()
    {
        if (m_streamWebSocket)
        {
            try
            {
                m_streamWebSocket.Close(1000, L"Closed due to user request.");
            }
            catch (winrt::hresult_error const& ex)
            {
                AppendOutputLine(BuildWebSocketError(ex));
                AppendOutputLine(ex.message());
            }
            m_streamWebSocket = nullptr;
        }
    }

    void Scenario2_Binary::AppendOutputLine(hstring const& value)
    {
        OutputField().Text(OutputField().Text() + value + L"\r\n");
    }
}
