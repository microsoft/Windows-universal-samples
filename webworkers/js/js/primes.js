/// <reference group="Dedicated Worker" />
//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var primes = new Array();
    var i;

    self.onmessage = function (e) {
        i = parseInt(e.data);

        //2 is a prime
        if (i === 2) {
            postMessage(1);
            i += 2;
        }

        calculatePrimes();
    };

    function isPrime(number) {
        //Check each number n=1 to sqrt(number) and see if any divide evenly into number, if so then not a prime!
        for (var n = 2; n <= Math.sqrt(number) ; n++) {
            if (number % n === 0) {
                return;
            }
        }
        primes.push(number);
        postMessage(1); //Tell the UI thread we found a prime.
    }

    function calculatePrimes() {
        isPrime(i);
        i += 2; //Each thread handles every other possible number.
        setTimeout(calculatePrimes, 0);
    }
})();
