//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/filterSearchList.html", {
        ready: function (element, options) {
            addItems = element.querySelector("#addItems");
            deleteItems = element.querySelector("#deleteItems");
            list = element.querySelector("#list");

            addItems.addEventListener("click", runAddToSearchListAnimation, false);
            deleteItems.addEventListener("click", runDeleteFromSearchListAnimation, false);
            deleteItems.style.display = "none";
            addItems.style.display = "block";
        }
    });

    var addItems;
    var deleteItems;
    var list;

    function runAddToSearchListAnimation() {
        // If there are less than 8 items in list, add another.
        if (list.childElementCount < 8) {
            var affectedItems = document.querySelectorAll(".listItem");

            // Create new item.
            var newItem = document.createElement("div");
            newItem.className = "listItem";
            newItem.style.background = randomColor();

            var addToSearchList;
            if (list.childElementCount > 0) {
                // Create addToSearchList animation with affected items (all items in list).
                addToSearchList = WinJS.UI.Animation.createAddToSearchListAnimation(newItem, affectedItems);
                // Insert new item in a random position in the list.
                list.insertBefore(newItem, list.childNodes[Math.floor(Math.random() * list.childElementCount)]);
            } else {
                // Create addToSearchList animation with no affected items (list is empty).
                addToSearchList = WinJS.UI.Animation.createAddToSearchListAnimation(newItem);
                list.appendChild(newItem);
            }

            // Execute animation.
            addToSearchList.execute();

            // Add another item.
            // The random delay represents the time it could take to get the next result to add in a search application.
            WinJS.Promise.timeout(Math.floor(Math.random() * 200)).done(runAddToSearchListAnimation);
        } else {
            deleteItems.style.display = "block";
            addItems.style.display = "none";
        }
    }

    function runDeleteFromSearchListAnimation() {
        var listItems = document.querySelectorAll(".listItem:not([deleting])");

        // If there are any children still visible, remove another child.
        if (listItems.length > 0) {
            // Choose a random item to delete.
            var deletedItem = listItems[Math.floor(Math.random() * listItems.length)];

            deletedItem.setAttribute("deleting", true);
            var affectedItems = document.querySelectorAll(".listItem:not([deleting])");

            // Create deleteFromSearchList.
            var deleteFromSearchList = WinJS.UI.Animation.createDeleteFromSearchListAnimation(deletedItem, affectedItems);

            // Take deletedItem out of the regular document layout flow so remaining list items will change position in response.
            deletedItem.style.position = "fixed";
            // Set it to its final visual state (hidden).
            deletedItem.style.opacity = "0";

            // Execute animation.
            var currentDeleteAnimation = deleteFromSearchList.execute();

            // Remove deleted item from the DOM after this animation is complete.
            // Removing an item from the DOM while an animation is executing on that item cuts the animation short.
            currentDeleteAnimation.done(
                // On animation completion, remove item.
                function () { list.removeChild(deletedItem); });

            // Delete another item from the list.
            // The random delay represents the processing time needed when searching.
            WinJS.Promise.timeout(Math.floor(Math.random() * 200)).done(runDeleteFromSearchListAnimation);
        } else {
            deleteItems.style.display = "none";
            addItems.style.display = "block";
        }
    }

    function randomColor() {
        // Return a random color in #rgb format
        return '#' + Math.floor((1 + Math.random()) * 4096).toString(16).substr(1);
    }
})();
