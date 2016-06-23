//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var printManager = Windows.Graphics.Printing.PrintManager.getForCurrentView();
    var printDate;

    var page = WinJS.UI.Pages.define("/html/scenario1basic.html", {
        ready: function (element, options) {
            printDate = document.getElementById("printDate");

            document.getElementById("printButton").addEventListener("click", printButtonHandler, false);
            
            // Register for Print Contract
            printManager.addEventListener("printtaskrequested", onPrintTaskRequested);

            // Optionally, functions to be executed immediately before and after printing can be configured as follows:
            window.addEventListener("beforeprint", beforePrint, false);
            window.addEventListener("afterprint", afterPrint, false);

        },
        unload: function () {
            printManager.removeEventListener("printtaskrequested", onPrintTaskRequested);
            window.removeEventListener("beforeprint", beforePrint);
            window.removeEventListener("afterprint", afterPrint);
        }
    });

    /// <summary>
    /// Print event handler for printing via the PrintManager API.
    /// </summary>
    /// <param name="printEvent" type="Windows.Graphics.Printing.PrintTaskRequest">
    /// The event containing the print task request object.
    /// </param>
    function onPrintTaskRequested(printEvent) {
        var printTask = printEvent.request.createPrintTask("Print Sample", function (args) {
            var deferral = args.getDeferral();

            // Register the handler for print task completion event
            printTask.addEventListener("completed", onPrintTaskCompleted);

            MSApp.getHtmlPrintDocumentSourceAsync(document).then(function (source) {
                args.setSource(source);
                deferral.complete();
            });
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
        printDate.innerText = "Printed on " + new Date().toLocaleDateString();
    };

    /// <summary>
    /// Executed immediately after printing.
    /// </summary>
    var afterPrint = function () {
        printDate.innerText = "";
    };

    function printButtonHandler() {
        // If the print contract is registered, the print experience is invoked.
        Windows.Graphics.Printing.PrintManager.showPrintUIAsync();
    }
})();
