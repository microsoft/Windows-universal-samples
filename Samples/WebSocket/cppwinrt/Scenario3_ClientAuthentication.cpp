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
#include "Scenario3_ClientAuthentication.h"
#include "Scenario3_ClientAuthentication.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Security::Cryptography::Certificates;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_ClientAuthentication::Scenario3_ClientAuthentication()
    {
        InitializeComponent();
        UpdateVisualState();
    }

    void Scenario3_ClientAuthentication::OnNavigatedFrom(NavigationEventArgs const&)
    {
        CloseSocket();
    }

    void Scenario3_ClientAuthentication::UpdateVisualState()
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

    void Scenario3_ClientAuthentication::SetBusy(bool value)
    {
        m_busy = value;
        UpdateVisualState();
    }

    IAsyncAction Scenario3_ClientAuthentication::OnConnect(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        SetBusy(true);
        co_await ConnectWebSocketAsync();
        SetBusy(false);
    }

    IAsyncOperation<Certificate> Scenario3_ClientAuthentication::FindCertificateFromStoreAsync()
    {
        auto lifetime = get_strong();

        // Find the client certificate for authentication. If not found, it means it has not been installed.
        CertificateQuery query;
        query.IssuerName(m_ClientCertIssuerName);
        query.FriendlyName(m_ClientCertFriendlyName);

        IVectorView<Certificate> certs = co_await CertificateStores::FindAllAsync(query);
        if (certs.Size() == 0)
        {
            co_return nullptr;
        }

        // This sample installs only one certificate, so if we find one, it must be ours.
        co_return certs.GetAt(0);
    }

    IAsyncOperation<Certificate> Scenario3_ClientAuthentication::InstallClientCertificateAsync()
    {
        auto lifetime = get_strong();

        // Load the certificate from the clientCert.pfx file packaged with this sample.
        // This certificate has been signed with a trusted root certificate installed on the server.
        // The installation is done by running the setupServer.ps1 file, which should have been done
        // before running the app.
        // WARNING: Including a pfx file in the app package violates the Windows Store
        // certification requirements. We are shipping the pfx file with the package for demonstrating
        // the usage of client certificates. Apps that will be published through Windows Store
        // need to use other approaches to obtain a client certificate.
        Uri clientCertUri(m_ClientCertUriPath);
        StorageFile clientCertFile = co_await StorageFile::GetFileFromApplicationUriAsync(clientCertUri);

        IBuffer buffer = co_await FileIO::ReadBufferAsync(clientCertFile);
        AppendOutputLine(L"Reading certificate succeeded.");

        hstring clientCertData = Windows::Security::Cryptography::CryptographicBuffer::EncodeToBase64String(buffer);

        try
        {
            // Install the certificate to the app's certificate store.
            // The app certificate store is removed when the app is uninstalled.

            // To install a certificate to the CurrentUser\MY store, which is not app specific,
            // you need to use CertificateEnrollmentManager.UserCertificateEnrollmentManager.ImportPfxDataAsync().
            // In order to call that method, an app must have the "sharedUserCertificates" capability.
            // There are two ways to add this capability:
            //
            // 1. You can double click on the Package.appxmanifest file from the
            // solution explorer, select the "Capabilities" tab in the opened page, and then check the
            // "Shared User Certificates" box from the capabilities list.
            // 2. You can right click on the Package.appxmanifest file from the
            // solution explorer, select "View Code", and add "sharedUserCertificates" under the <Capabilities>
            // element directly.
            // Package.appxmanifest in this sample shows this capability commented out.
            //
            // The certificate will remain even when the app is uninstalled.
            co_await CertificateEnrollmentManager::ImportPfxDataAsync(
                clientCertData,
                m_ClientCertPassword,
                ExportOption::Exportable,
                KeyProtectionLevel::NoConsent,
                InstallOptions::DeleteExpired,
                m_ClientCertFriendlyName);

            AppendOutputLine(L"Installing certificate succeeded.");

            // Return the certificate we just installed.
            co_return co_await FindCertificateFromStoreAsync();
        }
        catch (winrt::hresult_error const& ex)
        {
            // This can happen if the certificate has already expired.
            AppendOutputLine(L"Installing certificate failed with " + ex.message());
            co_return nullptr;
        }
    }

    IAsyncOperation<Certificate> Scenario3_ClientAuthentication::GetClientCertificateAsync()
    {
        auto lifetime = get_strong();

        // The client certificate could be installed already (if this scenario has been run before),
        // try finding it from the store.
        Certificate cert = co_await FindCertificateFromStoreAsync();
        if (cert != nullptr)
        {
            co_return cert;
        }

        // If the client certificate is not already installed, install it.
        co_return co_await InstallClientCertificateAsync();
    }

    IAsyncAction Scenario3_ClientAuthentication::ConnectWebSocketAsync()
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
            return;
        }

        // Certificate validation is meaningful only for secure connections.
        if (server.SchemeName() != L"wss")
        {
            AppendOutputLine(L"Note: Certificate validation is performed only for the wss: scheme.");
        }

        m_streamWebSocket = StreamWebSocket();
        m_streamWebSocket.Closed({ get_weak(), &Scenario3_ClientAuthentication::OnClosed });

        // It is okay to set the ClientCertificate property even if GetClientCertificateAsync returns null.
        Certificate certificate = co_await GetClientCertificateAsync();
        m_streamWebSocket.Control().ClientCertificate(certificate);

        // When connecting to wss:// endpoint, the OS by default performs validation of
        // the server certificate based on well-known trusted CAs. For testing purposes, we are ignoring
        // SSL errors.
        // WARNING: Only test applications should ignore SSL errors.
        // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
        // attacks. (Although the connection is secure, the server is not authenticated.)
        // Note that not all certificate validation errors can be ignored.
        // In this case, we are ignoring these errors since the certificate assigned to the localhost
        // URI is self-signed and has subject name = fabrikam.com
        m_streamWebSocket.Control().IgnorableServerCertificateErrors().Append(ChainValidationResult::Untrusted);
        m_streamWebSocket.Control().IgnorableServerCertificateErrors().Append(ChainValidationResult::InvalidName);

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
        AppendOutputLine(L"Connected to WebSocket Server.");
    }

    void Scenario3_ClientAuthentication::OnDisconnect(IInspectable const&, RoutedEventArgs const&)
    {
        SetBusy(true);
        m_rootPage.NotifyUser(L"Closing", NotifyType::StatusMessage);
        CloseSocket();
        SetBusy(false);
        m_rootPage.NotifyUser(L"Closed", NotifyType::StatusMessage);
    }

    // This may be triggered remotely by the server or locally by Close/Dispose()
    IAsyncAction Scenario3_ClientAuthentication::OnClosed(IWebSocket sender, WebSocketClosedEventArgs e)
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

    void Scenario3_ClientAuthentication::CloseSocket()
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

    void Scenario3_ClientAuthentication::AppendOutputLine(hstring const& value)
    {
        OutputField().Text(OutputField().Text() + value + L"\r\n");
    }
}
