//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2_InstalledLocation.html", {
        ready: function (element, options) {
            document.getElementById("installedLocationGetInstalledLocation").addEventListener("click", installedLocationGetInstalledLocation, false);
            installedLocationDisplayOutput();
        }
    });

    function installedLocationGetInstalledLocation() {
        // This will give the Installed Location.  You can use this location to access files, if you need them.
        var pkg = Windows.ApplicationModel.Package.current;
        var installedLocation = pkg.installedLocation;

        var html = "Installed Location: " + installedLocation.path;

        installedLocationDisplayOutput(html);
    }

    function installedLocationDisplayOutput(output) {
        output = output || "";

        document.getElementById("installedLocationOutput").innerHTML = output;
    }
})();
