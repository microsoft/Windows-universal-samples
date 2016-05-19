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

    // Local variables
    var streamWebSocket;
    var busy;

    // DOM elements
    var serverAddressField;
    var startButton;
    var stopButton;
    var outputField;

    var page = WinJS.UI.Pages.define("/html/scenario2-binary.html", {
        ready: function (element, options) {
            serverAddressField = document.getElementById("serverAddressField");
            startButton = document.getElementById("startButton");
            stopButton = document.getElementById("stopButton");
            outputField = document.getElementById("outputField");

            startButton.addEventListener("click", onStart, false);
            stopButton.addEventListener("click", onStop, false);

            updateVisualState();
        },
        unload: function () {
            closeSocket();
        }
    });

    function updateVisualState() {
        serverAddressField.disabled = busy || streamWebSocket;
        startButton.disabled = busy || streamWebSocket;
        stopButton.disabled = busy || !streamWebSocket;
    }

    function setBusy(value) {
        busy = value;
        updateVisualState();
    }

    function onStart() {
        setBusy(true);
        startAsync().done(function () {
            setBusy(false);
        });
    }

    function startAsync() {
        // Validating the URI is required since it was received from an untrusted source (user
        // input). The URI is validated by calling validateAndCreateUri() that will return null
        // for strings that are not valid WebSocket URIs.
        // Note that when enabling the text box users may provide URIs to machines on the local network
        // or internet. In these cases the app requires the "Home or Work Networking" or
        // "Internet (Client)" capability respectively.
        var server = SdkSample.validateAndCreateUri(serverAddressField.value);
        if (!server) {
            return WinJS.Promise.wrap();
        }

        streamWebSocket = new Windows.Networking.Sockets.StreamWebSocket();
        streamWebSocket.addEventListener("closed", onClosed);

        appendOutputLine("Connecting to: " + server.absoluteUri);

        return streamWebSocket.connectAsync(server).then(function () {
            WinJS.log && WinJS.log("Connected", "sample", "status");

            // Start a loop to continuously read incoming data.
            receiveData(streamWebSocket);

            // Start a loop to continuously write outgoing data.
            sendData(streamWebSocket);
        }, function (error) {
            streamWebSocket.close();
            streamWebSocket = null;

            appendOutputLine(SdkSample.buildWebSocketError(error));
            appendOutputLine(error.message);
        });
    }

    function sendData(activeSocket) {
        var dataWriter = new Windows.Storage.Streams.DataWriter(activeSocket.outputStream);
        var bytesSent = 0;
        var data = "Hello World";

        function loopAsync() {
            if (streamWebSocket != activeSocket) {
                // Our socket is no longer active. Stop sending.
                return;
            }
            var size = dataWriter.measureString(data);

            dataWriter.writeString(data);
            return dataWriter.storeAsync().then(function () {
                bytesSent += size;
                dataSent.innerText = bytesSent;

                // Add a 1 second delay so the user can see what's going on.
                return WinJS.Promise.timeout(1000);
            }).then(loopAsync);
        }

        loopAsync().then(function () {
        }, function (error) {
            appendOutputLine("During write: " + SdkSample.buildWebSocketError(error));
            appendOutputLine(error.message);
        }).done(function () {
            dataWriter.close();
        });
    }

    function receiveData(activeSocket) {
        var dataReader = new Windows.Storage.Streams.DataReader(activeSocket.inputStream);
        var bytesReceived = 0;

        // When buffering, return as soon as any data is available.
        dataReader.inputStreamOptions = Windows.Storage.Streams.InputStreamOptions.partial;

        function loopAsync() {
            if (streamWebSocket != activeSocket) {
                // Our socket is no longer active. Stop reading.
                appendOutputLine("Background read stopped.");
                return;
            }
            // Buffer as much data as you require for your protocol.
            return dataReader.loadAsync(100).then(function (sizeBytesRead) {
                bytesReceived += sizeBytesRead;
                dataReceived.innerText = bytesReceived;

                var incomingBytes = new Array(sizeBytesRead);
                dataReader.readBytes(incomingBytes);

                // Do something with the data.
                // Alternatively you can use DataReader to read out individual booleans,
                // ints, strings, etc.

                // Start another read.
                return loopAsync();
            });
        }

        loopAsync().then(function () {
        }, function (error) {
            appendOutputLine("During read: " + SdkSample.buildWebSocketError(error));
            appendOutputLine(error.message);
        }).done(function () {
            dataReader.close();
        });
    }

    function onStop() {
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
