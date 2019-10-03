/// <reference group="Dedicated Worker" />
//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    self.onmessage = function (e) {
        self.postMessage("Port Received. Waiting for message...");
        e.ports[0].onmessage = function (ev) {
            self.postMessage("Received message from Worker 1: '" + ev.data + "'");
        };
    };
})();
