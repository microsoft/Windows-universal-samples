//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var displayInformation;
    var standardBox;
    var overrideBox;
    var overrideFont;

    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            displayInformation = Windows.Graphics.Display.DisplayInformation.getForCurrentView();
            standardBox = document.getElementById("standardBox");
            overrideBox = document.getElementById("overrideBox");
            overrideFont = document.getElementById("overrideFont");

            window.addEventListener("resize", initialize, false);
            displayInformation.addEventListener("dpichanged", initialize);
            initialize();
        },
        unload: function () {
            window.removeEventListener("resize", initialize, false);
            displayInformation.removeEventListener("dpichanged", initialize);
        }
    });

    function pxToPt(px) {
        return px * 72 / 96;
    }

    function roundToTenths(v) {
        return Math.round(v * 10) / 10;
    }

    function initialize() {
        // Get the effective size
        var rect = document.documentElement.getBoundingClientRect();
        document.getElementById("effectiveResolutionValue").textContent = roundToTenths(rect.width) + "x" + roundToTenths(rect.height);

        var rawPixelsPerViewPixel = displayInformation.rawPixelsPerViewPixel;

        // Set the override rectangle size and override text font size by taking our desired
        // size in raw pixels and converting it to view pixels.
        var rectSizeInRawPx = 100;
        var rectSizeInViewPx = rectSizeInRawPx / rawPixelsPerViewPixel;
        overrideBox.style.width = rectSizeInViewPx + "px";
        overrideBox.style.height = rectSizeInViewPx + "px";

        var fontSizeInRawPt = 20;
        var fontSizeInViewPt = fontSizeInRawPt / rawPixelsPerViewPixel;
        overrideFont.style.fontSize = fontSizeInViewPt + "pt";

        // Report the values we set.
        var viewPixels = standardBox.getBoundingClientRect().width;
        document.getElementById("standardBoxPhysicalPixels").textContent = Math.round(viewPixels * rawPixelsPerViewPixel) + " raw px";
        document.getElementById("standardBoxRelativePixels").textContent = roundToTenths(viewPixels) + " view px";

        viewPixels = overrideBox.getBoundingClientRect().width;
        document.getElementById("overrideBoxPhysicalPixels").textContent = Math.round(viewPixels * rawPixelsPerViewPixel) + " raw px";
        document.getElementById("overrideBoxRelativePixels").textContent = roundToTenths(viewPixels) + " view px";

        var fontSize = roundToTenths(pxToPt(parseFloat(getComputedStyle(overrideFont).fontSize)));
        overrideFont.textContent = fontSize + "pt";
    }
})();
