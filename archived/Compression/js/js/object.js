//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/object.html", {
        ready: function (element, options) {
            function scenario(algorithm) {
                return function () {
                    clearProgress();
                    if (typeof (algorithm) === "undefined") {
                        // Use default algorithm
                        doScenario();
                    } else {
                        doScenario(Windows.Storage.Compression.CompressAlgorithm[algorithm]);
                    }
                };
            }

            document.getElementById("DefaultButton").addEventListener("click", scenario(), false);
            document.getElementById("XpressButton").addEventListener("click", scenario("xpress"), false);
            document.getElementById("XpressHuffButton").addEventListener("click", scenario("xpresshuff"), false);
            document.getElementById("MszipButton").addEventListener("click", scenario("mszip"), false);
            document.getElementById("LzmsButton").addEventListener("click", scenario("lzms"), false);
        }
    });

    function onError(e) {
        WinJS.log && WinJS.log(e, "sample", "error");
    }

    function clearProgress() {
        WinJS.log && WinJS.log("Testing...", "sample", "status");

        var progress = document.getElementById("scenarioProgress");
        progress.innerHTML = "";
    }

    function showProgress(message) {
        var progress = document.getElementById("scenarioProgress");
        progress.innerHTML += "<p>" + message + "</p>";
    }

    function getEnumerationValueName(namespace, value, defaultName) {
        for (var /*@override*/name in namespace) {
            if (namespace[name] === value) {
                return name;
            }
        }

        return defaultName;
    }

    //
    // Sample for structured data compression and decompression
    //
    function doScenario(compressAlgorithm) {
        //
        // NOTE: Small object is used for demonstration purposes only. Normally object like this
        // would be too small to be effectively compressed because there is not enough entropy
        // to remove and any potential gains from compression would be negated by format metadata.
        //
        var testObject = [1, 2, 3, "sampletext", { value: "sampleobject" }];

        try {
            //
            // NOTE: Read and write positions are shared when IInputStream and IOutputStream
            // are queried directly from IRandomAccessStream, so we need either rewind stream
            // before reading or get separate streams using getInput/OutputStreamAt
            //
            var stream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
            var compressor;

            //
            // It is safe to pass-through undefined value as compressAlgorithm in which case default
            // algorithm (Xpress) will be used. For the purposes of this sample both usages are shown.
            //
            if (typeof (compressAlgorithm) === "undefined") {
                //
                // If you don't have any specific algorithm requirements - this is recommended way to
                // initialize Compressor object
                //
                compressor = new WinJS.Compressor(stream.getOutputStreamAt(0));
            } else {
                compressor = new WinJS.Compressor(stream.getOutputStreamAt(0), compressAlgorithm);
            }

            var decompressor = new WinJS.Decompressor(stream.getInputStreamAt(0));

            showProgress("Compressor object created");
            compressor.compressAsync(testObject).then(function () {
                showProgress("Test object compressed");
                compressor.close();
                return decompressor.readObjectAsync();
            }, onError).then(function (decompressedObject) {
                decompressor.close();
                showProgress("Test object decompressed: " + decompressedObject);
                if (JSON.stringify(testObject) === JSON.stringify(decompressedObject)) {
                    showProgress("Test object matches decompressed one");
                } else {
                    onError(new Error("Test object doesn't match decompressed one"));
                }
            }, onError).done(function () {
                WinJS.log && WinJS.log("Test finished", "sample", "status");
            }, onError);

        } catch (e) {
            onError(e);
        }
    }
})();
