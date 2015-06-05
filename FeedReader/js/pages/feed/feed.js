//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Helper variables.
    var nav = WinJS.Navigation;
    var ui = WinJS.UI;

    var groupedItems;
    var errorDiv;
    var listView;
    var listViewDiv;
    var feed;
    var page;
    var listViewState = {
        previousFeedTitle: "",
        previousItemIndex: 0
    };

    ui.Pages.define("/pages/feed/feed.html", {
        initializeLayout: function () {
            /// <summary>This function updates the ListView with new layouts.</summary>
            listView.itemDataSource = groupedItems.dataSource;

            if (document.documentElement.offsetWidth <= 480) {
                listView.itemTemplate = document.querySelector("#snappedArticleTemplate");
                listView.layout = new ui.ListLayout();
            } else {
                listView.itemTemplate = Render.renderArticleTile;
                listView.layout = new ui.CellSpanningLayout({
                    groupHeaderPosition: "top",
                    groupInfo: {
                        enableCellSpanning: true,
                        cellWidth: 250,
                        cellHeight: 250
                    },
                    itemInfo: page.getItemInfo
                });
            }

            page.verifyArticleLoad();
        },

        getItemInfo: function (index) {
            /// <summary>
            /// Given an index return the height and width of the tile corresponding to the item.
            /// </summary>
            /// <param name="index">Index of item in the ListView.</param>
            /// <returns>Height and width of the tile.</returns>
            var result;
            var item = groupedItems.getAt(index);

            // Set the dimensions of the title based on the type of tile (large, or normal).
            if (item.title === item.group.largeTileTitle) {
                result = {
                    height: 510,
                    width: 510,
                    newColumn: true
                };
            } else {
                result = {
                    height: 250,
                    width: 250,
                    newColumn: false
                };
            }

            return result;
        },

        groupHeaderInvoked: function (args) {
            WinJS.Navigation.navigate('/pages/feed/feed.html', { feedKey: event.srcElement.groupKey });
        },

        itemInvoked: function (args) {
            /// <summary>
            /// Navigate to the article corresponding to the item which was invoked.
            /// </summary>
            /// <param name="args">Event args.</param>
            var article = groupedItems.getAt(args.detail.itemIndex);
            listViewState.previousFeedTitle = feed.title;
            listViewState.previousItemIndex = args.detail.itemIndex;
            nav.navigate("/pages/article/article.html", { article: article });
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
            page = this;
            errorDiv = element.querySelector("#errorDiv");
            listViewDiv = element.querySelector("#articlesListView");
            listView = listViewDiv.winControl;

            // Get the current feed.
            if (options && options.feedKey) {
                var feeds = Data.subFeeds;
                for (var i = 0, len = feeds.length; i < len; i++) {
                    if (feeds[i].uri === options.feedKey) {
                        feed = feeds[i];
                        break;
                    }
                }
            }

            if (!feed) {
                feed = Data.subFeeds[0];
            }

            element.querySelector("header[role=banner] .pagetitle").innerText = feed.title;

            groupedItems = Render.getGroupedArticleItems([feed]);

            // Setup the list view.
            listView.ongroupheaderinvoked = this.groupHeaderInvoked.bind(this);
            listView.oniteminvoked = this.itemInvoked.bind(this);
            // If the user comes back to this page after navigating to an item, ensure
            // that the previous item is visible when the ListView is loaded.
            if (listViewState.previousFeedTitle === feed.title &&
                listViewState.previousItemIndex > 0) {
                setImmediate(function () {
                    listView.ensureVisible(listViewState.previousItemIndex);
                    listViewState.previousItemIndex = 0;
                });
            }
            this.initializeLayout();
            listView.element.focus();

            document.getElementById("refreshContentCmd").addEventListener("click", refreshFeed, false);
        },

        unload: function () {
            /// <summary>This function is called when the user navigates away from the page.</summary>
            document.getElementById("refreshContentCmd").removeEventListener("click", refreshFeed, false);
        },

        updateLayout: function (element) {
            /// <summary>This function updates the page layout in response to window size changes.</summary>
            /// <param name="element">The DOM element that contains all the content for the page.</param>
            this.initializeLayout();
        },

        verifyArticleLoad: function () {
            /// <summary>
            /// This function verifies that the articles for the feed were loaded successfully by
            /// examining the result of the last loadArticlesAsync promise.  If there was an error,
            /// an appropriate message is displayed to the user.
            /// </summary>
            feed.loadArticlesAsync().done(function () {
                // Successful load, display the listview containing the articles.
                listViewDiv.style.display = "";
                errorDiv.style.display = "none";
            }, function () {
                // Error loading articles, display the appropriate error message.
                listViewDiv.style.display = "none";
                errorDiv.style.display = "";
                errorDiv.innerText = Status.error ? Status.message :
                                                    "Unable to load this feed.";
            });
        }
    });

    function refreshFeed() {
        /// <summary>Refreshes the articles displayed on the page.</summary>
        feed.refreshArticlesAsync();
        groupedItems = Render.getGroupedArticleItems([feed], ListViewLimits.maxItems);
        page.initializeLayout();
    }
})();
