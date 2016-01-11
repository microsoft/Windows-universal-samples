//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

var wap = Windows.ApplicationModel.Package;
var voiceCommandManager = Windows.ApplicationModel.VoiceCommands.VoiceCommandDefinitionManager;

(function () {
    "use strict";

    var app = WinJS.Application;
    var nav = WinJS.Navigation;
    var activationKinds = Windows.ApplicationModel.Activation.ActivationKind;
    var AppViewBackButtonVisibility = Windows.UI.Core.AppViewBackButtonVisibility;
    var systemNavigationManager = Windows.UI.Core.SystemNavigationManager.getForCurrentView();
    var splitView;

    WinJS.Namespace.define("SdkSample", {
    });

    // provide a global data storage namespace.
    var store = new Store();
    WinJS.Namespace.define("SdkSample.DataStore", {
        TripStore: store
    });

    function activated(eventObject) {
        /// <summary> Main entrypoint. When an app is invoked by Cortana when pressing a button in 
        /// Cortana's UI, Protocol Activation will occur. However, when activated directly by Cortana
        /// A VoiceCommand action will occur instead. 
        /// In this case, the 'show trip to {destination}' command leads to the VoiceCommand activation,
        /// and buttons displayed by the background task lead to protocol activation. </summary>

        var activationKind = eventObject.detail.kind;
        var activatedEventArgs = eventObject.detail.detail;

        WinJS.Utilities.startLog();

        var p = WinJS.UI.processAll().
            then(function () {

                SdkSample.DataStore.TripStore.loadTrips().then(function () {
                    return wap.current.installedLocation.getFileAsync("AdventureworksCommands.xml");
                }).then(function (file) {
                    return voiceCommandManager.installCommandDefinitionsFromStorageFileAsync(file);
                }).then(function () {
                    var language = window.navigator.userLanguage || window.navigator.language;

                    var commandSetName = "AdventureWorksCommandSet_" + language.toLowerCase();
                    if (voiceCommandManager.installedCommandDefinitions.hasKey(commandSetName)) {
                        var vcd = voiceCommandManager.installedCommandDefinitions.lookup(commandSetName);
                        var phraseList = [];
                        SdkSample.DataStore.TripStore.Trips.forEach(function (trip) {
                            phraseList.push(trip.destination);
                        });
                        vcd.setPhraseListAsync("destination", phraseList).done();
                    } else {
                        WinJS.log && WinJS.log("VCD not installed yet?", "", "warning");
                    }
                }).done(function () {

                    var url = "/html/tripListView.html";
                    var initialState = {};

                    if (activationKind == Windows.ApplicationModel.Activation.ActivationKind.protocol) {
                        // Cortana protocol activates apps when the background task provides a link into the app.
                        var uri = eventObject.detail.uri;
                        var decoder = new Windows.Foundation.WwwFormUrlDecoder(uri.query);
                        var destination = decoder.getFirstValueByName("LaunchContext");

                        var destinationTrip = null;
                        for (var i = 0; i < SdkSample.DataStore.TripStore.Trips.length; i++) {
                            var trip = SdkSample.DataStore.TripStore.Trips.getAt(i)
                            if (trip.destination == destination) {
                                destinationTrip = trip;
                                break;
                            }
                        }

                        if (destinationTrip != null) {
                            initialState.trip = destinationTrip;
                            url = "/html/tripDetails.html";
                            nav.history.backStack.push({ location: "/html/tripListView.html" })
                        }

                    } else if (activationKind == Windows.ApplicationModel.Activation.ActivationKind.voiceCommand) {
                        // When directly launched via VCD, activation is via the VoiceCommand ActivationKind.
                        // Using the "
                        var speechRecognitionResult = activatedEventArgs[0].result;
                        var voiceCommandName = speechRecognitionResult.rulePath[0];
                        var destination = "";
                        switch (voiceCommandName) {
                            case "showTripToDestination":
                                var destination = speechRecognitionResult.semanticInterpretation.properties["destination"][0];
                                var destinationTrip = null;
                                for (var i = 0; i < SdkSample.DataStore.TripStore.Trips.length; i++) {
                                    var trip = SdkSample.DataStore.TripStore.Trips.getAt(i)
                                    if (trip.destination == destination) {
                                        destinationTrip = trip;
                                        break;
                                    }
                                }

                                if (destinationTrip != null) {
                                    initialState.trip = destinationTrip;
                                    url = "/html/tripDetails.html";
                                    nav.history.backStack.push({ location: "/html/tripListView.html" })
                                }
                                break;
                            default:
                                break;
                        }
                    } else {
                        var navHistory = app.sessionState.navigationHistory;
                        if (navHistory) {
                            nav.history = navHistory;
                            url = navHistory.current.location;
                            initialState = navHistory.current.state || initialState;
                        }
                    }

                    initialState.activationKind = activationKind;
                    initialState.activatedEventArgs = activatedEventArgs;
                    nav.history.current.initialPlaceholder = true;
                    return nav.navigate(url, initialState);
                }, function (e) {
                    WinJS.log && WinJS.log("Failed to load VCD", e.message, "error");
                });

            });

        // Calling done on a promise chain allows unhandled exceptions to propagate.
        p.done();

        // Use setPromise to indicate to the system that the splash screen must not be torn down
        // until after processAll and navigate complete asynchronously.
        eventObject.setPromise(p);
    }

    function navigating(eventObject) {
        /// <summary> Handle swapping out the content block for the new page, and setting up
        /// the backstack. </summary>

        var url = eventObject.detail.location;
        var host = document.getElementById("contentHost");
        // Call unload and dispose methods on current page, if any exist
        if (host.winControl) {
            host.winControl.unload && host.winControl.unload();
            host.winControl.dispose && host.winControl.dispose();
        }
        WinJS.Utilities.disposeSubTree(host);
        WinJS.Utilities.empty(host);
        WinJS.log && WinJS.log("", "", "status");

        var p = WinJS.UI.Pages.render(url, host, eventObject.detail.state).
            then(function () {
                var navHistory = nav.history;
                app.sessionState.navigationHistory = {
                    backStack: navHistory.backStack.slice(0),
                    forwardStack: navHistory.forwardStack.slice(0),
                    current: navHistory.current
                };
                app.sessionState.lastUrl = url;
            });
        p.done();
        eventObject.detail.setPromise(p);
    }

    function navigated(eventObject) {
        // If we returned to the root page, then empty the backstack.
        if (nav.history.backStack.length > 0 && eventObject.detail.location == nav.history.backStack[0].location) {
            nav.history.backStack.length = 0;
        }

        // Set the Back button state appropriately.
        systemNavigationManager.appViewBackButtonVisibility = nav.canGoBack ? 
            AppViewBackButtonVisibility.visible : AppViewBackButtonVisibility.collapsed;
    }

    function backRequested(eventObject) {
        if (!eventObject.handled && nav.canGoBack) {
            eventObject.handled = true;
            nav.back();
        }
    }

    // Register for Back button events.
    systemNavigationManager.addEventListener("backrequested", backRequested);

    // Register for the navigated event so we can update the Back button.
    nav.addEventListener("navigated", navigated);

    nav.addEventListener("navigating", navigating);
    app.addEventListener("activated", activated, false);
    app.start();
})();

window.onerror = function (E) {
    "use strict";
    debugger;
}