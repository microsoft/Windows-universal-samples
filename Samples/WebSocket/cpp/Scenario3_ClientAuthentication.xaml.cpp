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
// Scenario3.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include "Scenario3_ClientAuthentication.xaml.h"

using namespace concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Sockets;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::Certificates;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web;

static String^ const ClientCertUriPath = "ms-appx:///Assets/clientCert.pfx";
static String^ const ClientCertPassword = "1234";
static String^ const ClientCertFriendlyName = "WebSocketSampleClientCert";
static String^ const ClientCertIssuerName = "www.contoso.com";

Scenario3::Scenario3()
{
    InitializeComponent();
    UpdateVisualState();
}

void Scenario3::OnNavigatedFrom(NavigationEventArgs^ e)
{
    CloseSocket();
}

void Scenario3::UpdateVisualState()
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

void Scenario3::SetBusy(bool value)
{
    busy = value;
    UpdateVisualState();
}

void Scenario3::OnConnect()
{
    SetBusy(true);
    ConnectWebSocketAsync().then([this]()
    {
        SetBusy(false);
    });
}

task<Certificate^> Scenario3::FindCertificateFromStoreAsync()
{
    // Find the client certificate for authentication. If not found, it means it has not been installed.
    CertificateQuery^ query = ref new CertificateQuery();
    query->IssuerName = ClientCertIssuerName;
    query->FriendlyName = ClientCertFriendlyName;

    return create_task(CertificateStores::FindAllAsync(query))
        .then([](IVectorView<Certificate^>^ certs)
    {
        // This sample installs only one certificate, so if we find one, it must be ours.
        return (certs->Size > 0) ? certs->GetAt(0) : nullptr;
    });
}

task<Certificate^> Scenario3::InstallClientCertificateAsync()
{
    // Load the certificate from the clientCert.pfx file packaged with this sample.
    // This certificate has been signed with a trusted root certificate installed on the server.
    // The installation is done by running the setupServer.ps1 file, which should have been done
    // before running the app.
    // WARNING: Including a pfx file in the app package violates the Windows Store
    // certification requirements. We are shipping the pfx file with the package for demonstrating
    // the usage of client certificates. Apps that will be published through Windows Store
    // need to use other approaches to obtain a client certificate.
    return create_task(StorageFile::GetFileFromApplicationUriAsync(ref new Uri(ClientCertUriPath)))
        .then([](StorageFile^ clientCertFile)
    {
        return create_task(FileIO::ReadBufferAsync(clientCertFile));
    }).then([this](IBuffer^ buffer)
    {
        String^ clientCertData = CryptographicBuffer::EncodeToBase64String(buffer);
        AppendOutputLine("Reading certificate succeeded.");

        // Install the certificate to the app's certificate store.
        // The app certificate store is removed when the app is uninstalled.

        //To install a certificate to the CurrentUser\MY store, which is not app specific,
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
        return create_task(CertificateEnrollmentManager::ImportPfxDataAsync(
            clientCertData,
            ClientCertPassword,
            ExportOption::Exportable,
            KeyProtectionLevel::NoConsent,
            InstallOptions::DeleteExpired,
            ClientCertFriendlyName));
    }).then([this]()
    {
        AppendOutputLine("Installing certificate succeeded.");

        // Return the certificate we just installed.
        return FindCertificateFromStoreAsync();
    }).then([this](task<Certificate^> previousTask)
    {
        try
        {
            return previousTask.get();
        }
        catch (Exception^ ex)
        {
            // This can happen if the certificate has already expired.
            AppendOutputLine("Installing certificate failed with" + ex->Message);
            return static_cast<Certificate^>(nullptr);
        }
    });
}

task<Certificate^> Scenario3::GetClientCertificateAsync()
{
    // The client certificate could be installed already (if this scenario has been run before),
    // try finding it from the store.
    return FindCertificateFromStoreAsync().then([this](Certificate^ cert)
    {
        if (cert)
        {
            return task_from_result(cert);
        }

        // If the client certificate is not already installed, install and return it.
        return InstallClientCertificateAsync();
    });
}

task<void> Scenario3::ConnectWebSocketAsync()
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

    // Certificate validation is meaningful only for secure connections.
    if (server->SchemeName != "wss")
    {
        AppendOutputLine("Note: Certificate validation is performed only for the wss: scheme.");
    }

    streamWebSocket = ref new StreamWebSocket();
    streamWebSocket->Closed += ref new TypedEventHandler<IWebSocket^, WebSocketClosedEventArgs^>(this, &Scenario3::OnClosed);

    return GetClientCertificateAsync().then([this, server](Certificate^ cert)
    {
        // It is okay to set the ClientCertificate property even if GetClientCertificateAsync returns null.
        streamWebSocket->Control->ClientCertificate = cert;

        // When connecting to wss:// endpoint, the OS by default performs validation of
        // the server certificate based on well-known trusted CAs. For testing purposes, we are ignoring
        // SSL errors.
        // WARNING: Only test applications should ignore SSL errors.
        // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
        // attacks. (Although the connection is secure, the server is not authenticated.)
        // Note that not all certificate validation errors can be ignored.
        // In this case, we are ignoring these errors since the certificate assigned to the localhost
        // URI is self-signed and has subject name = fabrikam.com
        streamWebSocket->Control->IgnorableServerCertificateErrors->Append(ChainValidationResult::Untrusted);
        streamWebSocket->Control->IgnorableServerCertificateErrors->Append(ChainValidationResult::InvalidName);

        AppendOutputLine("Connecting to " + server->DisplayUri + "...");
        return create_task(streamWebSocket->ConnectAsync(server));
    }).then([this](task<void> previousTask)
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

        rootPage->NotifyUser("Connected", NotifyType::StatusMessage);
        AppendOutputLine("Connected to WebSocket Server.");
    });
}

void Scenario3::OnDisconnect()
{
    SetBusy(true);
    rootPage->NotifyUser("Closing", NotifyType::StatusMessage);
    CloseSocket();
    SetBusy(false);
    rootPage->NotifyUser("Closed", NotifyType::StatusMessage);
}

// The method may be triggered remotely by the server or locally by Close()/delete operator.
void Scenario3::OnClosed(IWebSocket^ sender, WebSocketClosedEventArgs^ args)
{
    // Dispatch the event to the UI thread so we do not need to synchronize access to streamWebSocket.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, sender, args] ()
    {
        AppendOutputLine("Closed; Code: " + args->Code.ToString() + ", Reason: " + args->Reason);

        if (streamWebSocket == sender)
        {
            CloseSocket();
            UpdateVisualState();
        }
    }));
}

void Scenario3::CloseSocket()
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

void Scenario3::AppendOutputLine(String^ value)
{
    OutputField->Text += value + "\r\n";
}
