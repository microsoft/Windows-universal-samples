//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/addAndDeleteFromList.html", {
        ready: function (element, options) {
            list = element.querySelector("#list");

            addItem.addEventListener("click", runAddToListAnimation, false);
            deleteItem.addEventListener("click", runDeleteFromListAnimation, false);
        }
    });

    var list;

    function runAddToListAnimation() {
        errorMessage.innerHTML = "";

        // Set affected items to all items that may move in response to the list addition.
        var affectedItems = document.querySelectorAll(".listItem");

        // Create new item.
        var newItem = document.createElement("div");
        newItem.className = "listItem";
        newItem.style.background = randomColor();

        // Create addToList animation.
        var addToList = WinJS.UI.Animation.createAddToListAnimation(newItem, affectedItems);

        // Insert new item into DOM tree.
        // This causes the affected items to change position.
        list.insertBefore(newItem, list.firstChild);

        // Execute the animation.
        addToList.execute();
    }

    function runDeleteFromListAnimation() {
        var listItems = document.querySelectorAll(".listItem:not([deleting])");

        if (listItems.length === 0) {
            errorMessage.innerHTML = "No items to delete. Please add an item.";
        } else {
            var deletedItem = listItems[0];
            deletedItem.setAttribute("deleting", true);
            var affectedItems = document.querySelectorAll(".listItem:not([deleting])");

            // Create deleteFromList animation.
            var deleteFromList = WinJS.UI.Animation.createDeleteFromListAnimation(deletedItem, affectedItems);

            // Take deletedItem out of the regular document layout flow so remaining list items will change position in response.
            deletedItem.style.position = "fixed";
            // Set deletedItem to its final visual state (hidden).
            deletedItem.style.opacity = "0";

            // Execute deleteFromList animation, then clean up.
            deleteFromList.execute().done(
                // After animation is complete, remove item from the DOM tree.
                function () { list.removeChild(deletedItem); });
        }
    }

    function randomColor() {
        // Return a random color in #rgb format
        return '#' + Math.floor((1 + Math.random()) * 4096).toString(16).substr(1);
    }
})();
