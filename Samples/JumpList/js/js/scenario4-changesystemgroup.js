//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ApplicationData = Windows.Storage.ApplicationData;
    var StorageApplicationPermissions = Windows.Storage.AccessCache.StorageApplicationPermissions;
    var RecentStorageItemVisibility = Windows.Storage.AccessCache.RecentStorageItemVisibility;
    var StartScreen = Windows.UI.StartScreen;
    var JumpList = StartScreen.JumpList;
    var JumpListSystemGroupKind = StartScreen.JumpListSystemGroupKind;

    var page = WinJS.UI.Pages.define("/html/scenario4-changesystemgroup.html", {
        ready: function (element, options) {
            SystemGroup_Recent.addEventListener("change", onSystemGroupRecentChange);
            SystemGroup_Frequent.addEventListener("change", onSystemGroupFrequentChange);
            SystemGroup_None.addEventListener("change", onSystemGroupNoneChange);
            PrepareSampleFiles.addEventListener("click", onPrepareSampleFilesClick);
            loadSavedSystemGroup();
        }
    });

    function onSystemGroupRecentChange() {
        if (SystemGroup_Recent.checked) {
            JumpList.loadCurrentAsync().done(function (jumpList) {
                jumpList.systemGroupKind = JumpListSystemGroupKind.recent;
                jumpList.saveAsync();
            });
        }
    }

    function onSystemGroupFrequentChange() {
        if (SystemGroup_Frequent.checked) {
            JumpList.loadCurrentAsync().done(function (jumpList) {
                jumpList.systemGroupKind = JumpListSystemGroupKind.frequent;
                jumpList.saveAsync();
            });
        }
    }

    function onSystemGroupNoneChange() {
        if (SystemGroup_None.checked) {
            JumpList.loadCurrentAsync().done(function (jumpList) {
                jumpList.systemGroupKind = JumpListSystemGroupKind.none;
                jumpList.saveAsync();
            });
        }
    }

    function addFile(fileName)
    {
        var localFolder = ApplicationData.current.localFolder;
        localFolder.tryGetItemAsync(fileName).done(function (file) {
            if (file == null) {
                localFolder.createFileAsync(fileName).done(function (newFile) {
                    StorageApplicationPermissions.mostRecentlyUsedList.add(newFile, "", RecentStorageItemVisibility.appAndSystem);
                });
            }
        });
    }

    function onPrepareSampleFilesClick() {
        // NOTE: This is for the sake of the sample only. Real apps should allow
        // the system list to be populated by user activity.

        for (var i = 0; i < 5; ++i) {
            addFile("Temp" + i + ".ms-jumplist-sample");
        }
    }

    function loadSavedSystemGroup() {
        JumpList.loadCurrentAsync().done(function (jumpList) {
            switch (jumpList.systemGroupKind)
            {
                case JumpListSystemGroupKind.recent:
                    SystemGroup_Recent.checked = true;
                    break;
                case JumpListSystemGroupKind.frequent:
                    SystemGroup_Frequent.checked = true;
                    break;
                case JumpListSystemGroupKind.none:
                    SystemGroup_None.checked = true;
                    break;
            }
        });
    }
})();
