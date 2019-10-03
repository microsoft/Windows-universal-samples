//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/1_NavToUrl.html", {
        ready: function (element, options) {
            document.getElementById("goOrStopButton").addEventListener("click", goToUrl, false);
            document.getElementById("backButton").addEventListener("click", goBack, false);
            document.getElementById("forwardButton").addEventListener("click", goForward, false);

            // Register for navigation-related events in the WebView control.  These don't include
            // navigation events within sub-frames -- there are separate MSWebViewFrame* events for those.
            var webviewControl = document.getElementById("webview");
            webviewControl.addEventListener("MSWebViewNavigationStarting", navigationStarting);
            webviewControl.addEventListener("MSWebViewContentLoading", contentLoading);
            webviewControl.addEventListener("MSWebViewDOMContentLoaded", domContentLoaded);
            webviewControl.addEventListener("MSWebViewNavigationCompleted", navigationCompleted);
            webviewControl.addEventListener("MSWebViewUnviewableContentIdentified", unviewableContentIdentified);
            webviewControl.addEventListener("MSWebViewUnsupportedUriSchemeIdentified", unsupportedUriSchemeIdentified);

            webviewControl.navigate("http://go.microsoft.com/fwlink/?LinkId=294155");
        }
    });

    function goToUrl() {
        var destinationUrl = document.getElementById("urlField").value;
        try {
            document.getElementById("webview").navigate(destinationUrl);
        } catch (error) {
            WinJS.log && WinJS.log("\"" + destinationUrl + "\" is not a valid absolute URL.\n", "sdksample", "error");
            return;
        }
    }

    function stopNavigation() {
        document.getElementById("webview").stop();
        updateNavigatingState(false);
    }

    function goForward() {
        var webviewControl = document.getElementById("webview");
        if (webviewControl.canGoForward) {
            webviewControl.goForward();
        }
    }

    function goBack() {
        var webviewControl = document.getElementById("webview");
        if (webviewControl.canGoBack) {
            webviewControl.goBack();
        }
    }

    // This function is called when navigation either starts or stops in the WebView.
    // This updates the UI, shows/hides the spinning progress indicator and enables or
    // disables buttons as appropriate.
    function updateNavigatingState(isNavigating) {
        document.getElementById("progressRing").style.visibility = (isNavigating ? "visible" : "hidden");

        // The goOrStopButton can act as either a Go button or a Stop button depending on
        // whether the WebView is navigating or not.  Change its label and its event
        // listener according to the situation.
        var goOrStopButton = document.getElementById("goOrStopButton");
        goOrStopButton.innerText = (isNavigating ? "Stop" : "Go");
        goOrStopButton.removeEventListener("click", (isNavigating ? goToUrl : stopNavigation), false);
        goOrStopButton.addEventListener("click", (isNavigating ? stopNavigation : goToUrl), false);

        if (!isNavigating) {
            document.getElementById("backButton").disabled = !(document.getElementById("webview").canGoBack);
            document.getElementById("forwardButton").disabled = !(document.getElementById("webview").canGoForward);
        }
    }

    // NavigationStarting event is triggered when the WebView begins navigating to a new URL.
    function navigationStarting(e) {
        // clear any previous error messages that were displayed with WinJS.log
        WinJS.log && WinJS.log("", "sdksample", "status");

        appendLog && appendLog("Starting navigation to " + e.uri + ".\n");
        document.getElementById("urlField").value = e.uri;
        updateNavigatingState(true);
    }

    function contentLoading(e) {
        appendLog && appendLog("Loading content for " + e.uri + ".\n");
    }

    function domContentLoaded(e) {
        appendLog && appendLog("Content for " + e.uri + " has finished loading.\n");
    }

    // NavigationCompleted event is triggered either after all the DOM content has been loaded
    // successfully, or when loading failed.  The event arg for this is different from the other
    // navigation events, and includes a isSuccess field to indicate the status.
    function navigationCompleted(e) {
        updateNavigatingState(false);

        if (e.isSuccess) {
            appendLog && appendLog("Navigation completed successfully.\n");
        } else {
            WinJS.log && WinJS.log("Navigation failed with error code " + e.webErrorStatus, "sdksample", "error");
        }
    }

    // UnviewableContentIdentified event is triggered when the URL being navigated to is not
    // a type that can be displayed in WebView, for example an EXE file or a ZIP file.
    function unviewableContentIdentified(e) {
        updateNavigatingState(false);
        if (e.mediaType === undefined) {
            WinJS.log && WinJS.log(e.uri + " cannot be displayed in WebView", "sdksample", "error");
        } else {
            WinJS.log && WinJS.log(e.uri + " is of type " + e.mediaType + " which cannot be displayed in WebView", "sdksample", "error");
        }
    }

    // UnsupportedUriSchemeIdentified event is triggered when URL scheme being navigated to is
    // not supported by webview, for example file://, mailto://.
    // By default, if the event is not handled or cancelled, the default handler for that 
    // scheme is invoked.
    function unsupportedUriSchemeIdentified(e) {
        updateNavigatingState(false);

        // block all URIs from invoking other apps except the mailto: protocol
        if (e.uri.search("mailto:") < 0) {
            WinJS.log && WinJS.log(e.uri + " scheme is not supported by WebView", "sdksample", "error");
            e.preventDefault();
        }
    }

    // Appends a line of text to logArea
    function appendLog(message) {
        var logArea = document.getElementById("logArea");
        logArea.value += message;
        logArea.scrollTop = logArea.scrollHeight;
    }

})();
