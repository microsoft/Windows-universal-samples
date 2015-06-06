//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/SetLockScreenImage.html", {
        ready: function (element, options) {
        document.getElementById("button1").addEventListener("click", OpenPickerAndSetLockScreenImage, false);
        }
    });

    function OpenPickerAndSetLockScreenImage() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.viewMode = Windows.Storage.Pickers.PickerViewMode.thumbnail;
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
        picker.fileTypeFilter.replaceAll([".jpg", ".jpeg", ".png", ".bmp"]);

        picker.pickSingleFileAsync().done(function (file) {
            if (file) {
                // Application now has read/write access to the picked file, setting image to lockscreen.
                Windows.System.UserProfile.LockScreen.setImageFileAsync(file).done(function (imageSet) {
                    WinJS.log && WinJS.log("File \"" + file.name + "\" set as lock screen image.", "sample", "status");
                    document.getElementById("newScreen").src = URL.createObjectURL(file, { oneTimeOnly: true });
                    document.getElementById("newScreen").style.visibility = "visible";
                },
                function (imageSet) {
                    // Set Image promise failed.  Display failure message.
                    WinJS.log && WinJS.log("Setting the lock screen image failed.  Make sure your copy of Windows is activated.", "sample", "error");
                });
            }
            else {
                WinJS.log && WinJS.log("No file was selected using the picker.", "sample", "error");
            }
        },
        function (file) {
            WinJS.log && WinJS.log("File was not returned", "sample", "error");
        });
    }
})();
