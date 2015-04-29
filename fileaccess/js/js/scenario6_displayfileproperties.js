//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario6_DisplayFileProperties.html", {
        ready: function (element, options) {
            document.getElementById("showProperties").addEventListener("click", showProperties, false);
            SdkSample.validateFileExistence();
        }
    });

    var dateAccessedProperty = "System.DateAccessed";
    var fileOwnerProperty    = "System.FileOwner";

    function showProperties() {
        if (SdkSample.sampleFile !== null) {
            // Get top level file properties
            var outputText = "Filename: " + SdkSample.sampleFile.name;
            outputText += "\nFile type: " + SdkSample.sampleFile.fileType;

            // Get basic properties
            SdkSample.sampleFile.getBasicPropertiesAsync().then(function (basicProperties) {
                outputText += "\nSize: " + basicProperties.size + " bytes";
                outputText += "\nDate modified: " + basicProperties.dateModified;

                // Get extra properties
                return SdkSample.sampleFile.properties.retrievePropertiesAsync([fileOwnerProperty, dateAccessedProperty]);
            }).done(function (extraProperties) {
                var propValue = extraProperties[dateAccessedProperty];
                if (propValue) {
                    outputText += "\nDate accessed: " + propValue;
                }
                propValue = extraProperties[fileOwnerProperty];
                if (propValue) {
                    outputText += "\nFile owner: " + propValue;
                }

                WinJS.log && WinJS.log(outputText, "sample", "status");
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }
})();
