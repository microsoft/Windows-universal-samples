//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Array containing all available local HostName endpoints
    var localHostItems = [];

    var page = WinJS.UI.Pages.define("/html/scenario1_Start.html", {
        ready: function (element, options) {
            document.getElementById("StartListener").addEventListener("click", startListener, false);
            document.getElementById("serviceNameAccept").addEventListener("change", socketsSample.getValues, false);
            document.getElementById("bindToAny").addEventListener("change", bindToAnyChanged, false);
            document.getElementById("bindToAddress").addEventListener("change", bindToAnyChanged, false);
            document.getElementById("bindToAdapter").addEventListener("change", bindToAnyChanged, false);

            socketsSample.hostNameConnect = "localhost";
            socketsSample.setValues();
            populateAdapterList();
        }
    });

    function bindToAnyChanged() {
        var bindToAny = document.getElementById("bindToAny");
        var adapterList = document.getElementById("adapterList");

        adapterList.disabled = bindToAny.checked;
    }

    function startListener() {
        if (socketsSample.listener) {
            WinJS.log("Already have a listener; call close to close the listener.", "", "error");
            return;
        }

        var serviceName = document.getElementById("serviceNameAccept").value;
        if (serviceName === "") {
            WinJS.log("Please provide a service name.", "", "error");
            return;
        }

        var bindToAny = document.getElementById("bindToAny").checked;
        var bindToAddress = document.getElementById("bindToAddress").checked;
        var bindToAdapter = document.getElementById("bindToAdapter").checked;
        var adapterListSelectedIndex = document.getElementById("adapterList").selectedIndex;

        var selectedLocalHost = localHostItems[adapterListSelectedIndex];

        socketsSample.closing = false;
        socketsSample.bindingToService = true;
        socketsSample.adapter = null;
        socketsSample.listener = new Windows.Networking.Sockets.StreamSocketListener(serviceName);
        socketsSample.listener.addEventListener("connectionreceived", onServerAccept);

        if (bindToAny) {
            // Don't limit traffic to an address or an adapter.
            socketsSample.listener.bindServiceNameAsync(serviceName).done(function () {
                WinJS.log("Listening", "", "status");
                socketsSample.bindingToService = false;
            }, onError);
        } else if (bindToAddress) {
            // The user selected an address. For demo purposes, we ensure that connect will be using the same address.
            socketsSample.hostNameConnect = selectedLocalHost.canonicalName;

            // Try to bind to a specific address.
            socketsSample.listener.bindEndpointAsync(selectedLocalHost, serviceName).done(function () {
                WinJS.log("Listening on address " + selectedLocalHost.canonicalName, "", "status");
                socketsSample.bindingToService = false;
            }, onError);
        } else if (bindToAdapter) {
            // The user selected an address. For demo purposes, we ensure that connect will be using the same address.
            socketsSample.hostNameConnect = selectedLocalHost.canonicalName;

            // For demo purposes, ensure that use the same adapter in the client connect scenario.
            socketsSample.adapter = selectedLocalHost.ipInformation.networkAdapter;

            // Try to limit traffic to the selected adapter. 
            // This option will be overridden by interfaces with weak-host or forwarding modes enabled.
            socketsSample.listener.bindServiceNameAsync(
                serviceName,
                Windows.Networking.Sockets.SocketProtectionLevel.plainSocket,
                socketsSample.adapter).done(function () {
                    WinJS.log("Listening on adapter " + socketsSample.adapter.networkAdapterId, "", "status");
                    socketsSample.bindingToService = false;
                }, onError);
        }
    }

    // This must be a real function; it will loop back on itself with the
    // call to acceptAsync at the very end.
    function onServerAccept(eventArgument) {
        socketsSample.serverSocket = eventArgument.socket;
        socketsSample.serverReader = new Windows.Storage.Streams.DataReader(socketsSample.serverSocket.inputStream);
        startServerRead();
    }

    // The protocol here is simple: a four-byte 'network byte order' (big-endian) integer
    // that says how long a string is, and then a string that is that long.
    // We wait for exactly 4 bytes, read in the count value, and then wait for
    // count bytes, and then display them.
    function startServerRead() {
        socketsSample.serverReader.loadAsync(4).done(function (sizeBytesRead) {
            // Make sure 4 bytes were read.
            if (sizeBytesRead !== 4) {
                WinJS.log("Server: connection lost.", "", "error");
                return;
            }

            // Read in the 4 bytes count and then read in that many bytes.
            var count = socketsSample.serverReader.readInt32();
            return socketsSample.serverReader.loadAsync(count).then(function (stringBytesRead) {
                // Make sure the whole string was read.
                if (stringBytesRead !== count) {
                    WinJS.log("Server: connection lost.", "", "error");
                    return;
                }
                // Read in the string.
                var string = socketsSample.serverReader.readString(count);
                socketsSample.displayOutput("Received data: \"" + string + "\"");

                // Restart the read for more bytes. We could just call startServerRead() but in
                // the case subsequent read operations complete synchronously we start building
                // up the stack and potentially crash. We use WinJS.Promise.timeout() to invoke
                // this function after the stack for the current call unwinds.
                WinJS.Promise.timeout().done(function () { return startServerRead(); });
            }); // End of "read in rest of string" function.
        }, onError);
    }

    function onError(reason) {
        // Clean up a listener if we failed to bind to a port.
        if (socketsSample.bindingToService) {
            socketsSample.listener = null;
            socketsSample.adapter = null;
            socketsSample.bindingToService = false;
        }

        // When we close a socket, outstanding async operations will be canceled and the
        // error callbacks called.  There's no point in displaying those errors.
        if (!socketsSample.closing) {
            WinJS.log(reason, "", "error");
        }
    }

    function populateAdapterList() {
        // Clear both the localHostItems list and the select element.
        localHostItems.length = 0;
        var adapterList = document.getElementById("adapterList");
        while (adapterList.options.length > 0) {
            adapterList.options.remove(0);
        }

        // Populate the local address and adapter pairs collection.
        var localHostNames = Windows.Networking.Connectivity.NetworkInformation.getHostNames();
        for (var i = 0; i < localHostNames.length; i++) {
            if (localHostNames[i].ipInformation !== null) {
                localHostItems.push(localHostNames[i]);
                var option = document.createElement("option");
                option.textContent = "Address: " + localHostNames[i].displayName +
                    " Adapter: " + localHostNames[i].ipInformation.networkAdapter.networkAdapterId;
                option.value.textContent = localHostItems.length - 1;
                adapterList.appendChild(option);
            }
        }
    }
})();
