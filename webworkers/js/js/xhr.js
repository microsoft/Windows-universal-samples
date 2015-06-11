/// <reference group="Dedicated Worker" />
//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
importScripts('/Microsoft.WinJS.4.0/js/WinJS.js');

(function () {
    "use strict";

    var url;

    self.onmessage = function (e) {
        url = e.data.url;
        WinJS.xhr({ url: url }).then(onSuccess, onError, onProgress);
    };

    function onSuccess(e) {
        postMessage("Request finished and returned " + e.responseText.length + " characters of data.");
        postMessage("<hr>");
    }

    function onError(e) {
        postMessage("Request had an error");
        postMessage("<hr>");
    };

    function onProgress(e) {
      if (e.readyState === 2) {
        postMessage("Sending XMLHttpRequest to " + url);
      }
    };
})();
