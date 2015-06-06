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
    var page = WinJS.UI.Pages.define("/html/scenario5_XSLTTransformation.html", {
        ready: function (element, options) {
            document.getElementById("scenario5XsltTransformToString").addEventListener("click", scenario5XsltTransformToString, false);
            document.getElementById("scenario5XsltTransformToDocument").addEventListener("click", scenario5XsltTransformToDocument, false);

            xsltInitialize();
        }
    });

    function xsltInitialize() {
        var xml = document.getElementById("xml");
        var xslt = document.getElementById("xslt");
        var result = document.getElementById("result");

        xml.value = "";
        xslt.value = "";

        Windows.ApplicationModel.Package.current.installedLocation.getFolderAsync("xsltTransform").then(function (xsltFolder) {
            // Load xml file
            xsltFolder.getFileAsync("xmlContent.xml").done(function (file) {
                Windows.Data.Xml.Dom.XmlDocument.loadFromFileAsync(file).then(function (doc) {
                    xml.value = doc.getXml();
                });
            });

            // Load xslt file
            xsltFolder.getFileAsync("xslContent.xml").then(function (file) {
                Windows.Data.Xml.Dom.XmlDocument.loadFromFileAsync(file).done(function (doc) {
                    xslt.value = doc.getXml();
                });
            });
        });

        result.value = "";
    }

    function scenario5XsltTransformToString() {
        var xml = document.getElementById("xml");
        var xslt = document.getElementById("xslt");
        var result = document.getElementById("result");

        // Get xml content
        var doc = new Windows.Data.Xml.Dom.XmlDocument;
        try {
            if (xml.value === null || xml.value.trim() === "") {
                result.value = "Source XML can't be empty";
                result.style.color = "red";
                return;
            }
            doc.loadXml(xml.value);
        } catch (error) {
            xml.style.color = "red";
            result.value = error.description;
            result.style.color = "red";
            return;
        }

        // Get xsl content
        var xslDoc = new Windows.Data.Xml.Dom.XmlDocument;
        try {
            if (xslt.value === null || xslt.value.trim() === "") {
                result.value = "XSL content can't be empty";
                result.style.color = "red";
                return;
            }
            xslDoc.loadXml(xslt.value);
        } catch (error) {
            xslt.style.color = "red";
            result.value = error.description;
            result.style.color = "red";
            return;
        }

        // Transform xml according to the style sheet declaration specified in xslt file
        try {
            var xsltProcessor = new Windows.Data.Xml.Xsl.XsltProcessor(xslDoc);
            result.value = xsltProcessor.transformToString(doc);
        } catch (error) {
            result.value = error.description;
            result.style.color = "red";
            return;
        }
    }

    function scenario5XsltTransformToDocument() {
        var xml = document.getElementById("xml");
        var xslt = document.getElementById("xslt");
        var result = document.getElementById("result");

        // Get xml content
        var doc = new Windows.Data.Xml.Dom.XmlDocument;
        try {
            if (xml.value === null || xml.value.trim() === "") {
                result.value = "Source XML can't be empty";
                result.style.color = "red";
                return;
            }
            doc.loadXml(xml.value);
        } catch (error) {
            xml.style.color = "red";
            result.value = error.description;
            result.style.color = "red";
            return;
        }

        // Get xsl content
        var xslDoc = new Windows.Data.Xml.Dom.XmlDocument;
        try {
            if (xslt.value === null || xslt.value.trim() === "") {
                result.value = "XSL content can't be empty";
                result.style.color = "red";
                return;
            }
            xslDoc.loadXml(xslt.value);
        } catch (error) {
            xslt.style.color = "red";
            result.value = error.description;
            result.style.color = "red";
            return;
        }

        // Transform xml according to the style sheet declaration specified in xslt file
        try {
            var xsltProcessor = new Windows.Data.Xml.Xsl.XsltProcessor(xslDoc);
            doc = xsltProcessor.transformToDocument(doc);
            result.value = doc.getXml();
        } catch (error) {
            result.value = error.description;
            result.style.color = "red";
            return;
        }
    }
})();
