//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

/// <reference path="HTMLCanvasElement.js"/>

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/CanvasBlob-Image.html", {
        ready: function (element, options) {
            drawCanvas();
            document.getElementById("reset").addEventListener("click", /*@static_cast(EventListener)*/reset, false);
            document.getElementById("showInImage").addEventListener("click", /*@static_cast(EventListener)*/showInImage, false);
            reset();
        },
        
        unload: function () {
            reset();
        }
    });

    function reset() {
        WinJS.log && WinJS.log("", "sample", "error");
        document.getElementById("imageHolder").src = "/images/placeholder-sdk.png";

    }

    function drawCanvas() {
        // Get the canvas element using the DOM
        var canvas = /*@static_cast(HTMLCanvasElement)*/ document.getElementById("canvas");

        // Use getContext to use the canvas for drawing
        var context = /*@static_cast(CanvasRenderingContext2D)*/ canvas.getContext("2d");
        context.strokeStyle = "#336699";

        // Draw shape
        context.beginPath();
        context.moveTo(75, 25);
        context.quadraticCurveTo(25, 25, 25, 62.5);
        context.quadraticCurveTo(25, 100, 50, 100);
        context.quadraticCurveTo(50, 120, 30, 125);
        context.quadraticCurveTo(60, 120, 65, 100);
        context.quadraticCurveTo(125, 100, 125, 62.5);
        context.quadraticCurveTo(125, 25, 75, 25);
        context.stroke();
    }

    function showInImage() {
        var canvas = /*@static_cast(HTMLCanvasElement)*/ document.getElementById("canvas");

        // Convert Canvas to blob and display in image tag
        var blob = canvas.msToBlob();
        var url = URL.createObjectURL(blob, {oneTimeOnly: true});
        document.getElementById("imageHolder").src = url;

        // Show status
        WinJS.log && WinJS.log("The image src has been set to: " + url, "sample", "status");
    }
})();
