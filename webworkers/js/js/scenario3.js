//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var timersWorker = new Worker('js/timers.js');

    timersWorker.onmessage = function (e) {
        document.getElementById("timerLog")
          .appendChild(document.createElement('p'))
          .textContent = e.data;
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

    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function () {
            document.getElementById("addTimeout2000").addEventListener("click", scheduleTimeout2000);
            document.getElementById("addInterval3000").addEventListener("click", scheduleInterval3000);
            document.getElementById("clearTimers").addEventListener("click", clearAllTimers);
            document.getElementById("clearTimerLog").addEventListener("click", function () {
                document.getElementById("timerLog").innerHTML = "";
            });
        },
        unload: clearAllTimers
    });
})();
