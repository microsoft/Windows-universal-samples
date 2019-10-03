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

    var sampleTitle = "XML Document";

    var scenarios = [
        { url: "/html/scenario1_BuildNewRss.html", title: "Build New RSS" },
        { url: "/html/scenario2_MarkHotProducts.html", title: "DOM Load/Save" },
        { url: "/html/scenario3_XmlLoading.html", title: "Set Load Settings" },
        { url: "/html/scenario4_GiftDispatch.html", title: "XPath Query" },
        { url: "/html/scenario5_XSLTTransformation.html", title: "XSLT Transformation" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();

function loadXmlFile(foldername, filename, scenarioNumber) {
    var defaultBtn = document.getElementById(scenarioNumber + "BtnDefault");
    var originalData = document.getElementById(scenarioNumber + "OriginalData");
    var output = document.getElementById(scenarioNumber + "Result");

    Windows.ApplicationModel.Package.current.installedLocation.getFolderAsync(foldername).then(function (folder) {
        folder.getFileAsync(filename).done(function (file) {
            var loadSettings = new Windows.Data.Xml.Dom.XmlLoadSettings;
            loadSettings.prohibitDtd = false;
            loadSettings.resolveExternals = false;
            Windows.Data.Xml.Dom.XmlDocument.loadFromFileAsync(file, loadSettings).then(function (doc) {
                originalData.value = doc.getXml();
                defaultBtn.disabled = false;
            }, function (error) {
                output.value = "Error: Unable to load XML file";
                output.style.color = "red";
            });
        }, function (error) {
            output.value = error.description;
            output.style.color = "red";
        });
    }, function (error) {
        output.value = error.description;
        output.style.color = "red";
    });
};