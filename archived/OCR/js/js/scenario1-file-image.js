//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Keep objects in-scope across the lifetime of the scenario.
    var FileToken = "";

    // Define namespace and API aliases.
    var FutureAccess = Windows.Storage.AccessCache.StorageApplicationPermissions.futureAccessList;
    var Globalization = Windows.Globalization;
    var Ocr = Windows.Media.Ocr;

    var page = WinJS.UI.Pages.define("/html/scenario1-file-image.html", {
        ready: function (element, options) {
            element.querySelector("#buttonLoad").addEventListener("click", loadHandler, false);
            element.querySelector("#buttonSample").addEventListener("click", sampleHandler, false);
            element.querySelector("#buttonExtract").addEventListener("click", extractHandler, false);
            element.querySelector("#languageList").addEventListener("change", languageChanged, false);
            element.querySelector("#userLanguageToggle").addEventListener("change", updateLanguages, false);

            updateLanguages();
            sampleHandler();
        }
    });

    // Updates list of ocr languages available on device.
    function updateLanguages() {
        clearResults();

        var userLangTogle = document.getElementById("userLanguageToggle");
        var languageList = document.getElementById("languageList");
        var extractButton = document.getElementById("buttonExtract");

        // Remove all available languages from list.
        while (languageList.options.length > 0) {
            languageList.remove(0);
        };

        var languages = Ocr.OcrEngine.availableRecognizerLanguages;

        // Check if any OCR language is available on device.
        if (languages.length > 0) {
            if (!userLangTogle.winControl.checked) {
                for (var i = 0; i < languages.length; i++) {

                    // Create option entry for every available language. 
                    var option = document.createElement("option");
                    option.text = languages[i].displayName;
                    option.value = languages[i].languageTag;
                    languageList.add(option);
                }

                userLangTogle.disabled = false;
                languageList.disabled = false;
                extractButton.disabled = false;

                languageChanged();
            } else {
                userLangTogle.disabled = false;
                languageList.disabled = true;
                extractButton.disabled = false;

                WinJS.log && WinJS.log(
                    "Run OCR in first OCR available language from UserProfile.GlobalizationPreferences.Languages list.",
                    "sample",
                    "status");
            }
        } else {
            // Prevent OCR if no OCR languages are available on device.
            userLangTogle.disabled = true;
            languageList.disabled = true;
            extractButton.disabled = true;

            WinJS.log && WinJS.log(
                "No available OCR languages.",
                "sample",
                "error");
        }
    }

    // Invoked when user select new language from language list.
    // Tries to change language for Optical Character Recognition.
    function languageChanged() {
        clearResults();

        var langTag = document.getElementById("languageList").value;
        if (langTag) {
            var lang = new Globalization.Language(langTag);

            WinJS.log && WinJS.log(
                "Selected OCR language is " + lang.displayName + "." +
                        Ocr.OcrEngine.availableRecognizerLanguages.length + " OCR language(s) are available." +
                        "Check combo box for full list.",
                 "sample",
                 "status");
        }
    }

    // Invoked when the user clicks on the Load button.
    function loadHandler() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
        picker.fileTypeFilter.append(".jpg");
        picker.fileTypeFilter.append(".jpeg");
        picker.fileTypeFilter.append(".png");

        picker.pickSingleFileAsync().done(function (file) {
            // The returned file is null if the user cancelled the picker.
            file && loadImage(file);
        });
    }

    // Invoked when the user clicks on the Sample button.
    function sampleHandler() {
        // Load sample "Hello World" image.
        Windows.ApplicationModel.Package.current.installedLocation.getFileAsync("images\\splash-sdk.png")
            .then(function (file) {
                loadImage(file);
            });
    }

    // Load an image from a file and display for preview before extracting text.
    function loadImage(file) {
        // Request persisted access permissions to the file the user selected.
        // This allows the app to directly load the file in the future without relying on a
        // broker such as the file picker.
        FileToken = FutureAccess.add(file);

        // Display image for preview.
        document.getElementById("imagePreview").src = window.URL.createObjectURL(file, { oneTimeOnly: true });

        // Clear results from previous OCR recognize.
        clearResults();

        // Update status message.
        file.properties.getImagePropertiesAsync().then(function (imageProps) {
            WinJS.log && WinJS.log("Loaded image from file " + file.displayName +
                                        " (" + imageProps.width + "x" + imageProps.height + ").",
                                    "sample",
                                    "status");
        });
    }

    // Clear results from previous OCR recognize.
    function clearResults() {
        document.getElementById("imagePreview").style.transform = "";
        document.getElementById("imagePreview").hidden = false;
        document.getElementById("textOverlay").innerHTML = "";
        document.getElementById("buttonExtract").disabled = languageList.options.length === 0;
        document.getElementById("imageText").textContent = "";
    }

    // Invoked when the user clicks on the Extract button.
    function extractHandler() {
        var ocrEngine = null;

        if (document.getElementById("userLanguageToggle").winControl.checked) {
            // Try to create OcrEngine for first supported language from UserProfile.GlobalizationPreferences.Languages list.
            // If none of the languages are available on device, method returns null.
            ocrEngine = Ocr.OcrEngine.tryCreateFromUserProfileLanguages();
        }
        else {
            // Try to create OcrEngine for specified language.
            // If language is not supported on device, method returns null.
            ocrEngine = Ocr.OcrEngine.tryCreateFromLanguage(new Globalization.Language(document.getElementById("languageList").value));
        }

        if (!ocrEngine) {
            WinJS.log && WinJS.log("Selected language is not available.", "sample", "status");
            return;
        }

        // Prevent another OCR request, since only image can be processed at the time at same OCR engine instance.
        document.getElementById("buttonExtract").winControl.disabled = true;

        var bitmap;
        FutureAccess.getFileAsync(FileToken).then(function (file) {
            return file.openAsync(Windows.Storage.FileAccessMode.read);
        }).then(function (stream) {
            var bitmapDecoder = Windows.Graphics.Imaging.BitmapDecoder;
            return bitmapDecoder.createAsync(stream);
        }).then(function (decoder) {
            // Check if OcrEngine supports image resolution.
            if (decoder.pixelWidth > Ocr.OcrEngine.maxImageDimension ||
                decoder.pixelHeight > Ocr.OcrEngine.maxImageDimension) {

                throw "Bitmap dimensions ({" + decoder.pixelWidth + "x" + decoder.pixelHeight + ") are too big for OCR.\n" +
                        "Max image dimension is " + Ocr.OcrEngine.maxImageDimension + "."
            }

            return decoder.getSoftwareBitmapAsync();
        }).then(function (bmp) {
            bitmap = bmp;
            return ocrEngine.recognizeAsync(bitmap);
        }).then(function (result) {
            var ocrText = document.getElementById("textOverlay");
            if (result.lines != null) {
                for (var l = 0; l < result.lines.length; l++) {
                    var line = result.lines[l];
                    for (var w = 0; w < line.words.length; w++) {
                        var word = line.words[w];

                        // Define "div" tag to overlay recognized word.
                        var wordbox = document.createElement("div");

                        wordbox.className = "result";
                        wordbox.style.width = 100 * word.boundingRect.width / bitmap.pixelWidth + "%";
                        wordbox.style.height = 100 * word.boundingRect.height / bitmap.pixelHeight + "%";
                        wordbox.style.top = 100 * word.boundingRect.y / bitmap.pixelHeight + "%";
                        wordbox.style.left = 100 * word.boundingRect.x / bitmap.pixelWidth + "%";

                        if (result.textAngle) {
                            // Rotate word for detected text angle.
                            wordbox.style.transform = "rotate(" + result.textAngle + "deg)";

                            // Calculate image/rotation center relative to word bounding box.
                            var centerX = -100 * (word.boundingRect.x - bitmap.pixelWidth / 2) / word.boundingRect.width;
                            var centerY = -100 * (word.boundingRect.y - bitmap.pixelHeight / 2) / word.boundingRect.height;
                            wordbox.style.transformOrigin = "" + centerX + "% " + centerY + "%";
                        }

                        // Put the filled word box in the results div.
                        ocrText.appendChild(wordbox);
                    }
                }
            }

            document.getElementById("imageText").textContent = result.text;

            WinJS.log && WinJS.log(
                "Image is OCRed for " + ocrEngine.recognizerLanguage.displayName + " language.",
                "sample",
                "status");

        }).then(null, function (e) {
            document.getElementById("imagetext").textcontent = "";
            WinJS.log && WinJS.log(e.message, "sample", "error");
        });
    }

    // Invoked when the user clicks on the Overlay button.
    // Check state of this this control determines whether extracted text will be overlaid over image. 
    function overlayHandler() {
        document.getElementById("textOverlay").hidden = !document.getElementById("overlay").checked;
    }
})();
