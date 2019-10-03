//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // All the items from the feed collection.  This is used by the subscriptions page to show all
    // the available feeds, categorized by the group they belong to.
    var groupedFeedItems = new WinJS.Binding.List().createGrouped(
        function groupKeySelector(item) { return item.group.key; },
        function groupDataSelector(item) { return item.group; }
    );

    function setupFeedCollectionListView() {
        /// <summary>
        /// Sets up the list view for the subscriptions page.
        /// </summary>
        Data.allFeeds.forEach(function (feed) {
            var feedItem = getFeedItem(feed);
            groupedFeedItems.push(feedItem);
        });
    }

    function getFeedItem(feed) {
        /// <summary>
        /// Given an instance of the Feed class, this returns a simplified object containing
        /// the properties that have to be rendered.
        /// </summary>
        /// <param name="feed">Feed to be retrieved.</param>
        /// <returns>Simplified object containing the properties used for rendering.</returns>
        var result = {
            group: { key: feed.category, title: feed.category },
            title: feed.title,
            description: feed.description,
            image: feed.image,
            uri: feed.uri
        };

        return result;
    }

    function getGroupedArticleItems(feeds, numOfItems) {
        /// <summary>
        /// Given a list of feeds and the number of articles to display per feed, this returns a populated grouped
        /// binding list that contains the articles of the feeds that the user has subscribed to.
        /// </summary>
        /// <param name="feeds">List of feeds.</param>
        /// <param name="numOfItems">Number of articles to display per feed.</param>
        /// <returns>
        /// Binding list that contains the articles of the feeds that the user has subscribed to.
        /// </returns>
        var groupedItems = new WinJS.Binding.List().createGrouped(
            function groupKeySelector(item) { return item.group.key; },
            function groupDataSelector(item) { return item.group; }
        );

        for (var i = 0, feedsLen = feeds.length; i < feedsLen; i++) {
            feeds[i].loadArticlesAsync().done(function (articles) {
                // If a feed contains no articles, don't display the feed.
                if (articles.length === 0) {
                    return;
                }

                var articleGroup = {
                    key: articles[0].feed.uri,
                    title: articles[0].feed.title,
                    largeTileTitle: articles[0].title
                };
                var articlesLen = (numOfItems && numOfItems < articles.length) ? numOfItems : articles.length;
                for (var j = 0; j < articlesLen; j++) {
                    var item = getArticleItem(articles[j], articleGroup);
                    groupedItems.push(item);
                }
            },
            function (feed) {
                // Error loading the articles for a feed.
                var articleGroup = { key: feed.uri, title: feed.title, largeTileTitle: null };
                
                groupedItems.push({
                    group: articleGroup,
                    error: "Unable to load this feed."
                });
            });
        }

        return groupedItems;
    }

    function getArticleItem(article, articleGroup) {
        /// <summary>
        /// Given an instance of the Article class and an article group object, this returns an article item
        /// that's meant for use by the list views.
        /// </summary>
        /// <param name="article">Article to be retrieved.</param>
        /// <param name="articleGroup">Object defining the group this article belongs to.</param>
        /// <returns>Returns an article object that's meant for use by the list views.</returns>
        var result = {
            group: articleGroup,
            title: article.title,
            body: article.body,
            imageSrc: article.imageSrc,
            imageBackground: null,
            tileType: "normal-text-tile"
        };

        return result;
    }

    function renderArticleTile(itemPromise) {
        /// <summary>
        /// Renders an item given an itemPromise.  This is meant for the article rendering pages: news.html and feed.html.
        /// </summary>
        /// <param name="itemPromise">An object that implements the IItemPromise interface.</param>
        /// <returns>Root element for the tile.</returns>
        return itemPromise.then(function (item) {
            var article = item.data;

            var div = document.createElement("div");

            // If there were any errors loading articles for a feed, display an error tile instead.
            if (article.error) {
                var errorTemplateEl = document.getElementById("errorTemplate");

                // The error template does not exist in the feed page.  In this case return an empty div.
                if (errorTemplateEl) {
                    article.tileType = "error-tile";
                    return errorTemplateEl.winControl.render(article).then(function (errorElement) {
                        div.className += "win-interactive";
                        div.appendChild(errorElement);

                        return div;
                    });
                }

                return div;
            }

            var textArticleTemplate = document.getElementById("textArticleTemplate").winControl;

            if (article.group.largeTileTitle === article.title) {
                article.tileType = "large-text-tile";
            }

            return textArticleTemplate.render(article).then(function (textArticleElement) {
                div.appendChild(textArticleElement);

                // Verify that the image URI is suffixed with the file extension and optionally a query string.
                var imageIsValid = (article.imageSrc) ?
                    article.imageSrc.search(/.png$|\?/i) !== -1 ||
                    article.imageSrc.search(/.gif$|\?/i) !== -1 ||
                    article.imageSrc.search(/.jpg$|\?/i) !== -1 ||
                    article.imageSrc.search(/.bmp$|\?/i) !== -1 : false;

                if (imageIsValid) {
                    getImageSizeAsync(article.imageSrc).then(function (size) {
                        if (size.height > 250 && size.width > 250) {
                            if (article.group.largeTileTitle === article.title) {
                                article.tileType = "large-image-tile";
                            } else {
                                article.tileType = "normal-image-tile";
                            }

                            article.imageBackground = "url('" + article.imageSrc + "') center center";

                            var imageArticleTemplateEl = document.getElementById("imageArticleTemplate");

                            // If imageArticleTemplateEl is empty, the user must be in another page by now.
                            if (!imageArticleTemplateEl) {
                                return;
                            }

                            var imageArticleTemplate = imageArticleTemplateEl.winControl;
                            imageArticleTemplate.render(article).then(function (imageArticleElement) {
                                WinJS.Utilities.empty(div);
                                div.appendChild(imageArticleElement);
                            });
                        }
                    }, function () {
                        // If the image fails to load, disregard the error and return the text tile.
                    });
                }

                return div;
            });
        });
    }

    function getImageSizeAsync(src) {
        /// <summary>
        /// Given the URI to an image, returns an object containing the width and height of the image.
        /// </summary>
        /// <param name="src">Image URI.</param>
        /// <returns>Object containing the width and height of the image.</returns>
        return new WinJS.Promise(function (complete, error) {
            var img = new Image();
            img.src = src;

            img.onload = function () {
                complete({
                    width: img.width,
                    height: img.height
                });
            };

            img.onerror = error;
        });
    }

    function getFeedObject(index) {
        /// <summary>
        /// Given a feed item index of the groupedFeedItems list, this returns the corresponding instance of the Feed class.
        /// </summary>
        /// <param name="index">Index of feed item in groupedFeedItems list.</param>
        /// <returns>Feed class corresponding to the index.</returns>
        var uri = Render.groupedFeedItems.getAt(index).uri;
        var feeds = Data.allFeeds;
        
        for (var i = 0, len = feeds.length; i < len; i++) {
            if (feeds[i].uri === uri) {
                return feeds[i];
            }
        }

        throw new RangeError("Feed did not exist at index " + index);
    }

    function getFeedIndex(title) {
        /// <summary>
        /// Given a feed item title, return the index of the feed item in the groupedFeedItems list.
        /// </summary>
        /// <param name="title">Title of feed item.</param>
        /// <returns>Index of feed item in the groupedFeedItem list.</returns>
        var feedItems = Render.groupedFeedItems;

        for (var i = 0, len = feedItems.length; i < len; i++) {
            if (title === feedItems.getAt(i).title) {
                return i;
            }
        }

        return -1;
    }

    // Public interface.
    WinJS.Namespace.define("Render", {
        groupedFeedItems: groupedFeedItems,
        setupFeedCollectionListView: setupFeedCollectionListView,

        getFeedObject: getFeedObject,
        getFeedIndex: getFeedIndex,
        getGroupedArticleItems: getGroupedArticleItems,

        renderArticleTile: renderArticleTile,
        selectionIndices: []
    });
})();
