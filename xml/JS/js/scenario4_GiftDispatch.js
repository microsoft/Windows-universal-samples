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
    var page = WinJS.UI.Pages.define("/html/scenario4_GiftDispatch.html", {
        ready: function (element, options) {
            document.getElementById("scenario4BtnDefault").addEventListener("click", scenario4Dispatch, false);
            scenario4Initialize();
        }
    });

    function scenario4Initialize() {
        loadXmlFile("giftDispatch", "employees.xml", "scenario4");
    }
    function scenario4Dispatch() {
        var doc = new Windows.Data.Xml.Dom.XmlDocument;
        doc.loadXml(document.getElementById("scenario4OriginalData").value);

        var currentDate = new Date();
        var thisYear = 2012;    // We don't use currentDate.getFullYear() to get current year so that all gifts can be delivered.
        var previousOneYear = thisYear - 1;
        var previousFiveYear = thisYear - 5;
        var previousTenYear = thisYear - 10;

        var xpath = [];
        // Select >= 1 year and < 5 years
        xpath[0] = "descendant::employee[startyear <= " + previousOneYear + " and startyear > " + previousFiveYear + "]";
        // Select >= 5 years and < 10 years
        xpath[1] = "descendant::employee[startyear <= " + previousFiveYear + " and startyear > " + previousTenYear + "]";
        // Select >= 10 years
        xpath[2] = "descendant::employee[startyear <= " + previousTenYear + "]";

        var gifts = [];
        gifts.fill("Gift Card", "XBOX", "Windows Phone");

        var output = [];
        for (var i = 0; i < 3; i++) {
            var employees = doc.selectNodes(xpath[i]);
            for (var index = 0; index < employees.length; index++) {
                var employeeName = employees.item(index).selectSingleNode("descendant::name");
                var department = employees.item(index).selectSingleNode("descendant::department");

                output.push("[", employeeName.firstChild.nodeValue, "]/[", department.firstChild.nodeValue, "]/[", gifts[i], "]\n");
            }
        }

        document.getElementById("scenario4Result").value = output.join("");
    }
})();
