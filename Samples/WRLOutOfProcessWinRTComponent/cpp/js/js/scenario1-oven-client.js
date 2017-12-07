//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1-oven-client.html", {
        ready: function (element, options) {
            document.getElementById("ovenClientStartButton").addEventListener("click", ovenClientRun, false);
        }
    });
    
    function ovenClientRun() {
        // Namespaces and classes are projected as JS objects
        var SampleNamespace = Microsoft.SDKSamples.Kitchen;

        // Component Creation
        var myOven = new SampleNamespace.Oven({ width: 2, height: 2, depth: 2 });

        // Getters and setters are accessed using property syntax
        printLn("Oven volume is: " + myOven.volume.toString());

        // Registering event listeners
        myOven.addEventListener("breadbaked", breadCompleteHandler1);
        myOven.addEventListener("breadbaked", breadCompleteHandler2);
        myOven.addEventListener("breadbaked", breadCompleteHandler3);

        // Event handlers can be removed just like standard DOM events
        myOven.removeEventListener("breadbaked", breadCompleteHandler3);

        // Bake a loaf of bread. This will trigger the BreadBaked event.
        // Method names are converted from PascalCase to camelCase in JS.
        myOven.bakeBread("Sourdough");

        // Trigger the event again with a different preheat time
        myOven.configurePreheatTemperature(SampleNamespace.High);
        myOven.bakeBread("Wheat");
    }

    function breadCompleteHandler1(evt) {
        // The sender of the event is mapped to evt.target
        printLn("Event Handler 1: Invoked");
        printLn("Event Handler 1: Oven volume is: " + evt.target.volume.toString());
        printLn("Event Handler 1: Bread flavor is: " + evt.bread.flavor);
    }

    function breadCompleteHandler2(evt) {
        printLn("Event Handler 2: Invoked");
    }

    function breadCompleteHandler3(evt) {
        // Event handler 3 was removed and will not be invoked
        printLn("Event Handler 3: Invoked");
    }

    function printLn(str) {
        document.getElementById("ovenClientOutput").innerHTML += str + "<br>";
    }
})();
