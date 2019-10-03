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
        self.postMessage("Port Received.");
        self.postMessage("Received message from UI. Passing '" + e.data.message + "' to Worker 2.");
        e.ports[0].postMessage(e.data.message);
    };
})();
