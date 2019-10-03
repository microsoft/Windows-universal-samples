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

    var page = WinJS.UI.Pages.define("/html/scenario5_Certificates.html", {
        ready: function (element, options) {
            document.getElementById("buttonConnectSocket").addEventListener("click", openClient, false);
        }
    });

    function openClient(eventObject) {
        var serviceName = document.getElementById("serviceNameConnect").value;
        if (serviceName === "") {
            WinJS.log && WinJS.log("Please provide a service name.", "", "error");
            return;
        }

        // Validating the host name is required since it was received from an untrusted source (user input).
        // The host name is validated by catching exceptions thrown by the HostName constructor.
        var hostName;
        try {
            hostName = new Windows.Networking.HostName(document.getElementById("hostNameConnect").value);
        } catch (error) {
            WinJS.log && WinJS.log("Error: Invalid host name.", "", "error");
            return;
        }

        connectSocketWithRetry(hostName, serviceName);
    }

    function connectSocketWithRetry(hostName, serviceName) {
        var closing = false;
        var clientSocket = new Windows.Networking.Sockets.StreamSocket();
        WinJS.log && WinJS.log("Connecting to: " + hostName.displayName, "", "status");

        // If necessary, tweak the socket's control options before carrying out the connect operation.
        // Refer to the StreamSocketControl class' MSDN documentation for the full list of control options.
        clientSocket.control.clientCertificate = null;

        var abortedByUser = false;

        // Establish a secure connection to the server (by default, the local IIS server).
        clientSocket.connectAsync(
            hostName,
            serviceName,
            Windows.Networking.Sockets.SocketProtectionLevel.tls12)
        .then(function () {
            // No SSL errors: return an empty promise and continue processing in the .done function
            return;
        }, function (reason) {
            // If the exception was caused by an SSL error that is ignorable we are going to prompt the user
            // with an enumeration of the errors and ask for permission to ignore.
            if (clientSocket.information.serverCertificateErrorSeverity ===
                Windows.Networking.Sockets.SocketSslErrorSeverity.ignorable) {
                return shouldIgnoreCertificateErrorsAsync(
                    clientSocket.information.serverCertificateErrors)
                    .then(function (userAcceptedRetry) {
                        if (userAcceptedRetry) {
                            return connectSocketWithRetryHandleSslErrorAsync(clientSocket, hostName, serviceName);
                        }

                        throw new Error("Connection aborted by user (certificate not trusted)");
                    });
            }

            // Handle other exceptions in done().
            throw reason;
        })
        .done(function () {
            // Get detailed certificate information.
            var certInformation = getCertificateInformation(
                clientSocket.information.serverCertificate,
                clientSocket.information.serverIntermediateCertificates);

            WinJS.log && WinJS.log("Connected to server. Certificate information:\r\n" + certInformation, "", "status");

            clientSocket.close();
            clientSocket = null;
        }, function (reason) {
            // If this is an unknown status it means that the error is fatal and retry will likely fail.
            if (("number" in reason) &&
                (Windows.Networking.Sockets.SocketError.getStatus(reason.number) ===
                Windows.Networking.Sockets.SocketErrorStatus.unknown)) {
                throw reason;
            }

            WinJS.log && WinJS.log(reason, "", "error");
            clientSocket.close();
            clientSocket = null;
        });
    }

    function connectSocketWithRetryHandleSslErrorAsync(clientSocket, hostName, serviceName) {
        // -----------------------------------------------------------------------------------------------
        // WARNING: Only test applications should ignore SSL errors.
        // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle attacks.
        // -----------------------------------------------------------------------------------------------

        clientSocket.control.ignorableServerCertificateErrors.clear();
        for (var i = 0; i < clientSocket.information.serverCertificateErrors.length; i++) {
            clientSocket.control.ignorableServerCertificateErrors.append(
                clientSocket.information.serverCertificateErrors[i]);
        }

        WinJS.log && WinJS.log("Retrying connection", "", "status");
        return clientSocket.connectAsync(
            hostName,
            serviceName,
            Windows.Networking.Sockets.SocketProtectionLevel.tls12);
    }

    function shouldIgnoreCertificateErrorsAsync(connectionErrors) {
        var connectionErrorString = getCertificateErrorDescription(connectionErrors[0]);
        for (var i = 1; i < connectionErrors.length; i++) {
            connectionErrorString += ", " + getCertificateErrorDescription(connectionErrors[i]);
        }

        var dialogMessage =
            "The remote server certificate validation failed with the following errors: " +
            connectionErrorString + "\r\nSecurity certificate problems may" +
            " indicate an attempt to fool you or intercept any data you send to the server.";

        var dialog = new Windows.UI.Popups.MessageDialog(dialogMessage, "Server Certificate Validation Errors");

        var continueButtonId = 1;
        var abortButtonId = 0;
        dialog.commands.append(new Windows.UI.Popups.UICommand("Continue (not recommended)", null, continueButtonId));
        dialog.commands.append(new Windows.UI.Popups.UICommand("Cancel", null, abortButtonId));
        dialog.defaultCommandIndex = 1;
        dialog.cancelCommandIndex = 1;

        return dialog.showAsync().then(function (selected) {
            return (selected.id === continueButtonId);
        });
    }

    function getCertificateInformation(serverCert, intermediateCertificates) {
        var certificateInfoString = "";
        certificateInfoString += "Friendly Name: " + serverCert.friendlyName + "\r\n";
        certificateInfoString += "Subject: " + serverCert.subject + "\r\n";
        certificateInfoString += "Issuer: " + serverCert.issuer + "\r\n";
        certificateInfoString += "Validity: " + serverCert.validFrom + " - " + serverCert.validTo + "\r\n";

        // Enumerate the entire certificate chain
        if (intermediateCertificates.length > 0) {
            certificateInfoString += "Certificate chain:\r\n";

            for (var i = 0; i < intermediateCertificates.length; i++) {
                var cert = intermediateCertificates[i];
                certificateInfoString += "Intermediate Certificate Subject: " + cert.subject + "\r\n";
            }
        } else {
            certificateInfoString += "No certificates within the intermediate chain.\r\n";
        }

        return certificateInfoString;
    }

    function getCertificateErrorDescription(errorNum)
    {
        for (var stringName in Windows.Security.Cryptography.Certificates.ChainValidationResult) {
            if (Windows.Security.Cryptography.Certificates.ChainValidationResult[stringName] === errorNum) {
                return stringName;
            }
        }

        return String(null);
    }

})();
