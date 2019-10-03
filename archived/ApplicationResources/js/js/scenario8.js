//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario8.html", {
        ready: function (element, options) {
            document.getElementById("scenario8Show").addEventListener("click", show, false);
        }
    });

    function show() {
        var currentData = WinJS.Binding.as({ count: document.getElementById('scenario8Binding').value });
        document.getElementById('scenario8Binding').addEventListener("keyup", function () {
            currentData.count = this.value;
        }, false);

        var el = document.getElementById('messageCount');
        WinJS.Resources.processAll(el).done(function(){
            WinJS.Binding.processAll(el, currentData);
        });
        
    }
})();
