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

    WinJS.Namespace.define("Application", {
        PageControlNavigator: WinJS.Class.define(
            // Define the constructor function for the PageControlNavigator.
            function PageControlNavigator(element, options) {
                /// <summary>
                /// Initialize the PageControlNavigator.
                /// </summary>
                /// <param name="element">Content host element.</param>
                /// <param name="options">Options containing the home page.</param>
                this._element = element || document.createElement("div");
                this._element.appendChild(this._createPageElement());

                this.home = options.home;

                this._eventHandlerRemover = [];

                var that = this;
                function addRemovableEventListener(e, eventName, handler, capture) {
                    e.addEventListener(eventName, handler, capture);
                    that._eventHandlerRemover.push(function () {
                        e.removeEventListener(eventName, handler);
                    });
                };

                addRemovableEventListener(nav, "navigating", this._navigating.bind(this), false);
                addRemovableEventListener(nav, "navigated", this._navigated.bind(this), false);

                Windows.UI.Core.SystemNavigationManager.getForCurrentView().addEventListener("backrequested", function () {
                    if (nav.canGoBack) {
                        nav.back();
                    }
                });

                window.onresize = this._resized.bind(this);

                // Event listener for resource context changes: If app resources get
                // localized or have variants created for different high-contrast modes
                // or different scales, then the contextChanged event will occur at
                // runtime if language or scale or high contrast is changed on the 
                // system. 
                addRemovableEventListener(WinJS.Resources, "contextchanged", this._contextChanged.bind(this), false);


                document.body.onkeyup = this._keyupHandler.bind(this);
                document.body.onkeypress = this._keypressHandler.bind(this);

                Application.navigator = this;
            }, {
                home: "",

                _element: null,
                _lastNavigationPromise: WinJS.Promise.as(),

                // This is the currently loaded Page object.
                pageControl: {
                    get: function () { return this.pageElement && this.pageElement.winControl; }
                },

                // This is the root element of the current page.
                pageElement: {
                    get: function () { return this._element.firstElementChild; }
                },

                dispose: function () {
                    /// <summary>
                    /// This function disposes the page navigator and its contents.
                    /// </summary>
                    if (this._disposed) {
                        return;
                    }

                    this._disposed = true;
                    WinJS.Utilities.disposeSubTree(this._element);
                    for (var i = 0; i < this._eventHandlerRemover.length; i++) {
                        this._eventHandlerRemover[i]();
                    }
                    this._eventHandlerRemover = null;
                },

                _contextChanged: function (e) {
                    /// <summary>
                    /// Responds to resource context changes and calls page-specific 
                    /// implementations of updateResources to reload app resources.
                    /// </summary>

                    // Could test e.detail.qualifier here to filter for changes on 
                    // specific resource qualifiers (e.g., scale, language, contrast).
                    // Will pass e to the page-specific implementations of 
                    // updateResources instead to allow finer control.
                    if (this.pageControl && this.pageControl.updateResources) {
                        this.pageControl.updateResources.call(this.pageControl, this.pageElement, e);
                    }
                },

                _createPageElement: function () {
                    /// <summary>
                    /// Creates a container for a new page to be loaded into.
                    /// </summary>
                    /// <returns>New div container.</returns>
                    var element = document.createElement("div");
                    element.setAttribute("dir", window.getComputedStyle(this._element, null).direction);
                    element.style.width = "100%";
                    element.style.height = "100%";
                    return element;
                },

                _getAnimationElements: function () {
                    /// <summary>
                    /// Retrieves a list of animation elements for the current page.
                    /// If the page does not define a list, animate the entire page.
                    /// </summary>
                    /// <returns>List of animation elements for the current page.</returns>
                    if (this.pageControl && this.pageControl.getAnimationElements) {
                        return this.pageControl.getAnimationElements();
                    }
                    return this.pageElement;
                },

                _navigated: function () {
                    /// <summary>
                    /// Animate to show the new page and update Back button.
                    /// </summary>
                    WinJS.UI.Animation.enterPage(this._getAnimationElements()).done();

                    Windows.UI.Core.SystemNavigationManager.getForCurrentView().appViewBackButtonVisibility =
                        nav.canGoBack ?
                        Windows.UI.Core.AppViewBackButtonVisibility.visible :
                        Windows.UI.Core.AppViewBackButtonVisibility.collapsed;
                },

                _navigating: function (args) {
                    /// <summary>
                    /// Responds to navigation by adding new pages to the DOM.
                    /// </summary>
                    /// <param name="args">Event args.</param>
                    var newElement = this._createPageElement();
                    var parentedComplete;
                    var parented = new WinJS.Promise(function (complete) { parentedComplete = complete; });

                    this._lastNavigationPromise.cancel();

                    this._lastNavigationPromise = WinJS.Promise.timeout().then(function () {
                        return WinJS.UI.Pages.render(args.detail.location, newElement, args.detail.state, parented);
                    }).then(function parentElement(control) {
                        var oldElement = this.pageElement;
                        if (oldElement.winControl && oldElement.winControl.unload) {
                            oldElement.winControl.unload();
                        }
                        WinJS.Utilities.disposeSubTree(this._element);
                        this._element.appendChild(newElement);
                        this._element.removeChild(oldElement);
                        oldElement.innerText = "";

                        parentedComplete();
                    }.bind(this));

                    args.detail.setPromise(this._lastNavigationPromise);
                },

                _keypressHandler: function (args) {
                    /// <summary>
                    /// This function responds to keypresses to only navigate when the backspace key is not used elsewhere.
                    /// </summary>
                    /// <param name="args">Event args.</param>
                    if (args.key === "Backspace") {
                        nav.back();
                    }
                },

                _keyupHandler: function (args) {
                    /// <summary>
                    /// This function responds to keyup to enable keyboard navigation.
                    /// </summary>
                    /// <param name="args">Event args.</param>
                    if ((args.key === "Left" && args.altKey) || (args.key === "BrowserBack")) {
                        nav.back();
                    } else if ((args.key === "Right" && args.altKey) || (args.key === "BrowserForward")) {
                        nav.forward();
                    }
                },

                _resized: function (args) {
                    /// <summary>
                    /// Responds to resize events and call the updateLayout function on the currently loaded page.
                    /// </summary>
                    /// <param name="args">Event args.</param>
                    if (this.pageControl && this.pageControl.updateLayout) {
                        this.pageControl.updateLayout.call(this.pageControl, this.pageElement);
                    }
                },
            }
        )
    });
})();
