//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            document.getElementById("addTimeout2000").addEventListener("click", scheduleTimeout2000, false);
            document.getElementById("addInterval3000").addEventListener("click", scheduleInterval3000, false);
            document.getElementById("clearTimers").addEventListener("click", clearAllTimers, false);
            document.getElementById("clearTimerLog").addEventListener("click", function () {
                document.getElementById("timerLog").innerHTML = "";
            }, false);
        },
        unload: function() {
            clearAllTimers();
        }
    });

    var timersWorker = new Worker('js/timers.js');

    timersWorker.onmessage = function (e) {
        document.getElementById("timerLog").innerHTML += e.data + "<br>";
    };

    function scheduleTimeout2000() {
        timersWorker.postMessage({
            typeOfTimer: "timeout",
            delay: 2000
        });
    }
    function scheduleInterval3000() {
        timersWorker.postMessage({
            typeOfTimer: "interval",
            delay: 3000
        });
    }
    function clearAllTimers() {
        timersWorker.postMessage({
            typeOfTimer: "clear",
            delay: null
        });
    }
})();
