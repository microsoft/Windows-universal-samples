/// <reference group="Dedicated Worker" />
//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
(function() {
  "use strict";
  var timers = [];
  var intervals = [];

  onmessage = function (e) {

    switch (e.data.typeOfTimer) {
      case "timeout":
        timers.push(setTimeout(sendTimerMessage, e.data.delay));
        break;
      case 'interval':
        intervals.push(setInterval(sendIntervalMessage, e.data.delay));
        break;
      case 'clear':
        timers.forEach(clearTimeout);
        intervals.forEach(clearInterval);
        break;
    }
  };

  function sendTimerMessage() {
    postMessage("Timeout fired: " + new Date());
  }

  function sendIntervalMessage() {
    postMessage("Interval fired: " + new Date());
  }
}());
