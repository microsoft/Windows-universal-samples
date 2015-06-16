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
    var page = WinJS.UI.Pages.define("/html/scenario2_MarkHotProducts.html", {
        ready: function (element, options) {
            document.getElementById("scenario2BtnDefault").addEventListener("click", scenario2Mark, false);
            document.getElementById("scenario2BtnSave").addEventListener("click", scenario2Save, false);

            scenario2Initialize();
        }
    });

    function scenario2Initialize() {
        loadXmlFile("markHotProducts", "products.xml", "scenario2");
    }

    // This function will look up products and mark hot products
    function scenario2Mark() {
        var doc = new Windows.Data.Xml.Dom.XmlDocument;
        doc.loadXml(document.getElementById("scenario2OriginalData").value);

        // Mark 'hot' attribute to '1' if 'sell10days' is greater than 'InStore'
        var xpath = "/products/product[Sell10day>InStore]/@hot";
        var hotAttributes = doc.selectNodes(xpath);
        for (var index = 0; index < hotAttributes.length; index++) {
            hotAttributes.item(index).nodeValue = "1";
        }

        document.getElementById("scenario2Result").value = doc.getXml();
        document.getElementById("scenario2BtnSave").disabled = false;  // Enable Save button
    }

    // This function is to save the new xml in which hot products are marked to a file.
    function scenario2Save() {
        var doc = new Windows.Data.Xml.Dom.XmlDocument;
        doc.loadXml(document.getElementById("scenario2Result").value);

        // Save xml to a file
        Windows.Storage.ApplicationData.current.localFolder.createFileAsync("HotProducts.xml", Windows.Storage.CreationCollisionOption.generateUniqueName).then(function (file) {
            doc.saveToFileAsync(file).done(function () {
                document.getElementById("scenario2Result").value = "Save to \"" + file.path + "\" successfully.";
            }, function (error) {    // An exception is gotten in saving file
                document.getElementById("scenario2Result").value = "Fail to save file";
                document.getElementById("scenario2Result").style.color = "red";
            });
        }, function (error) {    // An exception is gotten in creating file
            document.getElementById("scenario2Result").value = error.description;
            document.getElementById("scenario2Result").style.color = "red";
        });

        document.getElementById("scenario2BtnSave").disabled = true;
    }
})();
