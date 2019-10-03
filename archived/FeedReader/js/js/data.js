//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Feed = WinJS.Class.define(
        function (feedData) {
            /// <summary>
            /// Initialize the feed object with the given feed data.
            /// </summary>
            /// <param name="feedData">The feed data.</param>
            this.category = feedData.Category;
            this.title = feedData.Name;
            this.description = feedData.Description;
            this.image = feedData.imgUri;
            this.uri = feedData.uri;
            this._currentLoadStatus = Feed.LoadStatus.none;
        }, {
            // Feed properties.
            category: null,
            title: null,
            description: null,
            image: null,
            uri: null,

            // Feed articles.
            articles: [],

            // Status.
            _currentLoadStatus: null,

            // Loading promise.
            _loadingPromise: null,

            _downloadArticlesAsync: function () {
                /// <summary>
                /// Downloads the articles of the feed.
                /// </summary>
                /// <returns>Returns a promise which downloads the articles for the feed.</returns>
                this._currentLoadStatus = Feed.LoadStatus.loading;

                var that = this;
                return new WinJS.Promise(function (complete, error) {
                    var feedUri = new Windows.Foundation.Uri(that.uri);

                    // Determine whether to bypass the cache based on the user's connection cost.
                    var syndicationClient = Windows.Web.Syndication.SyndicationClient();
                    syndicationClient.bypassCacheOnRetrieve = Status.bypassCache;

                    syndicationClient.retrieveFeedAsync(feedUri).done(function (syndicationFeed) {
                        that.articles = [];

                        // Create an article for each item in the feed.
                        syndicationFeed.items.forEach(function (syndicationItem) {
                            var article = new Article(syndicationItem, that);
                            that.articles.push(article);
                        });

                        that._currentLoadStatus = Feed.LoadStatus.loaded;

                        // Propagate success up with the current feed.
                        complete(that.articles);
                    }, function () {
                        that._currentLoadStatus = Feed.LoadStatus.failed;

                        // Propagate the feed retrieval error up.
                        error(that);
                    });
                });
            },

            refreshArticlesAsync: function () {
                /// <summary>
                /// Updates the articles of the feed.
                /// </summary>
                if (this._currentLoadStatus !== Feed.LoadStatus.loading) {
                    this._loadingPromise = this._downloadArticlesAsync();
                    // Load the articles for the feed and suppress any errors.  The error handling is done
                    // when we need to render the page containing the articles.
                    this._loadingPromise.then();
                }
            },

            loadArticlesAsync: function () {
                /// <summary>
                /// Loads the articles of the feed.
                /// </summary>
                /// <returns>
                /// Returns a new or existing promise which downloads the articles for a given feed.
                /// </returns>
                if (this._currentLoadStatus === Feed.LoadStatus.none) {
                    this._loadingPromise = this._downloadArticlesAsync();
                }

                return this._loadingPromise;
            }
        }, {
            // Load Status.
            LoadStatus: {
                none: "none",
                loading: "loading",
                loaded: "loaded",
                failed: "failed"
            },
        }
    );

    var Article = WinJS.Class.define(
        function (syndicationItem, feed) {
            /// <summary>
            /// Initialize the article with the syndication item and feed.
            /// </summary>
            /// <param name="syndicationItem">Represents an article in a feed.</param>
            /// <param name="feed">The feed associated with the article.</param>
            try {
                // Get and format the body div
                var bodyDiv = this._getFormattedBody(syndicationItem);

                // Get the feed.
                this.feed = feed;

                // Get the title.
                var titleText = syndicationItem.title ? syndicationItem.title.text : "";
                this.title = this._getSanitized(titleText);

                // Get the body.
                this.body = bodyDiv.innerHTML;

                // Get the image.
                var imageEl = bodyDiv.querySelector("img");
                this.imageSrc = (imageEl) ? imageEl.getAttribute("src") : null;
            } catch (e) {
                // Ignore malformed article.
            }
        }, {
            // Public members
            feed: null,
            title: null,
            body: null,
            imageSrc: null,
            imageBackground: null,

            _getFormattedBody: function (syndicationItem) {
                /// <summary>
                /// Gets the formatted body of the article.
                /// </summary>
                /// <param name="syndicationItem">Represents an article in a feed.</param>
                /// <returns>Returns a div containing formatted article body.</returns>
                var bodyContent = "";

                // Get the content from the Syndication Item
                if (syndicationItem.content) {
                    bodyContent = syndicationItem.content.text;
                } else {
                    bodyContent = syndicationItem.summary.text;
                }

                // Sanitize content.
                bodyContent = this._getSanitized(bodyContent);

                // Replace unnessecary linebreaks.
                bodyContent = bodyContent.replace(/(<br>)+/, "<br>");

                // Create the div.
                var body = document.createElement("div");
                body.innerHTML = bodyContent;

                WinJS.Utilities.query("[style]", body).setAttribute("style", "");

                WinJS.Utilities.query(".feedflare", body).removeClass("feedflare");

                // Add the "Read full article" link.
                syndicationItem.links.forEach(function (link) {
                    if (link.nodeName === "link") {
                        var fullArticleLink = document.createElement("a");
                        fullArticleLink.className = "full-article-link";
                        fullArticleLink.href = link.nodeValue;
                        fullArticleLink.innerHTML = "Read full article...";

                        body.appendChild(fullArticleLink);
                    }
                });

                return body;
            },

            _getSanitized: function (text) {
                /// <summary>
                /// Sanitizes the given text.
                /// </summary>
                /// <param name="text">Text to be sanitized.</param>
                /// <returns>The sanitized text.</returns>

                // Remove HTML tags and replace special characters (&lt; = <, &amp; = &, &quot; = ", etc.).
                var element = document.createElement("textarea");
                element.innerHTML = text;
                text = element.value;

                // Trim leading and trailing whitespace.
                text = text.replace(/^\s+/, "");
                text = text.replace(/\s+$/, "");

                return text;
            }
        }
    );

    // Handler object for the data collection.
    var Feeds = {
        // List of all available feeds listed in feedCollection.json.
        all: [],

        // List of the subscribed feeds.
        subscribed: [],

        subscribe: function (feed) {
            /// <summary>
            /// Subscribes to the given feed.
            /// </summary>
            /// <param name="feed">Feed to subscribe to.</param>
            Feeds.subscribed.push(feed);

            // Load the articles for the feed and suppress any errors.  The error handling is done
            // when we need to render the page containing the articles.
            feed.loadArticlesAsync().then();
        },

        unsubscribe: function (feed) {
            /// <summary>
            /// Unsubscribes from the given feed.
            /// </summary>
            /// <param name="feed">Feed to unsubscribe from.</param>
            var index = Feeds.subscribed.indexOf(feed);
            Feeds.subscribed.splice(index, 1);
        }
    };

    // Public interface.
    WinJS.Namespace.define("Data", {
        Feed: Feed,

        allFeeds: Feeds.all,
        subFeeds: Feeds.subscribed,

        subscribe: Feeds.subscribe,
        unsubscribe: Feeds.unsubscribe
    });
})();
