//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var PdfDocument = Windows.Data.Pdf.PdfDocument;
    var PdfPage = Windows.Data.Pdf.PdfPage;
    var PdfPageRenderOptions = Windows.Data.Pdf.PdfPageRenderOptions;

    var pdfDocument;
    var loadButton;
    var passwordBox;
    var renderingPanel;
    var pageNumberBox;
    var pageCountText;
    var output;
    var progressControl;

    // Use "| 0" to coerce to a 32-bit signed integer.
    var WrongPassword = 0x8007052b | 0; // HRESULT_FROM_WIN32(ERROR_WRONG_PASSWORD)
    var GenericFail = 0x80004005 | 0;   // E_FAIL

    var page = WinJS.UI.Pages.define("/html/scenario1-render.html", {
        ready: function (element, options) {
            loadButton = document.getElementById("loadButton");
            passwordBox = document.getElementById("passwordBox");
            renderingPanel = document.getElementById("renderingPanel");
            pageNumberBox = document.getElementById("pageNumberBox");
            pageCountText = document.getElementById("pageCountText");
            output = document.getElementById("output");
            progressControl = document.getElementById("progressControl");

            loadButton.addEventListener("click", loadPdfDocument);
            document.getElementById("viewPageButton").addEventListener("click", viewPage);
        }
    });

    function loadPdfDocument() {
        loadButton.disabled = true;
        pdfDocument = null;
        output.src = "";
        pageNumberBox.value = 1;
        renderingPanel.style.display = "none";
        WinJS.log && WinJS.log("", "sample", "status");

        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.append(".pdf");
        picker.pickSingleFileAsync().then(function (file) {
            if (file) {
                progressControl.style.display = "block";
                return PdfDocument.loadFromFileAsync(file, passwordBox.value);
            }
        }).then(function (doc) {
            pdfDocument = doc;
            if (pdfDocument) {
                renderingPanel.style.display = "block";
                if (pdfDocument.isPasswordProtected) {
                    WinJS.log && WinJS.log("Document is password protected.", "sample", "status");
                } else {
                    WinJS.log && WinJS.log("Document is not password protected.", "sample", "status");
                }
                pageCountText.innerText = pdfDocument.pageCount;
            }
        }, function (error) {
            switch (error.number) {
                case WrongPassword:
                    WinJS.log && WinJS.log("Document is password-protected and password is incorrect.", "sample", "error");
                    break;
                case GenericFail:
                    WinJS.log && WinJS.log("Document is not a valid PDF.", "sample", "error");
                    break;
                default:
                    // File I/O errors are reported as exceptions.
                    WinJS.log && WinJS.log(error.message, "sample", "error");
                    break;
            }
        }).done(function () {
            progressControl.style.display = "none";
            loadButton.disabled = false;
        });
    }

    function viewPage() {
        WinJS.log && WinJS.log("", "sample", "status");

        var pageNumber = parseInt(pageNumberBox.value, 10);
        if (isNaN(pageNumber) || (pageNumber < 1) || (pageNumber > pdfDocument.pageCount)) {
            WinJS.log && WinJS.log("Invalid page number.", "sample", "error");
            return;
        }

        output.src = "";
        progressControl.style.display = "block";

        // Convert from 1-based page number to 0-based page index.
        var pageIndex = pageNumber - 1;

        var page = pdfDocument.getPage(pageIndex);

        var stream = new Windows.Storage.Streams.InMemoryRandomAccessStream();

        var renderPromise;

        switch (options.selectedIndex) {
            // View actual size.
            case 0:
                renderPromise = page.renderToStreamAsync(stream);
                break;

            // View half size on beige background.
            case 1:
                var options1 = new PdfPageRenderOptions();
                options1.backgroundColor = Windows.UI.Colors.beige;
                options1.destinationHeight = page.size.height / 2;
                options1.destinationWidth = page.size.width / 2;
                renderPromise = page.renderToStreamAsync(stream, options1);
                break;

            // Crop to center.
            case 2:
                var options2 = new PdfPageRenderOptions();
                var rect = page.dimensions.trimBox;
                options2.sourceRect = { x: rect.x + rect.width / 4, y: rect.y + rect.height / 4, width: rect.width / 2, height: rect.height / 2 };
                renderPromise = page.renderToStreamAsync(stream, options2);
                break;
        }

        renderPromise.done(function () {
            // Put the stream in the image.
            var blob = MSApp.createBlobFromRandomAccessStream("image/png", stream);
            output.src = URL.createObjectURL(blob, { oneTimeOnly: true });

            progressControl.style.display = "none";
        });
    }
})();