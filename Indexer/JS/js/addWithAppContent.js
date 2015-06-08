//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/addWithAppContent.html", {
        ready: function (element, options) {
            document.getElementById("addToIndex").addEventListener("click", IndexerHelpers.addAppContentFilesToIndexedFolder, false);
        }
    });
})();