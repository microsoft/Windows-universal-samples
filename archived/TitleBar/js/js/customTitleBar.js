//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ViewManagement = Windows.UI.ViewManagement;
    var ApplicationView = ViewManagement.ApplicationView;

    // CoreTitleBarHelper is a helper class provided in the CoreViewHelpers project.
    // It allows JavaScript to access properties and events of the
    // CoreApplicationViewTitleBar object.
    var titleBarHelper = CoreViewHelpers.CoreTitleBarHelper.getForCurrentView();
    var titleBarContent;

    var page = WinJS.UI.Pages.define("/html/customTitleBar.html", {
        ready: function ready(element, options) {
            titleBarContent = element.querySelector("#titleBarContent");
            titleBarHelper.addEventListener("layoutmetricschanged", onLayoutMetricsChanged);
            titleBarHelper.addEventListener("isvisiblechanged", updatePositionAndVisibility);
            // The resize event is raised when the view enters or exits full screen mode.
            window.addEventListener("resize", updatePositionAndVisibility);
            onLayoutMetricsChanged();
            updatePositionAndVisibility();
        },
        unload: function unload() {
            titleBarHelper.removeEventListener("layoutmetricschanged", onLayoutMetricsChanged);
            titleBarHelper.removeEventListener("isvisiblechanged", updatePositionAndVisibility);
            window.removeEventListener("resize", updatePositionAndVisibility);
        }
    });

    function onLayoutMetricsChanged() {
        titleBarContent.style.height = titleBarHelper.height + "px";
        titleBarContent.style.paddingLeft = titleBarHelper.systemOverlayLeftInset + "px";
        titleBarContent.style.paddingRight = titleBarHelper.systemOverlayRightInset + "px";
    }

    // The title bar is the first child of the body.
    //
    // When not in full screen mode, the DOM looks like this:
    //
    //      <body>
    //          <div style="display: block; position: relative">Custom-rendered title bar</div>
    //          Rest of content
    //      </body>
    //
    // Relative position causes the title bar to push the rest of the content down.
    //
    // In full screen mode, the the DOM looks like this:
    //
    //      <body>
    //          <div style="display: block -or- none; position: absolute">Custom-rendered title bar</div>
    //          Rest of content
    //      </body>
    //
    // Absolute position causes the title bar to overlay the rest of the content.
    // The title bar is either display:block or display:none, depending on the value of the isVisible property.

    function updatePositionAndVisibility() {
        if (ApplicationView.getForCurrentView().isFullScreenMode) {
            // In full screen mode, the title bar overlays the content.
            // and might or might not be visible.
            titleBarContent.style.display = titleBarHelper.isVisible ? "block" : "none";
            titleBarContent.style.position = "absolute";
        } else {
            // When not in full screen mode, the title bar is visible and does not overlay content.
            titleBarContent.style.display = "block";
            titleBarContent.style.position = "relative";
        }
    }
})();
