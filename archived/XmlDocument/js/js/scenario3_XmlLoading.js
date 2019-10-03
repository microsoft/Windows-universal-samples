// *********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// *********************************************************

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3_XmlLoading.html", {
        ready: function (element, options) {
            document.getElementById("scenario3BtnFile").addEventListener("click", scenario3LoadFile, false);
            document.getElementById("scenario3BtnBuffer").addEventListener("click", scenario3LoadBuffer, false);

            scenario3Initialize();
        }
    });

    function scenario3Initialize() {
        var xml = document.getElementById("scenario3OriginalData");
        var result = document.getElementById("scenario3Result");

        xml.value = "";

        Windows.ApplicationModel.Package.current.installedLocation.getFolderAsync("loadExternaldtd").then(function (dtdFolder) {
            // Load xml file
            dtdFolder.getFileAsync("xmlWithExternaldtd.xml").done(function (file) {
                var loadSettings = new Windows.Data.Xml.Dom.XmlLoadSettings;
                loadSettings.prohibitDtd = false;
                loadSettings.resolveExternals = false;
                Windows.Data.Xml.Dom.XmlDocument.loadFromFileAsync(file, loadSettings).then(function (doc) {
                    xml.value = doc.getXml();
                });
            });
        });

        result.value = "";
    }

    function scenario3LoadFile() {
        // Get load settings
        var loadSettings = new Windows.Data.Xml.Dom.XmlLoadSettings;
        if (document.getElementById("scenario3Option1").checked) {
            loadSettings.prohibitDtd = true;          // DTD is prohibited
            loadSettings.resolveExternals = false;    // Disable the resolve to external definitions such as external DTD
        }
        if (document.getElementById("scenario3Option2").checked) {
            loadSettings.prohibitDtd = false;         // DTD is not prohibited
            loadSettings.resolveExternals = false;    // Disable the resolve to external definitions such as external DTD
        }
        if (document.getElementById("scenario3Option3").checked) {
            loadSettings.prohibitDtd = false;        // DTD is not prohibited
            loadSettings.resolveExternals = true;    // Enable the resolve to external definitions such as external DTD
        }

        // Load xml file with external DTD
        Windows.ApplicationModel.Package.current.installedLocation.getFolderAsync("loadExternaldtd").then(function (externalDtdFolder) {
            externalDtdFolder.getFileAsync("xmlWithExternaldtd.xml").done(function (file) {
                Windows.Data.Xml.Dom.XmlDocument.loadFromFileAsync(file, loadSettings).then(function (doc) {

                    document.getElementById("scenario3Result").value = doc.getXml();
                    document.getElementById("scenario3Result").style.color = "white";

                }, function (error) {    // An exception is thrown in loading xml file

                    // After loadSettings.ProhibitDtd is set to true, the exception is expected as the sample XML contains DTD
                    document.getElementById("scenario3Result").value = "Error: DTD is prohibited.";
                    document.getElementById("scenario3Result").style.color = "red";

                });
            });
        });
    }

    function scenario3LoadBuffer() {
        // Get load settings
        var loadSettings = new Windows.Data.Xml.Dom.XmlLoadSettings;
        if (document.getElementById("scenario3Option1").checked) {
            loadSettings.prohibitDtd = true;          // DTD is prohibited
            loadSettings.resolveExternals = false;    // Disable the resolve to external definitions such as external DTD
        }
        if (document.getElementById("scenario3Option2").checked) {
            loadSettings.prohibitDtd = false;         // DTD is not prohibited
            loadSettings.resolveExternals = false;    // Disable the resolve to external definitions such as external DTD
        }
        if (document.getElementById("scenario3Option3").checked) {
            loadSettings.prohibitDtd = false;        // DTD is not prohibited
            loadSettings.resolveExternals = true;    // Enable the resolve to external definitions such as external DTD
        }

        // Load xml file with external DTD
        try {
            var xml = document.getElementById("scenario3OriginalData").value;

            // Set external dtd file path
            if (loadSettings.resolveExternals & !loadSettings.prohibitDtd) {
                Windows.ApplicationModel.Package.current.installedLocation.getFolderAsync("loadExternaldtd").done(function (folder) {
                    var dtdPath = folder.path + "\\dtd.txt";
                    xml = xml.replace("dtd.txt", dtdPath);
                    loadXml(xml, loadSettings);
                });
            } else {
                loadXml(xml, loadSettings);
            }
        } catch (error) {    // An exception is thrown in loading xml file

            // After loadSettings.ProhibitDtd is set to true, the exception is expected as the sample XML contains DTD
            document.getElementById("scenario3Result").value = "Error: DTD is prohibited.";
            document.getElementById("scenario3Result").style.color = "red";
        }
    }

    function loadXml(xml, loadSettings)
    {
        var dataWriter = new Windows.Storage.Streams.DataWriter;
        dataWriter.writeString(xml);

        var ibuffer = dataWriter.detachBuffer();

        var xmlDoc = new Windows.Data.Xml.Dom.XmlDocument;
        xmlDoc.loadXmlFromBuffer(ibuffer, loadSettings);

        document.getElementById("scenario3Result").value = xmlDoc.getXml();
        document.getElementById("scenario3Result").style.color = "white";
    }
})();