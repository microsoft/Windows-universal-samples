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

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        lookupEnumName: lookupEnumName,
        validateAndCreateUri: validateAndCreateUri,
        buildWebSocketError: buildWebSocketError
    });
})();
