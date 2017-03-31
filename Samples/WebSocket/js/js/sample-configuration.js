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

    var WebSocketError = Windows.Networking.Sockets.WebSocketError;
    var WebErrorStatus = Windows.Web.WebErrorStatus;

    var sampleTitle = "WebSocket JS Sample";

    var scenarios = [
        { url: "/html/scenario1-utf8.html", title: "UTF-8 text messages" },
        { url: "/html/scenario2-binary.html", title: "Binary data stream" }
    ];

    // Look up the name for an enumeration member.
    function lookupEnumName(e, value) {
        for (var name in e) {
            if (e[name] === value) {
                return name;
            }
        }
        // No name available; just use the number.
        return value.toString();
    }

    function validateAndCreateUri(uriString) {
        var webSocketUri;
        try {
            webSocketUri = new Windows.Foundation.Uri(uriString);
        } catch (error) {
            WinJS.log && WinJS.log("Error: Invalid URI", "sample", "error");
            return null;
        }

        if (webSocketUri.fragment !== "") {
            WinJS.log && WinJS.log("Error: URI fragments not supported in WebSocket URIs.", "sample", "error");
            return null;
        }

        // Uri.schemeName returns the canonicalized scheme name so we can use case-sensitive, ordinal string
        // comparison.
        var scheme = webSocketUri.schemeName;
        if ((scheme !== "ws") && (scheme !== "wss")) {
            WinJS.log && WinJS.log("Error: WebSockets only support ws:// and wss:// schemes.", "sample", "error");
            return null;
        }

        return webSocketUri;
    }

    function buildWebSocketError(error) {
        var hResult = error.number;
        var status = WebSocketError.getStatus(hResult);

        // Normally we'd use the error number status to test for specific conditions we want to handle.
        // In this sample, we'll just output them for demonstration purposes.
        switch (status) {
            case WebErrorStatus.cannotConnect:
            case WebErrorStatus.notFound:
            case WebErrorStatus.requestTimeout:
                return "Cannot connect to the server. Please make sure " +
                        "to run the server setup script before running the sample.";

            case WebErrorStatus.unknown:
                return "COM error: " + hResult;

            default:
                return "Error: " + lookupEnumName(WebErrorStatus, status);
        }
    }

    // All the certificates in the certificate chain as well as the final certificate itself
    // must be valid.
    function areCertificateAndCertChainValidAsync(serverCert, certChain) {
        // Call isCertificateValidAsync for each certificate in certChain and for serverCert.
        var tasks = certChain.slice().concat(serverCert).map(isCertificateValidAsync);
        return WinJS.Promise.join(tasks).then(function (results) {
            // Return true if every result is true.
            return !results.includes(false);
        });
    }
    
    // This is a placeholder call to simulate long-running async calls. Note that this code runs synchronously as part  
    // of the SSL/TLS handshake. Avoid performing lengthy operations here - else, the remote server may terminate the 
    // connection abruptly. 
    function isCertificateValidAsync(serverCert) {
        return WinJS.Promise.timeout(100).then(function () {
            // In this sample, we check the issuer of the certificate - this is purely for illustration 
            // purposes and should not be considered as a recommendation for certificate validation.
            return serverCert.issuer == "www.fabrikam.com";
        });
    }

    //
    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        lookupEnumName: lookupEnumName,
        validateAndCreateUri: validateAndCreateUri,
        buildWebSocketError: buildWebSocketError,
        areCertificateAndCertChainValidAsync: areCertificateAndCertChainValidAsync
    });
})();
