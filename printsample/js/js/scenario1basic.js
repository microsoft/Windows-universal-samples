//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1basic.html", {
        ready: function (element, options) {
            document.getElementById("printButton").addEventListener("click", printButtonHandler, false);
            
            // Register for Print Contract
            registerForPrintContract();
        }
    });   

    function registerForPrintContract() {
        var printManager = Windows.Graphics.Printing.PrintManager.getForCurrentView();
        printManager.onprinttaskrequested = onPrintTaskRequested;
        WinJS.log && WinJS.log("Print Contract registered. Use the Print button to print.", "sample", "status");
    }

    // Variable to hold the document source to print
    var gHtmlPrintDocumentSource = null;

    /// <summary>
    /// Print event handler for printing via the PrintManager API.
    /// </summary>
    /// <param name="printEvent" type="Windows.Graphics.Printing.PrintTaskRequest">
    /// The event containing the print task request object.
    /// </param>
    function onPrintTaskRequested(printEvent) {
        var printTask = printEvent.request.createPrintTask("Print Sample", function (args) {
            args.setSource(gHtmlPrintDocumentSource);
            
            // Register the handler for print task completion event
            printTask.oncompleted = onPrintTaskCompleted;
        });
    }

    /// <summary>
    /// Print Task event handler is invoked when the print job is completed.
    /// </summary>
    /// <param name="printTaskCompletionEvent" type="Windows.Graphics.Printing.PrintTaskCompleted">
    /// The event containing the print task completion object.
    /// </param>
    function onPrintTaskCompleted(printTaskCompletionEvent) {
        // Notify the user about the failure
        if (printTaskCompletionEvent.completion === Windows.Graphics.Printing.PrintTaskCompletion.failed) {
            WinJS.log && WinJS.log("Failed to print.", "sample", "error");
        }
    }

    /// <summary>
    /// Executed just before printing.
    /// </summary>
    var beforePrint = function () {
        // Replace with code to be executed just before printing the current document:
    };

    /// <summary>
    /// Executed immediately after printing.
    /// </summary>
    var afterPrint = function () {
        // Replace with code to be executed immediately after printing the current document:
    };

    function printButtonHandler() {
        // Optionally, functions to be executed immediately before and after printing can be configured as following:
        window.document.body.onbeforeprint = beforePrint;
        window.document.body.onafterprint = afterPrint;

        // Get document source to print
        MSApp.getHtmlPrintDocumentSourceAsync(document).then(function (htmlPrintDocumentSource) {
            gHtmlPrintDocumentSource = htmlPrintDocumentSource;

            // If the print contract is registered, the print experience is invoked.
            Windows.Graphics.Printing.PrintManager.showPrintUIAsync();
        });
    }
})();
