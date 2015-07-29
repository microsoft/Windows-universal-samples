var strings;
(function () {
    'use strict';

    function showMessage() {
        var statusDiv = document.getElementById("statusMessage");

        var str = WinJS.Resources.getString('scenario3Message');
        statusDiv.textContent = str.value;
        statusDiv.lang = str.lang;
    }

    document.addEventListener("DOMContentLoaded", function () {

        // WinRT is not available in the web compartment, so we must load strings ourselves
        // File based resources can be used to load the correct strings
        WinJS.xhr({ url: '/strings/resources.json' }).done(function (response) {
            strings = JSON.parse(response.responseText);

            WinJS.Resources.processAll();
            showMessage();
        });
        
    }, false);

})();
