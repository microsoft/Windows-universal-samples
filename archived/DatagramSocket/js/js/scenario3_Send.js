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

    var page = WinJS.UI.Pages.define("/html/scenario3_Send.html", {
        ready: function (element, options) {
            document.getElementById("buttonSend").addEventListener("click", sendHello, false);
        }
    });

    function sendHello(eventObject) {
        if (!socketsSample.connected) {
            WinJS.log && WinJS.log("Client: you must connect the client before using it.", "sample", "error");
            return;
        }

        if (socketsSample.clientDataWriter === null) {
            socketsSample.clientDataWriter =
                new Windows.Storage.Streams.DataWriter(socketsSample.clientSocket.outputStream);
        }

        // Prepare the message to be sent. Writing data to the writer will just store data in memory.
        var string = "Hello World";
        socketsSample.clientDataWriter.writeString(string);

        // Write the locally buffered data to the network.
        socketsSample.clientDataWriter.storeAsync().done(function () {
            document.getElementById("labelSendOutput").innerHTML = "\"" + string + "\" sent successfully.";
        }, onError);
    }

    function onError(reason) {
        // When we close a socket, outstanding async operations will be canceled and the
        // error callbacks called.  There's no point in displaying those errors.
        if (!socketsSample.closing) {
            WinJS.log && WinJS.log(reason, "sample", "error");
        }
    }
})();
