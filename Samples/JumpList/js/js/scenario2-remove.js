//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var StartScreen = Windows.UI.StartScreen;
    var JumpList = StartScreen.JumpList;
    var JumpListSystemGroupKind = StartScreen.JumpListSystemGroupKind;

    var page = WinJS.UI.Pages.define("/html/scenario2-remove.html", {
        ready: function (element, options) {
            RemoveJumpList.addEventListener("click", onRemoveJumpListClick);
        }
    });

    function onRemoveJumpListClick() {
        JumpList.loadCurrentAsync().done(function (jumpList) {
            jumpList.systemGroupKind = JumpListSystemGroupKind.none;
            jumpList.items.clear();
            jumpList.saveAsync();
        });
    }
})();
