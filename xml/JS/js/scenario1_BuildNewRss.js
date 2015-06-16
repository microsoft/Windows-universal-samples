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
    var page = WinJS.UI.Pages.define("/html/scenario1_BuildNewRss.html", {
        ready: function (element, options) {
            document.getElementById("scenario1BtnDefault").addEventListener("click", scenario1BuildRss, false);

            scenario1Initialize();
        }
    });

    function scenario1Initialize() {
        loadXmlFile("buildRss", "rssTemplate.xml", "scenario1");
    }

    function scenario1BuildRss() {
        var data = document.getElementById("scenario1RssInput").value;
        if (data === "" || data === null) {
            document.getElementById("scenario1Result").value = "Please type in RSS content in the [RSS Content] box.";
            document.getElementById("scenario1Result").style.color = "red";
        } else {
            var doc = new Windows.Data.Xml.Dom.XmlDocument;
            doc.loadXml(document.getElementById("scenario1OriginalData").value);

            // Create a rss CDataSection and insert into DOM tree
            var cdata = doc.createCDataSection(data);
            var element = doc.getElementsByTagName("content").item(0);
            element.appendChild(cdata);

            document.getElementById("scenario1Result").value = doc.getXml();
            document.getElementById("scenario1Result").style.color = "white";
        }
    }
})();
