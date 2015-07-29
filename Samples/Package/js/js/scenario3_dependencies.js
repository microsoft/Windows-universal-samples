//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3_Dependencies.html", {
        ready: function (element, options) {
        document.getElementById("dependenciesGetDependencies").addEventListener("click", dependenciesGetDependencies, false);
        dependenciesDisplayOutput();
        }
    });

    function dependenciesGetDependencies() {
        var pkg = Windows.ApplicationModel.Package.current;
        var dependencies = pkg.dependencies;

        var output = [ "Count: " + dependencies.length ];
        for (var i = 0; i < dependencies.length; i++) {
            var dependency = pkg.dependencies[i];
            output.push("[" + i + "]: " + dependency.id.fullName);
        }

        var html = output.join("<BR>");

        dependenciesDisplayOutput(html);
    }

    function dependenciesDisplayOutput(output) {
        output = output || "";

        document.getElementById("dependenciesOutput").innerHTML = output;
    }

})();
