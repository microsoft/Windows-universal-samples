//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var progressText = "";
    var page = WinJS.UI.Pages.define("/html/stream.html", {
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

            document.getElementById("scenarioProgress").innerHTML = progressText;
        }
    });

    WinJS.Application.onactivated = function (args) {
        if (Windows.ApplicationModel.Activation.ActivationKind.pickFileContinuation === args.detail.kind) {
            try {
                var algorithmName = args.detail.continuationData["algorithmName"];
                var file = args.detail.files[0];

                if (file === null) {
                    throw "No file has been selected";
                }

                compressFile(algorithmName, args.detail.files[0]);
            }
            catch (e) {
                onError(e);
            }
        }
    };

    function onError(e) {
        WinJS.log && WinJS.log(e, "sample", "error");
    }

    function clearProgress() {
        WinJS.log && WinJS.log("Testing...", "sample", "status");

        var progress = document.getElementById("scenarioProgress");
        progressText = "";
        progress.innerHTML = "";
    }

    function showProgress(message) {
        var progress = document.getElementById("scenarioProgress");

        progressText += "<p>" + message + "</p>";
        progress.innerHTML = progressText;
    }

    function getEnumerationValueName(namespace, value, defaultName) {
        for (var /*@override*/name in namespace) {
            if (namespace[name] === value) {
                return name;
            }
        }

        return defaultName;
    }

    function compressFile(algorithmName, file) {

        try {

            var compressor;

            //
            // Note, that file could be automatically renamed to prevent collision
            //
            var compressedFileName = file.name + "." + algorithmName + ".compressed";
            var decompressedFileName = file.name + "." + algorithmName + ".decompressed";

            var compressAlgorithm = Windows.Storage.Compression.CompressAlgorithm[algorithmName];

            //
            // It is safe to pass-through undefined value as compressAlgorithm in which case default algorithm (Xpress) will be used.
            // For the purposes of this sample both usages are shown.
            //
            if (typeof (compressAlgorithm) === "undefined") {
                //
                // If you don't have any specific algorithm requirements - this is recommended way to initialize Compressor object
                //
                compressor = new WinJS.Compressor(compressedFileName);
            } else {
                compressor = new WinJS.Compressor(compressedFileName, compressAlgorithm);
            }

            compressor.compressAsync(file).then(function () {
                showProgress("Compression done");
                compressor.close();

                try {
                    //
                    // We cannot use compressedFileName here because of automatic renaming
                    //
                    var decompressor = new WinJS.Decompressor(compressor.fileName);

                    return decompressor.copyAsync(decompressedFileName).then(function () {
                        showProgress("Decompression done");
                        decompressor.close();
                    }, onError);
                } catch (e) {
                    onError(e);
                }
            }, onError).done(function () {
                WinJS.log && WinJS.log("Test finished", "sample", "status");
            }, onError);
        }
        catch (e) {
            onError(e);
        }
    }

    //
    // Sample for bulk stream compression and decompression
    //
    function doScenario(compressAlgorithm) {

        var filePicker = new Windows.Storage.Pickers.FileOpenPicker;
        var algorithmName = getEnumerationValueName(Windows.Storage.Compression.CompressAlgorithm, compressAlgorithm, "default");

        filePicker.fileTypeFilter.append("*");

        filePicker.pickSingleFileAsync().then(function (file) {
            try {
                //
                // If user cancels file picker pickSingleFileAsync succeeds with result set to null
                //
                if (file === null) {
                    throw "No file has been selected";
                }

                var compressor;

                //
                // Note, that file could be automatically renamed to prevent collision
                //
                var compressedFileName = file.name + "." + algorithmName + ".compressed";
                var decompressedFileName = file.name + "." + algorithmName + ".decompressed";

                //
                // It is safe to pass-through undefined value as compressAlgorithm in which case default algorithm (Xpress) will be used.
                // For the purposes of this sample both usages are shown.
                //
                if (typeof (compressAlgorithm) === "undefined") {
                    //
                    // If you don't have any specific algorithm requirements - this is recommended way to initialize Compressor object
                    //
                    compressor = new WinJS.Compressor(compressedFileName);
                } else {
                    compressor = new WinJS.Compressor(compressedFileName, compressAlgorithm);
                }

                compressor.compressAsync(file).then(function () {
                    showProgress("Compression done");
                    compressor.close();

                    try {
                        //
                        // We cannot use compressedFileName here because of automatic renaming
                        //
                        var decompressor = new WinJS.Decompressor(compressor.fileName);

                        return decompressor.copyAsync(decompressedFileName).then(function () {
                            showProgress("Decompression done");
                            decompressor.close();
                        }, onError);
                    } catch (e) {
                        onError(e);
                    }
                }, onError).done(function () {
                    WinJS.log && WinJS.log("Test finished", "sample", "status");
                }, onError);
            } catch (e) {
                onError(e);
            }
        }, onError);
    }
})();
