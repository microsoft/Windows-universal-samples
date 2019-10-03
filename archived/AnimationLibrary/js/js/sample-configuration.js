//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Animation library JS sample";

    var scenarios = [
        { url: "/html/enterPage.html", title: "Show page" },
        { url: "/html/transitionPages.html", title: "Transition between pages" },
        { url: "/html/enterContent.html", title: "Show content" },
        { url: "/html/transitionContents.html", title: "Transition between content" },
        { url: "/html/expandAndCollapse.html", title: "Expand and collapse" },
        { url: "/html/pointerFeedback.html", title: "Tap and click feedback" },
        { url: "/html/addAndDeleteFromList.html", title: "Add and remove from list" },
        { url: "/html/filterSearchList.html", title: "Filter search list" },
        { url: "/html/fadeInAndOut.html", title: "Fade in and out" },
        { url: "/html/crossfade.html", title: "Crossfade" },
        { url: "/html/reposition.html", title: "Reposition" },
        { url: "/html/dragAndDrop.html", title: "Drag and drop" },
        { url: "/html/dragBetween.html", title: "Drag between to reorder" },
        { url: "/html/showPopupUI.html", title: "Show pop-up UI" },
        { url: "/html/showEdgeUI.html", title: "Show edge UI" },
        { url: "/html/showPanel.html", title: "Show panel" },
        { url: "/html/swipeReveal.html", title: "Reveal ability to swipe" },
        { url: "/html/swipeSelection.html", title: "Swipe select and deselect" },
        { url: "/html/updateBadge.html", title: "Update a badge" },
        { url: "/html/updateTile.html", title: "Update a tile" },
        { url: "/html/customAnimation.html", title: "Run a custom animation" },
        { url: "/html/disableAnimations.html", title: "Disable animations" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
