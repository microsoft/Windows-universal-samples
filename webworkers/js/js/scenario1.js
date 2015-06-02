//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("startPrimes").addEventListener("click", calculatePrimes, false);
        },
        unload: function() {
            resetPrimes();
        }
    });

    //Scenario 1
    var primesCount = 0;
    var primesRunning = false;
    var primes1;
    var primes2;

    function resetPrimes() {
        var count = document.getElementById("primesCount");
        count.innerText = "0";
        primesCount = 0;
        if (primes1) {
            primes1.terminate();
        }
        if (primes2) {
            primes2.terminate();
        }
        primesRunning = false;
        document.getElementById("startPrimes").innerHTML = "Calculate Primes";
    }

    function displayPrime(e) {
        primesCount += e.data;
        if (primesCount >= 100000) {
            resetPrimes();
        }
        var count = document.getElementById("primesCount");
        count.innerText = primesCount;
    }

    function calculatePrimes() {
        if (primesRunning) {
            resetPrimes();
        } else {
            primes1 = new Worker('js/primes.js');
            primes2 = new Worker('js/primes.js');
            primes1.postMessage(2);
            primes1.onmessage = displayPrime;
            primes2.postMessage(3);
            primes2.onmessage = displayPrime;
            this.innerHTML = "Stop Calculating";
            primesRunning = true;
        }
    }
})();
