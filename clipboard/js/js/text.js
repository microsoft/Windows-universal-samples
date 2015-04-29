//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/text.html", {
        ready: function (element, options) {
            document.getElementById("scenario1Copy").addEventListener("click", scenario1Copy, false);
            document.getElementById("scenario1Paste").addEventListener("click", scenario1Paste, false);
        }
    });

    function displayStatus(statusString) {
        return WinJS.log && WinJS.log(statusString, "sample", "status");
    }

    function displayError(errorString) {
        return WinJS.log && WinJS.log(errorString, "sample", "error");
    }

    function scenario1ClearOutput() {
        displayError("");
        displayStatus("");
        document.getElementById("scenario1TextOutput").innerText = "";
        document.getElementById("scenario1HtmlOutput").innerHTML = "";
    }

    function scenario1Copy() {
        scenario1ClearOutput();

        // create the datapackage
        var dataPackage = new Windows.ApplicationModel.DataTransfer.DataPackage();

        // add the content of the "scenario1TextToCopy" element in (plain) text format
        var text = document.getElementById("scenario1TextToCopy").innerText;
        dataPackage.setText(text);

        // add the content of the "scenario1TextToCopy" element in HTML format
        // 1st step - get the HTML content of the element
        var htmlFragment = document.getElementById("scenario1TextToCopy").innerHTML;

        // 2nd step - convert to HTML format
        var htmlFormat = Windows.ApplicationModel.DataTransfer.HtmlFormatHelper.createHtmlFormat(htmlFragment);
        if (htmlFormat !== "") {
            // 3rd step - add HTML format to DataPackage
            dataPackage.setHtmlFormat(htmlFormat);

            // 4th step - populate resourceMap with StreamReference objects corresponding to local image files embedded in HTML
            var imagePath = document.getElementById("scenario1LocalImage").src;
            var imageUri = new Windows.Foundation.Uri(imagePath);
            var streamRef = Windows.Storage.Streams.RandomAccessStreamReference.createFromUri(imageUri);
            dataPackage.resourceMap.insert(imagePath, streamRef);
        }

        try {
            // copy the content to Clipboard
            Windows.ApplicationModel.DataTransfer.Clipboard.setContent(dataPackage);

            displayStatus("Text and HTML formats have been copied to Clipboard");
        } catch (e) {
            // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
            displayError("Error copying content to Clipboard: " + e + ". Try again.");
        }
    }

    function scenario1Paste() {
        scenario1ClearOutput();

        var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.getContent();

        // paste contents of plain text format (if present)
        if (dataPackageView.contains(Windows.ApplicationModel.DataTransfer.StandardDataFormats.text)) {
            dataPackageView.getTextAsync().done(function (text) {
                document.getElementById("scenario1TextOutput").innerText = text;
            }, function (e) {
                displayError("Error retrieving Text format from Clipboard: " + e);
            });
        } else {
            document.getElementById("scenario1TextOutput").innerText = "Text format is not available in clipboard";
        }

        // paste contents of HTML format (if present)
        if (dataPackageView.contains(Windows.ApplicationModel.DataTransfer.StandardDataFormats.html)) {
            // 1st step - get the HTML Format (CF_HTML) from DataPackageView
            dataPackageView.getHtmlFormatAsync().done(function (htmlFormat) {
                // 2nd step - extract HTML fragment from HTML Format
                var htmlFragment = Windows.ApplicationModel.DataTransfer.HtmlFormatHelper.getStaticFragment(htmlFormat);

                // 3rd step - add the fragment to DOM
                document.getElementById("scenario1HtmlOutput").innerHTML = htmlFragment;

                // 4th step - replace local image file URLs (if any) with the contents of the resourceMap
                var images = document.getElementById("scenario1HtmlOutput").getElementsByTagName("img");
                // Check first if there are any images (IMG tags) in the fragment, as getResourceMapAsync can be an expensive operation
                if (images.length > 0) {
                    dataPackageView.getResourceMapAsync().done(function (resourceMap) {
                        // Check if the resourceMap contains any items - it can be empty
                        if (resourceMap.size > 0) {
                            for (var i = 0, len = images.length; i < len; i++) {
                                var imageElement = images[i];
                                // Lookup RandomAccessStreamReference value corresponding to this image's SRC property
                                var streamRef = resourceMap.lookup(imageElement.getAttribute("src"));
                                if (streamRef) {
                                    // Generate blob URL, and replace contents of the SRC property
                                    replaceSrcURL(imageElement, streamRef);
                                }
                            }
                        }
                    });
                }
            }, function (e) {
                displayError("Error retrieving HTML format from Clipboard: " + e);
            });
        } else {
            document.getElementById("scenario1HtmlOutput").innerText = "HTML format is not available in clipboard";
        }
    }

    // Replaces the value of image element's SRC property with a blob URL generated from the contents of an image stream
    function replaceSrcURL(imageElement, streamRef) {
        // Map the image element's src to a corresponding blob URL generated from the streamReference
        streamRef.openReadAsync().done(function (imageStream) {
            var blobObject = window.MSApp.createBlobFromRandomAccessStream(imageStream.contentType, imageStream);
            var blobUrl = window.URL.createObjectURL(blobObject, { oneTimeOnly: true });
            imageElement.src = blobUrl;
        }, function (e) {
            displayError("Error opening stream corresponding to the image element with SRC=\"" + imageElement.getAttribute("src") + "\". Error: " + e);
        });
    }
})();
