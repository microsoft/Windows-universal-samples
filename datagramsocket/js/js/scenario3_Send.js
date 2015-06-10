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

        var string = "Hello World";
        socketsSample.clientDataWriter.writeString(string);

        WinJS.log && WinJS.log("Client sending: " + string + ".", "sample", "status");
        socketsSample.clientDataWriter.storeAsync().done(function () {
            WinJS.log && WinJS.log("Client sent: " + string + ".", "sample", "status");
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
