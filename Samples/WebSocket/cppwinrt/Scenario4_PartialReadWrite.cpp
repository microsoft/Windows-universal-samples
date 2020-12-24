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
#include "Scenario4_PartialReadWrite.h"
#include "Scenario4_PartialReadWrite.g.cpp"
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
    Scenario4_PartialReadWrite::Scenario4_PartialReadWrite()
    {
        InitializeComponent();
        UpdateVisualState();
    }

    void Scenario4_PartialReadWrite::OnNavigatedFrom(NavigationEventArgs const&)
    {
        CloseSocket();
    }

    void Scenario4_PartialReadWrite::UpdateVisualState()
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

    void Scenario4_PartialReadWrite::SetBusy(bool value)
    {
        m_busy = value;
        UpdateVisualState();
    }

    IAsyncAction Scenario4_PartialReadWrite::OnConnect(IInspectable sender, RoutedEventArgs e)
    {
        auto lifetime = get_strong();

        SetBusy(true);
        co_await ConnectAsync();
        SetBusy(false);
    }

    IAsyncAction Scenario4_PartialReadWrite::ConnectAsync()
    {
        auto lifetime = get_strong();

        if (ServerAddressField().Text().empty())
        {
            m_rootPage.NotifyUser(L"Please specify a URI to connect to", NotifyType::ErrorMessage);
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

        // To support receiving event notifications for partial messages, you must set this receive mode.
        // If you do not set this mode, you will only receive notifications for complete messages; which is
        // the default behavior. Setting to Partial allows us to process partial data as soon as it arrives,
        // as opposed to waiting until the EndOfMessage to process the entire data.
        m_messageWebSocket.Control().ReceiveMode(MessageWebSocketReceiveMode::PartialMessage);

        m_messageWebSocket.MessageReceived({ get_weak(), &Scenario4_PartialReadWrite::MessageReceived });
        m_messageWebSocket.Closed({ get_weak(), &Scenario4_PartialReadWrite::OnClosed });

        if (server.SchemeName() == L"wss")
        {
            // WARNING: Only test applications should ignore SSL errors.
            // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
            // attacks. (Although the connection is secure, the server is not authenticated.)
            // Note that not all certificate validation errors can be ignored.
            // In this case, we are ignoring these errors since the certificate assigned to the localhost
            // URI is self-signed and has subject name = fabrikam.com
            m_messageWebSocket.Control().IgnorableServerCertificateErrors().Append(ChainValidationResult::Untrusted);
            m_messageWebSocket.Control().IgnorableServerCertificateErrors().Append(ChainValidationResult::InvalidName);
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

        m_rootPage.NotifyUser(L"Connected", NotifyType::StatusMessage);
    }

    IAsyncAction Scenario4_PartialReadWrite::OnSend(IInspectable sender, RoutedEventArgs e)
    {
        auto lifetime = get_strong();

        SetBusy(true);
        co_await SendAsync();
        SetBusy(false);
    }

    IAsyncAction Scenario4_PartialReadWrite::SendAsync()
    {
        auto lifetime = get_strong();

        hstring message = InputField().Text();
        if (message.empty())
        {
            m_rootPage.NotifyUser(L"Please specify text to send", NotifyType::ErrorMessage);
            co_return;
        }

        try
        {
            DataWriter messageWriter;
            messageWriter.WriteString(message);
            IBuffer buffer = messageWriter.DetachBuffer();

            if (EndOfMessageCheckBox().IsChecked().Value())
            {
                AppendOutputLine(L"Sending end of message: " + message);
                co_await m_messageWebSocket.SendFinalFrameAsync(buffer);
            }
            else
            {
                AppendOutputLine(L"Sending partial message: " + message);
                co_await m_messageWebSocket.SendNonfinalFrameAsync(buffer);
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            AppendOutputLine(BuildWebSocketError(ex));
            AppendOutputLine(ex.message());
            co_return;
        }

        m_rootPage.NotifyUser(L"Send Complete", NotifyType::StatusMessage);
    }

    IAsyncAction Scenario4_PartialReadWrite::MessageReceived(MessageWebSocket sender, MessageWebSocketMessageReceivedEventArgs e)
    {
        auto lifetime = get_strong();

        co_await m_rootPage.Dispatcher();

        hstring partialOrCompleted = L"Partial";

        if (e.IsMessageComplete())
        {
            partialOrCompleted = L"Complete";
        }
        
        AppendOutputLine(partialOrCompleted + L" message Received; Type: " + to_hstring(e.MessageType()));
        DataReader reader = e.GetDataReader();

        reader.UnicodeEncoding(UnicodeEncoding::Utf8);

        try
        {
            // Note that it may be possible for partial UTF8 messages to be split between a character if it
            // extends multiple bytes. We avoid this by using only ASCII characters in this example. 
            // Should your application use multi-byte characters, you will need to do checks for broken characters.
            hstring read = reader.ReadString(reader.UnconsumedBufferLength());
            AppendOutputLine(read);
        }
        catch (winrt::hresult_error const& ex)
        {
            AppendOutputLine(BuildWebSocketError(ex));
            AppendOutputLine(ex.message());
        }
    }

    void Scenario4_PartialReadWrite::OnDisconnect(IInspectable const&, RoutedEventArgs const&)
    {
        SetBusy(true);
        m_rootPage.NotifyUser(L"Closing", NotifyType::StatusMessage);
        CloseSocket();
        SetBusy(false);
        m_rootPage.NotifyUser(L"Closed", NotifyType::StatusMessage);
    }

    // This may be triggered remotely by the server or locally by Close/Dispose()
    IAsyncAction Scenario4_PartialReadWrite::OnClosed(IWebSocket sender, WebSocketClosedEventArgs e)
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

    void Scenario4_PartialReadWrite::CloseSocket()
    {
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

    void Scenario4_PartialReadWrite::AppendOutputLine(hstring const& value)
    {
        OutputField().Text(OutputField().Text() + value + L"\r\n");
    }
}
