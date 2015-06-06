//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/pointerFeedback.html", {
        ready: function (element, options) {
            target1.addEventListener("pointerdown", onPointerDown, false);
            target1.addEventListener("pointerup", onPointerUp, false);
            target2.addEventListener("pointerdown", onPointerDown, false);
            target2.addEventListener("pointerup", onPointerUp, false);
            target3.addEventListener("pointerdown", onPointerDown, false);
            target3.addEventListener("pointerup", onPointerUp, false);
        }
    });

    function onPointerDown(evt) {
        WinJS.UI.Animation.pointerDown(evt.srcElement);
    }

    function onPointerUp(evt) {
        WinJS.UI.Animation.pointerUp(evt.srcElement);
    }
})();
