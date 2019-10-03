//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Commonly-used types.
    var StandardDataFormats = Windows.ApplicationModel.DataTransfer.StandardDataFormats;
    var HtmlFormatHelper = Windows.ApplicationModel.DataTransfer.HtmlFormatHelper;
    var QuickLink = Windows.ApplicationModel.DataTransfer.ShareTarget.QuickLink;

    // Variable to store the ShareOperation object
    var shareOperation = null;

    // Variable to store the visibility of the Extended Sharing section
    var extendedSharingCollapsed = true;

    // Variable to store the custom format string
    var customFormatName = "http://schema.org/Book";

    /// <summary>
    /// Helper function to set an element's inner text to a value.
    /// </summary>
    function setInnerText(id, value) {
        document.getElementById(id).innerText = value;
    }

    /// <summary>
    /// Helper function to display textual information in an element.
    /// </summary>
    /// <param name="container">
    /// The element in which to add new information.
    /// </param>
    /// <param name="label">
    /// A text label to show before the text content.
    /// </param>
    /// <param name="text">
    /// The text content to display.
    /// </param>
    /// <param name="className">
    /// Optional class name to assign to the div.
    /// </param>
    function addTextContent(container, label, text, className) {
        var div = document.createElement("div");
        var labelNode = document.createElement("strong");
        labelNode.innerText = label;
        div.appendChild(labelNode);
        var span = document.createElement("span");
        span.innerText = ": " + text;
        div.appendChild(span);
        if (className) {
            div.className = className;
        }
        return container.appendChild(div);
    }

    function addError(container, label, message, e) {
        return addTextContainer(container, label, "Error retrieving data: " + e, "error");
    }

    /// <summary>
    /// Helper function to display bitmap information in an element.
    /// Returns the div wrapper for the image.
    /// </summary>
    /// <param name="container">
    /// The element in which to add new information.
    /// </param>
    /// <param name="label">
    /// A text label to show before the text content.
    /// </param>
    /// <param name="streamReference">
    /// A random access stream reference containing the bitmap.
    /// </param>
    /// <param name="className">
    /// Class to assign to the div that wraps the image.
    /// </param>
    function addImageContent(container, label, streamReference, className) {
        addTextContent(container, label, "");
        var div = document.createElement("div");
        div.className = className;
        streamReference.openReadAsync().done(function (stream) {
            var img = document.createElement("img");
            img.src = URL.createObjectURL(stream, { oneTimeOnly: true });
            div.appendChild(img);
        }, function (e) {
            div.innerText = "Error reading image stream: " + e;
        });
        return container.appendChild(div);
    }

    function addContent(label, text) {
        var contentValue = document.getElementById("contentValue");
        addTextContent(contentValue, label, text);
    }
    function displayError(label, errorString) {
        return addContent(label, errorString);
    }

    /// <summary>
    /// Handler executed on activation of the target
    /// </summary>
    /// <param name="eventArgs">
    /// Arguments of the event. In the case of the Share contract, it has the ShareOperation
    /// </param>
    function activatedHandler(eventObject) {
        // In this sample we only do something if it was activated with the Share contract
        if (eventObject.detail.kind === Windows.ApplicationModel.Activation.ActivationKind.shareTarget) {
            eventObject.setPromise(WinJS.UI.processAll());

            // We receive the ShareOperation object as part of the eventArgs
            shareOperation = eventObject.detail.shareOperation;

            // We queue an asychronous event so that working with the ShareOperation object does not
            // block or delay the return of the activation handler.
            WinJS.Application.queueEvent({ type: "shareready" });
        }
    }

    /// <summary>
    /// Display properties of the data package.
    /// </summary>
    function showProperties() {
        var container = document.getElementById("dataPackageProperties");
        var properties = shareOperation.data.properties;

        addTextContent(container, "Title", properties.title);
        addTextContent(container, "Description", properties.description);
        addTextContent(container, "Package Family Name", properties.packageFamilyName);

        if (properties.contentSourceWebLink) {
            addTextContent(container, "Content Source Web Link", properties.contentSourceWebLink.rawUri);
        }

        if (properties.contentSourceApplicationLink) {
            addTextContent(container, "Content Source Application Link", properties.contentSourceApplicationLink.rawUri);
        }

        if (properties.square30x30Logo) {
            var div = addImageContent(container, "Logo", properties.square30x30Logo, "package-logo");
            var backgroundColor = properties.logoBackgroundColor;
            div.style.backgroundColor = "rgba(" + backgroundColor.r + "," + backgroundColor.g + "," + backgroundColor.b + "," + backgroundColor.a + ")";
        }

        if (properties.thumbnail) {
            addImageContent(container, "Thumbnail", properties.thumbnail, "thumbnail");
        }
    }

    /// <summary>
    /// Handle the incoming shared data. This should be done outside the activation handler.
    /// </summary>
    function shareReady() {
        var data = shareOperation.data;

        // Display properties of the data package.
        showProperties();

        // If this app was activated via a QuickLink, display the QuickLinkId
        if (shareOperation.quickLinkId) {
            document.getElementById("selectedQuickLinkId").innerText = shareOperation.quickLinkId;
            document.getElementById("quickLinkArea").className = "hidden";
        }

        // Display the data received based on data type
        var container = document.getElementById("dataPackageContents");

        if (data.contains(StandardDataFormats.text)) {
            data.getTextAsync().done(function (text) {
                addTextContent(container, "Text", text);
            }, function (e) {
                addError(container, "Text", e);
            });
        }
        if (data.contains(StandardDataFormats.webLink)) {
            data.getWebLinkAsync().done(function (webLink) {
                addTextContent(container, "WebLink", webLink.rawUri);
            }, function (e) {
                addError(container, "WebLink", e);
            });
        }
        if (data.contains(StandardDataFormats.applicationLink)) {
            data.getApplicationLinkAsync().done(function (applicationLink) {
                addTextContent(container, "ApplicationLink", applicationLink.rawUri);
            }, function (e) {
                addError(container, "ApplicationLink", e);
            });
        }
        if (data.contains(StandardDataFormats.storageItems)) {
            data.getStorageItemsAsync().done(function (storageItems) {
                var itemCount = storageItems.size;
                // Display info about the first 10 files.
                if (storageItems.size > 10) {
                    storageItems = storageItems.slice(0, 10);
                }
                var fileList = storageItems.map(item => item.name).join(", ");
                if (itemCount > 10) {
                    fileList += ` and ${itemCount - 10} more`;
                }
                addTextContent(container, "Storage Items", fileList);
            }, function (e) {
                addError(container, "Storage Items", e);
            });
        }
        if (data.contains(StandardDataFormats.bitmap)) {
            data.getBitmapAsync().done(function (bitmapStreamReference) {
                addImageContent(container, "Bitmap", bitmapStreamReference, "package-bitmap");
            }, function (e) {
                addError(container, "Bitmap", e);
            });
        }

        if (data.contains(StandardDataFormats.html)) {
            data.getHtmlFormatAsync().done(function (htmlFormat) {
                addTextContent(container, "HTML", "");

                // Extract the HTML fragment from the HTML format
                var htmlFragment = HtmlFormatHelper.getStaticFragment(htmlFormat);

                // Create an iframe and add it to the content.
                var iframe = document.createElement("iframe");
                iframe.style.width = "600px";
                container.appendChild(iframe);

                // Set the innerHTML of the iframe to the HTML fragment
                iframe.contentDocument.documentElement.innerHTML = htmlFragment;

                // Now we loop through any images and use the resourceMap to map each image element's src
                var images = iframe.contentDocument.documentElement.getElementsByTagName("img");
                if (images.length > 0) {
                    data.getResourceMapAsync().done(function (resourceMap) {
                        Array.prototype.forEach.call(images, function (image) {
                            var streamReference = resourceMap[image.src];
                            if (streamReference) {
                                // Call a helper function to map the image element's src to a corresponding blob URL generated from the streamReference
                                setResourceMapURL(streamReference, image);
                            }
                        });
                    }, function (e) {
                        addError(container, "Resource Map", e);
                    });
                }
            }, function (e) {
                addError(container, "HTML", e);
            });
        }

        if (data.contains(customFormatName)) {
            data.getTextAsync(customFormatName).done(function (customFormatString) {
                var customFormatObject = {};
                try {
                    customFormatObject = JSON.parse(customFormatString);
                } catch (e) {
                    // invalid JSON
                }
                // This sample expects the custom format to be of type http://schema.org/Book
                if (customFormatObject.type === "http://schema.org/Book") {
                    var lines = ["Type: " + customFormatObject.type];
                    var properties = customFormatObject.properties;
                    if (properties) {
                        lines.push(
                            `Image: ${properties.image}`,
                            `Name: ${properties.name}`,
                            `Book Format: ${properties.bookFormat}`,
                            `Author: ${properties.author}`,
                            `Number of Pages: ${properties.numberOfPages}`,
                            `Publisher: ${properties.publisher}`,
                            `Date Published: ${properties.datePublished}`,
                            `In Language: ${properties.inLanguage}`,
                            `ISBN: ${properties.isbn}`);
                    }
                    addTextContent(container, "Custom data", lines.join("\n"));
                } else {
                    addError(container, "Custom data", "Malformed data");

                }
            }, function (e) {
                addError(container, "Custom data", e);
            });
        }
    }

    /// <summary>
    /// Sets the blob URL for an image element based on a reference to an image stream within a resource map
    /// </summary>
    function setResourceMapURL(streamReference, imageElement) {
        if (streamReference) {
            streamReference.openReadAsync().done(function (imageStream) {
                if (imageStream) {
                    imageElement.src = URL.createObjectURL(imageStream, { oneTimeOnly: true });
                }
            }, function (e) {
                imageElement.alt = "Failed to load";
            });
        }
    }

    /// <summary>
    /// Use to simulate that an extended share operation has started
    /// </summary>
    function reportStarted() {
        shareOperation.reportStarted();
    }

    /// <summary>
    /// Use to simulate that an extended share operation has retrieved the data
    /// </summary>
    function reportDataRetrieved() {
        shareOperation.reportDataRetrieved();
    }

    /// <summary>
    /// Use to simulate that an extended share operation has reached the status "submittedToBackgroundManager"
    /// </summary>
    function reportSubmittedBackgroundTask() {
        shareOperation.reportSubmittedBackgroundTask();
    }

    /// <summary>
    /// Submit for extended share operations. Can either report success or failure, and in case of success, can add a quicklink.
    /// This does NOT take care of all the prerequisites (such as calling reportExtendedShareStatus(started)) before submitting.
    /// </summary>
    function reportError() {
        var errorText = document.getElementById("extendedShareErrorMessage").value;
        shareOperation.reportError(errorText);
    }

    /// <summary>
    /// Call the reportCompleted API with the proper quicklink (if needed)
    /// </summary>
    function reportCompleted() {
        var addQuickLink = document.getElementById("addQuickLink").checked;
        if (addQuickLink) {
            var el;
            var quickLink = new QuickLink();

            var quickLinkId = document.getElementById("quickLinkId").value;
            if (!quickLinkId) {
                el = document.getElementById("quickLinkError");
                el.className = "unhidden";
                el.innerText = "Missing QuickLink ID";
                return;
            }
            quickLink.id = quickLinkId;

            var quickLinkTitle = document.getElementById("quickLinkTitle").value;
            if (!quickLinkTitle) {
                el = document.getElementById("quickLinkError");
                el.className = "unhidden";
                el.innerText = "Missing QuickLink title";
                return;
            }
            quickLink.title = quickLinkTitle;

            // For quicklinks, the supported FileTypes and DataFormats are set independently from the manifest
            quickLink.supportedFileTypes.replaceAll(["*"]);
            quickLink.supportedDataFormats.replaceAll([StandardDataFormats.text, StandardDataFormats.webLink, StandardDataFormats.applicationLink, StandardDataFormats.bitmap, StandardDataFormats.storageItems, StandardDataFormats.html, customFormatName]);

            // Prepare the icon for a QuickLink
            var iconUri = new Windows.Foundation.Uri(quickLinkIcon.src);
            Windows.Storage.StorageFile.getFileFromApplicationUriAsync(iconUri).done(function (iconFile) {
                quickLink.thumbnail = Windows.Storage.Streams.RandomAccessStreamReference.createFromFile(iconFile);
                shareOperation.reportCompleted(quickLink);
            }, function (e) {
                // Even if the QuickLink cannot be created it is important to call ReportCompleted. Otherwise, if this is a long-running share,
                // the app will stick around in the long-running share progress list.
                shareOperation.reportCompleted();
                throw e;
            });
        } else {
            shareOperation.reportCompleted();
        }
    }

    /// <summary>
    /// Calls the share operation's dismiss UI function.
    /// </summary>
    function dismissUI() {
        shareOperation.dismissUI();
    }

    /// <summary>
    /// Expand/collapse the Extended Sharing div
    /// </summary>
    function expandoClick() {
        if (extendedSharingCollapsed) {
            document.getElementById("extendedSharing").className = "unhidden";
            // Set expando glyph to up arrow
            document.getElementById("expandoGlyph").innerHTML = "&#57360;";
            extendedSharingCollapsed = false;
        } else {
            document.getElementById("extendedSharing").className = "hidden";
            // Set expando glyph to down arrow
            document.getElementById("expandoGlyph").innerHTML = "&#57361;";
            extendedSharingCollapsed = true;
        }
    }

    /// <summary>
    /// Expand/collapse the QuickLink fields
    /// </summary>
    function addQuickLinkChanged() {
        if (document.getElementById("addQuickLink").checked) {
            quickLinkFields.className = "unhidden";
        } else {
            quickLinkFields.className = "hidden";
            document.getElementById("quickLinkError").className = "hidden";
        }
    }

    // Initialize the activation handler
    WinJS.Application.addEventListener("activated", activatedHandler, false);
    WinJS.Application.addEventListener("shareready", shareReady, false);
    WinJS.Application.start();

    function initialize() {
        document.getElementById("addQuickLink").addEventListener("change", addQuickLinkChanged, false);
        document.getElementById("reportCompleted").addEventListener("click", reportCompleted, false);
        document.getElementById("dismissUI").addEventListener("click", dismissUI, false);
        document.getElementById("expandoClick").addEventListener("click", expandoClick, false);
        document.getElementById("reportStarted").addEventListener("click", reportStarted, false);
        document.getElementById("reportDataRetrieved").addEventListener("click", reportDataRetrieved, false);
        document.getElementById("reportSubmittedBackgroundTask").addEventListener("click", reportSubmittedBackgroundTask, false);
        document.getElementById("reportError").addEventListener("click", reportError, false);
    }

    document.addEventListener("DOMContentLoaded", initialize, false);
})();