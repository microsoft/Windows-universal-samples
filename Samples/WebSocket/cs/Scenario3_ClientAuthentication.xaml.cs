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

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Networking.Sockets;
using Windows.Security.Cryptography.Certificates;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3 : Page
    {
        private static readonly string ClientCertUriPath = "ms-appx:///Assets/clientCert.pfx";
        private static readonly string ClientCertPassword = "1234";
        private static readonly string ClientCertFriendlyName = "WebSocketSampleClientCert";
        private static readonly string ClientCertIssuerName = "www.contoso.com";

        public Scenario3()
        {
            this.InitializeComponent();
            UpdateVisualState();
        }

        MainPage rootPage = MainPage.Current;

        private StreamWebSocket streamWebSocket;
        private bool busy;

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            CloseSocket();
        }

        private void UpdateVisualState()
        {
            if (busy)
            {
                VisualStateManager.GoToState(this, "Busy", false);
            }
            else
            {
                VisualStateManager.GoToState(this, streamWebSocket != null ? "Connected" : "Disconnected", false);
            }
        }

        private void SetBusy(bool value)
        {
            busy = value;
            UpdateVisualState();
        }

        private async void OnConnect()
        {
            SetBusy(true);
            await ConnectWebSocketAsync();
            SetBusy(false);
        }

        private async Task<Certificate> FindCertificateFromStoreAsync()
        {
            // Find the client certificate for authentication. If not found, it means it has not been installed.
            CertificateQuery query = new CertificateQuery();
            query.IssuerName = ClientCertIssuerName;
            query.FriendlyName = ClientCertFriendlyName;

            IReadOnlyList<Certificate> certs = await CertificateStores.FindAllAsync(query);
            if (certs.Count == 0)
            {
                return null;
            }

            // This sample installs only one certificate, so if we find one, it must be ours.
            return certs[0];
        }

        private async Task<Certificate> InstallClientCertificateAsync()
        {
            // Load the certificate from the clientCert.pfx file packaged with this sample.
            // This certificate has been signed with a trusted root certificate installed on the server.
            // The installation is done by running the setupServer.ps1 file, which should have been done
            // before running the app.
            // WARNING: Including a pfx file in the app package violates the Windows Store
            // certification requirements. We are shipping the pfx file with the package for demonstrating
            // the usage of client certificates. Apps that will be published through Windows Store
            // need to use other approaches to obtain a client certificate.
            StorageFile clientCertFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri(ClientCertUriPath));

            IBuffer buffer = await FileIO.ReadBufferAsync(clientCertFile);
            AppendOutputLine("Reading certificate succeeded.");

            string clientCertData = Windows.Security.Cryptography.CryptographicBuffer.EncodeToBase64String(buffer);

            try
            {
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
                await CertificateEnrollmentManager.ImportPfxDataAsync(
                    clientCertData,
                    ClientCertPassword,
                    ExportOption.Exportable,
                    KeyProtectionLevel.NoConsent,
                    InstallOptions.DeleteExpired,
                    ClientCertFriendlyName);

                AppendOutputLine("Installing certificate succeeded.");

                // Return the certificate we just instaled.
                return await FindCertificateFromStoreAsync();
            }
            catch (Exception ex)
            {
                // This can happen if the certificate has already expired.
                AppendOutputLine("Installing certificate failed with" + ex.Message);
                return null;
            }
        }

        private async Task<Certificate> GetClientCertificateAsync()
        {
            // The client certificate could be installed already (if this scenario has been run before),
            // try finding it from the store.
            Certificate cert = await FindCertificateFromStoreAsync();
            if (cert != null)
            {
                return cert;
            }

            // If the client certificate is not already installed, install it.
            return await InstallClientCertificateAsync();
        }

        private async Task ConnectWebSocketAsync()
        {
            // Validating the URI is required since it was received from an untrusted source (user input).
            // The URI is validated by calling TryGetUri() that will return 'false' for strings that are not
            // valid WebSocket URIs.
            // Note that when enabling the text box users may provide URIs to machines on the intrAnet
            // or intErnet. In these cases the app requires the "Home or Work Networking" or
            // "Internet (Client)" capability respectively.
            Uri server = rootPage.TryGetUri(ServerAddressField.Text);
            if (server == null)
            {
                return;
            }

            // Certificate validation is meaningful only for secure connections.
            if (server.Scheme != "wss")
            {
                AppendOutputLine("Note: Certificate validation is performed only for the wss: scheme.");
            }

            streamWebSocket = new StreamWebSocket();
            streamWebSocket.Closed += OnClosed;

            // It is okay to set the ClientCertificate property even if GetClientCertificateAsync returns null.
            streamWebSocket.Control.ClientCertificate = await GetClientCertificateAsync();

            // When connecting to wss:// endpoint, the OS by default performs validation of
            // the server certificate based on well-known trusted CAs. For testing purposes, we are ignoring
            // SSL errors.
            // WARNING: Only test applications should ignore SSL errors.
            // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
            // attacks. (Although the connection is secure, the server is not authenticated.)
            // Note that not all certificate validation errors can be ignored.
            // In this case, we are ignoring these errors since the certificate assigned to the localhost
            // URI is self-signed and has subject name = fabrikam.com
            streamWebSocket.Control.IgnorableServerCertificateErrors.Add(ChainValidationResult.Untrusted);
            streamWebSocket.Control.IgnorableServerCertificateErrors.Add(ChainValidationResult.InvalidName);

            AppendOutputLine($"Connecting to {server}...");

            try
            {
                await streamWebSocket.ConnectAsync(server);
            }
            catch (Exception ex) // For debugging
            {
                streamWebSocket.Dispose();
                streamWebSocket = null;

                AppendOutputLine(MainPage.BuildWebSocketError(ex));
                AppendOutputLine(ex.Message);
                return;
            }

            rootPage.NotifyUser("Connected", NotifyType.StatusMessage);
            AppendOutputLine("Connected to WebSocket Server.");
        }

        private void OnDisconnect()
        {
            SetBusy(true);
            rootPage.NotifyUser("Closing", NotifyType.StatusMessage);
            CloseSocket();
            SetBusy(false);
            rootPage.NotifyUser("Closed", NotifyType.StatusMessage);
        }

        // This may be triggered remotely by the server or locally by Close/Dispose()
        private async void OnClosed(IWebSocket sender, WebSocketClosedEventArgs args)
        {
            // Dispatch the event to the UI thread so we do not need to synchronize access to streamWebSocket.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                AppendOutputLine("Closed; Code: " + args.Code + ", Reason: " + args.Reason);

                if (streamWebSocket == sender)
                {
                    CloseSocket();
                    UpdateVisualState();
                }
            });
        }

        private void CloseSocket()
        {
            if (streamWebSocket != null)
            {
                try
                {
                    streamWebSocket.Close(1000, "Closed due to user request.");
                }
                catch (Exception ex)
                {
                    AppendOutputLine(MainPage.BuildWebSocketError(ex));
                    AppendOutputLine(ex.Message);
                }

                streamWebSocket = null;
            }
        }

        private void AppendOutputLine(string value)
        {
            OutputField.Text += value + "\r\n";
        }

    }
}
