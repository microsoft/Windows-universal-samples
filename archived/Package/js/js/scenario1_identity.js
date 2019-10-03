//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1_Identity.html", {
        ready: function (element, options) {
            document.getElementById("identityGetPackage").addEventListener("click", identityGetPackage, false);
            identityDisplayOutput();
        }
    });

    function versionString(version) {
        return "" + version.major + "." + version.minor + "." + version.build + "." + version.revision;
    }

    function architectureString(architecture) {
        switch (architecture)
        {
            case Windows.System.ProcessorArchitecture.x86:
                return "x86";
            case Windows.System.ProcessorArchitecture.arm:
                return "arm";
            case Windows.System.ProcessorArchitecture.x64:
                return "x64";
            case Windows.System.ProcessorArchitecture.neutral:
                return "neutral";
            case Windows.System.ProcessorArchitecture.unknown:
                return "unknown";
            default:
                return "???";
        }
    }

    function identityGetPackage() {
        // NOTE: 'package' is a Javascript reserved word so don't use it as a variable name
        var pkg = Windows.ApplicationModel.Package.current;
        var packageId = pkg.id;

        // Package only supports IPackage2 on Windows
        var output = [ "Name: \"" + packageId.name + "\"",
                       "Version: " + versionString(packageId.version),
                       "Architecture: " + architectureString(packageId.architecture),
                       "ResourceId: \"" + packageId.resourceId + "\"",
                       "Publisher: \"" + packageId.publisher + "\"",
                       "PublisherId: \"" + packageId.publisherId + "\"",
                       "FullName: \"" + packageId.fullName + "\"",
                       "FamilyName: \"" + packageId.familyName + "\"",
                       "IsFramework: " + pkg.isFramework
                     ];
        if ('isResourcePackage' in pkg) {
            output.push("IsResourcePackage: " + pkg.isResourcePackage,
                       "IsBundle: " + pkg.isBundle,
                       "IsDevelopmentMode: " + pkg.isDevelopmentMode + "\"",
                       "DisplayName: \"" + pkg.displayName + "\"",
                       "PublisherDisplayName: \"" + pkg.publisherDisplayName + "\"",
                       "Description: \"" + pkg.description + "\"",
                       "Logo: \"" + pkg.logo.absoluteUri+ "\"");
        }

        var html = output.join("<BR>");

        identityDisplayOutput(html);
    }

    function identityDisplayOutput(output) {
        output = output || "";

        document.getElementById("identityOutput").innerHTML = output;
    }
})();
