//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var System = Windows.System;
    var User = System.User;
    var KnownUserProperties = System.KnownUserProperties;

    var startButton;
    var stopButton;
    var userList;

    var nextUserNumber = 1;
    var userWatcher;

    var page = WinJS.UI.Pages.define("/html/scenario2-watchUsers.html", {
        ready: function (element, options) {
            startButton = document.getElementById("startButton");
            stopButton = document.getElementById("stopButton");
            userList = document.getElementById("userList");

            startButton.addEventListener("click", startWatching, false);
            stopButton.addEventListener("click", stopWatching, false);
        },
        unload: stopWatching
    });

    function startWatching() {
        WinJS.log && WinJS.log("", "sample", "status");

        nextUserNumber = 1;
        userList.innerHTML = ""; // remove all children

        userWatcher = User.createWatcher();
        userWatcher.addEventListener("added", onUserAdded);
        userWatcher.addEventListener("updated", onUserUpdated);
        userWatcher.addEventListener("removed", onUserRemoved);
        userWatcher.addEventListener("enumerationcompleted", onEnumerationCompleted);
        userWatcher.addEventListener("stopped", onWatcherStopped);
        userWatcher.start();
        startButton.disabled = true;
        stopButton.disabled = false;
    }

    function stopWatching() {
        if (userWatcher) {
            userWatcher.removeEventListener("added", onUserAdded);
            userWatcher.removeEventListener("updated", onUserUpdated);
            userWatcher.removeEventListener("removed", onUserRemoved);
            userWatcher.removeEventListener("enumerationCompleted", onEnumerationCompleted);
            userWatcher.removeEventListener("stopped", onWatcherStopped);
            userWatcher.stop();
            userWatcher = null;
            userList.innerHTML = ""; // remove all children
            startButton.disabled = false;
            stopButton.disabled = true;
        }
    }
    
    function findOptionByUserId(userId) {
       // make it safe to put inside "..."
        var sanitized = userId.replace("\\", "\\\\").replace("'", "\\'");
        return userList.querySelector("option[value='" + sanitized + "']");
    }

    function getDisplayNameOrGenericNameAsync(user) {
        return user.getPropertyAsync(KnownUserProperties.displayName).then(function (displayName) {
            // Choose a generic name if we do not have access to the actual name.
            if (displayName == "") {
                displayName = "User #" + nextUserNumber;
                ++nextUserNumber;
            }
            return displayName;
        });
    }

    function onUserAdded(e) {
        var user = e.user;

        // Create the user with "..." as the temporary display name.
        // Add it right away, because it might get removed while the
        // getDisplayNameOrGenericNameAsync is running.

        var newOption = document.createElement("option");
        newOption.text = "\u2026";
        newOption.value = user.nonRoamableId;
        newOption.selected = false;
        userList.add(newOption);

        // Uncomment this line, and the behavior in the TODO changes!
        // Instead of setting only the first character, it sets only the first word.
        //newOption.text = "Bogga";

        // Try to get the display name.
        getDisplayNameOrGenericNameAsync(user).then(function (displayName) {
            // TODO: Figure out why only the first character gets set.
            newOption.text = displayName;
        });
    }

    function onUserUpdated(e) {
        var user = e.user;
        
        // Look for the user in our collection and update the display name.
        var option = findOptionByUserId(user.nonRoamableId);

        // Try to update the display name.
        getDisplayNameOrGenericNameAsync(user).then(function (displayName) {
            // TODO: Figure out why only the first character gets set.
            option.text = displayName;
        });
    }

    function onUserRemoved(e) {
        var user = e.user;
        
        // Look for the user in our collection and remove it.
        var option = findOptionByUserId(user.nonRoamableId);

        if (option) {
            userList.remove(option);
        }
    }

    function onEnumerationCompleted()
    {
        WinJS.log && WinJS.log("Enumeration complete. Watching for changes...", "sample", "status");
    }

    function onWatcherStopped()
    {
        stopWatching();
    }

})();
