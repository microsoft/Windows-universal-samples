//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Helper variables.
    var ui = WinJS.UI;
    var listViewIn;
    var semanticZoom;

    ui.Pages.define("/pages/subscriptions/subscriptions.html", {
        initializeLayout: function () {
            /// <summary>This function updates the ListView with new layouts.</summary>
            if (document.documentElement.offsetWidth <= 480) {
                semanticZoom.zoomedOut = false;
                semanticZoom.locked = true;
                listViewIn.itemDataSource = Render.groupedFeedItems.dataSource;
                listViewIn.groupDataSource = null;
                listViewIn.layout = new ui.ListLayout();
            } else {
                semanticZoom.locked = false;
                listViewIn.itemDataSource = Render.groupedFeedItems.dataSource;
                listViewIn.groupDataSource = Render.groupedFeedItems.groups.dataSource;
                listViewIn.layout = new ui.GridLayout({ groupHeaderPosition: "top" });
            }

            // Set the selection mode.
            listViewIn.selectionMode = ui.SelectionMode.multi;
            listViewIn.tapBehavior = ui.TapBehavior.toggleSelect;
            listViewIn.selection.set(Render.selectionIndices);
        },

        ready: function (element, options) {
            /// <summary>
            /// This function is called whenever a user navigates to this page. It populates the
            /// page elements with the app's data.
            /// </summary>
            /// <param name="element">
            /// The DOM element that contains all the content for the page.
            /// </param>
            /// <param name="options">
            /// An object that contains one or more property/value pairs to apply to the PageControl.
            /// </param>

            // Disable the app bar.
            var appbar = document.getElementById("appbar").winControl;
            appbar.disabled = true;

            // Force the semantic zoom layout.
            semanticZoom = element.querySelector("#feedsListViewArea").winControl;
            semanticZoom.forceLayout();

            // Setup the zoomed in list view.
            listViewIn = element.querySelector("#feedsListView-in").winControl;
            listViewIn.groupHeaderTemplate = element.querySelector("#categoryHeaderTemplate");
            listViewIn.itemTemplate = element.querySelector("#feedItemTemplate");
            this.initializeLayout();
            listViewIn.element.focus();

            // Handle selection events.
            listViewIn.addEventListener("selectionchanging", function (evt) {
                // Prevent the user from being able to select more subscriptions
                // than the subscription limit.
                if (evt.detail.newSelection.count() > ListViewLimits.subLimit) {
                    evt.detail.preventTapBehavior();
                }
            });
            listViewIn.addEventListener("selectionchanged", function () {
                var oldSel = Render.selectionIndices;
                var newSel = listViewIn.selection.getIndices();
                Render.selectionIndices = newSel;

                var feed;
                if (newSel.length > oldSel.length) {
                    // Get the newly added index.
                    var added = getNewSelectionIndices(oldSel, newSel);
                    var addedIndex = added[0];

                    // Subscribe to the selected feed.
                    feed = Render.getFeedObject(addedIndex);
                    Data.subscribe(feed);
                } else if (newSel.length < oldSel.length)  {
                    // Get the newly removed index.
                    var removed = getNewSelectionIndices(newSel, oldSel);
                    var removedIndex = removed[0];

                    // Unsubscribe from the selected feed.
                    feed = Render.getFeedObject(removedIndex);
                    Data.unsubscribe(feed);
                }
            });

            // Setup the zoomed out list view.
            var listViewOut = element.querySelector("#feedsListView-out").winControl;
            listViewOut.itemDataSource = Render.groupedFeedItems.groups.dataSource;
            listViewOut.itemTemplate = element.querySelector("#categoryItemTemplate");
            listViewOut.layout = new ui.GridLayout();
        },

        unload: function () {
            /// <summary>This function is called when the user navigates away from the page.</summary>

            // Re-enable the appbar.
            var appbar = document.getElementById("appbar").winControl;
            appbar.disabled = false;
        },

        updateLayout: function (element) {
            /// <summary>This function updates the page layout in response to window size changes.</summary>
            /// <param name="element">The DOM element that contains all the content for the page.</param>
            this.initializeLayout();
        }

        });

    function getNewSelectionIndices(oldSel, newSel) {
        /// <summary>Returns an array with integers in 'newSel' but not in 'oldSel'.</summary>
        /// <param name="a">Array 'oldSel'.</param>
        /// <param name="b">Array 'newSel'.</param>
        /// <returns>Array with integers in 'newSel' but not in 'oldSel'.</returns>
        return newSel.filter(function (obj) {
            return (oldSel.indexOf(obj) === -1);
        });
    }
})();