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

(function () {
    "use strict";

    const Certificates = Windows.Security.Cryptography.Certificates;
    const ChainValidationResult = Certificates.ChainValidationResult;

    // Local variables
    var streamWebSocket;
    var busy;

    // DOM elements
    var serverAddressField;
    var connectButton;
    var disconnectButton;
    var outputField;

    // Certificate information
    const clientCertUriPath = "ms-appx:///data/clientCert.pfx";
    const clientCertPassword = "1234";
    const clientCertFriendlyName = "WebSocketSampleClientCert";
    const clientCertIssuerName = "www.contoso.com";

    var page = WinJS.UI.Pages.define("/html/scenario3-clientCertificate.html", {
        ready: function (element, options) {
            serverAddressField = document.getElementById("serverAddressField");
            connectButton = document.getElementById("connectButton");
            disconnectButton = document.getElementById("disconnectButton");
            outputField = document.getElementById("outputField");

            connectButton.addEventListener("click", onConnect, false);
            disconnectButton.addEventListener("click", onDisconnect, false);

            updateVisualState();
        },
        unload: function () {
            closeSocket();
        }
    });

    function updateVisualState() {
        serverAddressField.disabled = busy || streamWebSocket;
        connectButton.disabled = busy || streamWebSocket;
        disconnectButton.disabled = busy || !streamWebSocket;
    }

    function setBusy(value) {
        busy = value;
        updateVisualState();
    }

    function onConnect() {
        setBusy(true);
        connectWebSocketAsync().done(function () {
            setBusy(false);
        });
    }

    function findCertificateFromStoreAsync() {
        // Find the client certificate for authentication. If not found, it means it has not been installed.
        var query = new Certificates.CertificateQuery();
        query.issuerName = clientCertIssuerName;
        query.friendlyName = clientCertFriendlyName;

        return Certificates.CertificateStores.findAllAsync(query).then(function (certs) {
            // This sample installs only one certificate, so if we find one, it must be ours.
            return certs.length && certs[0];
        });
    }

    function installClientCertificateAsync() {
        // Load the certificate from the clientCert.pfx file packaged with this sample.
        // This certificate has been signed with a trusted root certificate installed on the server.
        // The installation is done by running the setupServer.ps1 file, which should have been done
        // before running the app.
        // WARNING: Including a pfx file in the app package violates the Windows Store
        // certification requirements. We are shipping the pfx file with the package for demonstrating
        // the usage of client certificates. Apps that will be published through Windows Store
        // need to use other approaches to obtain a client certificate.
        return Windows.Storage.StorageFile.getFileFromApplicationUriAsync(new Windows.Foundation.Uri(clientCertUriPath)
        ).then(function (clientCertFile) {
            return Windows.Storage.FileIO.readBufferAsync(clientCertFile);
        }).then(function (buffer) {
            appendOutputLine("Reading certificate succeeded.");
            var clientCertData = Windows.Security.Cryptography.CryptographicBuffer.encodeToBase64String(buffer);

            // Install the certificate to the app's certificate store.
            // The app certificate store is removed when the app is uninstalled.
            //
            // To install a certificate to the CurrentUser\MY store, which is not app specific,
            // you need to use CertificateEnrollmentManager.UserCertificateEnrollmentManager.importPfxDataAsync().
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
            // In this case, the certificate will remain even when the app is uninstalled.
            return Certificates.CertificateEnrollmentManager.importPfxDataAsync(
                clientCertData,
                clientCertPassword,
                Certificates.ExportOption.exportable,
                Certificates.KeyProtectionLevel.noConsent,
                Certificates.InstallOptions.deleteExpired,
                clientCertFriendlyName);
        }).then(function () {
            appendOutputLine("Installing certificate succeeded.");

            // Return the certificate we just installed.
            return findCertificateFromStoreAsync();
        }, function error(e) {
            // This can happen if the certificate is corrupted or has already expired.
            appendOutputLine("Installing certificate failed: " + e.message);
        });
    }

    function getClientCertificateAsync() {
        // The client certificate could be installed already (if this scenario has been run before),
        // try finding it from the store.
        return findCertificateFromStoreAsync().then(function (cert) {
            // If the client certificate is not already installed, install and return it.
            return cert || installClientCertificateAsync();
        });
    }

    function connectWebSocketAsync() {
        // Validating the URI is required since it was received from an untrusted source (user
        // input). The URI is validated by calling validateAndCreateUri() that will return null
        // for strings that are not valid WebSocket URIs.
        // Note that when enabling the text box users may provide URIs to machines on the local network
        // or internet. In these cases the app requires the "Home or Work Networking" or
        // "Internet (Client)" capability respectively.
        var server = SdkSample.validateAndCreateUri(serverAddressField.value);
        if (!server) {
            return;
        }

        // Certificate validation is meaningful only for secure connections.
        if (server.schemeName !== "wss") {
            appendOutputLine("Note: Certificate validation is performed only for the wss: scheme.");
        }

        streamWebSocket = new Windows.Networking.Sockets.StreamWebSocket();
        streamWebSocket.addEventListener("closed", onClosed);

        return getClientCertificateAsync().then(function (cert) {
            // It is okay to set the ClientCertificate property even if GetClientCertificateAsync returns null.
            streamWebSocket.control.clientCertificate = cert;

            // When connecting to wss:// endpoint, the OS by default performs validation of
            // the server certificate based on well-known trusted CAs. For testing purposes, we are ignoring
            // SSL errors.
            // WARNING: Only test applications should ignore SSL errors.
            // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
            // attacks. (Although the connection is secure, the server is not authenticated.)
            // Note that not all certificate validation errors can be ignored.
            // In this case, we are ignoring these errors since the certificate assigned to the localhost
            // URI is self-signed and has subject name = fabrikam.com
            streamWebSocket.control.ignorableServerCertificateErrors.push(
                ChainValidationResult.untrusted,
                ChainValidationResult.invalidName);

            appendOutputLine("Connecting to: " + server.absoluteUri);

            return streamWebSocket.connectAsync(server);
        }).then(function () {
            WinJS.log && WinJS.log("Connected", "sample", "status");
            appendOutputLine("Connected to WebSocket Server.");
        }, function error(e) {
            streamWebSocket.close();
            streamWebSocket = null;

            appendOutputLine(SdkSample.buildWebSocketError(e));
            appendOutputLine(e.message);
        });
    }

    function onDisconnect() {
        setBusy(true);
        WinJS.log && WinJS.log("Closing", "sample", "status");
        closeSocket();
        setBusy(false);
    }

    function onClosed(e) {
        appendOutputLine("Closed; Code: " + e.code + " Reason: " + e.reason);
        if (streamWebSocket === e.target) {
            closeSocket();
            updateVisualState();
        }
    }

    function closeSocket() {
        if (streamWebSocket) {
            try {
                streamWebSocket.close(1000, "Closed due to user request.");
            } catch (error) {
                WinJS.log && WinJS.log(SdkSample.buildWebSocketError(error), "sample", "error");
                appendOutputLine(error.message);
            }
            streamWebSocket = null;
        }
    }

    function appendOutputLine(text) {
        outputField.innerText += "\r\n" + text;
    }
})();
