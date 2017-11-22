//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

// Custom templating function
var VisitBackgroundEventsItemTemplate = WinJS.Utilities.markSupportedForProcessing(function VisitBackgroundEventsItemTemplate(itemPromise) {
    return itemPromise.then(function (currentItem) {

        // Build ListView Item Container div
        var result = document.createElement("div");
        result.className = "VisitBackgroundEventsListViewItemStyle";

        // Build content body
        var body = document.createElement("div");

        // Display title
        if (currentItem.data) {
            var title = document.createElement("h4");
            title.innerText = currentItem.data;
            body.appendChild(title);
        }

        // put the body into the ListView Item
        result.appendChild(body);

        return result;
    });
});

(function () {
    "use strict";
    var Background = Windows.ApplicationModel.Background;

    var sampleBackgroundTaskName = "SampleVisitBackgroundTask";
    var sampleBackgroundTaskEntryPoint = "js\\visitbackgroundtask.js";
    var visitTask;

    var visitBackgroundEvents;

    var registerBackgroundTaskButton;
    var unregisterBackgroundTaskButton;

    var page = WinJS.UI.Pages.define("/html/scenario8_VisitsBackgroundTask.html", {
        ready: function (element, options) {
            registerBackgroundTaskButton = document.getElementById("registerBackgroundTaskButton");
            unregisterBackgroundTaskButton = document.getElementById("unregisterBackgroundTaskButton");

            registerBackgroundTaskButton.addEventListener("click", registerBackgroundTask, false);
            unregisterBackgroundTaskButton.addEventListener("click", unregisterBackgroundTask, false);

            // Loop through all background tasks to see if our task is already registered
            var iter = Background.BackgroundTaskRegistration.allTasks.first();
            var hascur = iter.hasCurrent;
            while (hascur) {
                var cur = iter.current.value;
                if (cur.name === sampleBackgroundTaskName) {
                    visitTask = cur;
                    break;
                }
                hascur = iter.moveNext();
            }

            if (visitTask) {
                // Associate an event handler to the existing background task
                visitTask.addEventListener("completed", onCompleted);

                var backgroundAccessStatus = Background.BackgroundExecutionManager.getAccessStatus();
                switch (backgroundAccessStatus) {
                    case Background.BackgroundAccessStatus.alwaysAllowed:
                    case Background.BackgroundAccessStatus.allowedSubjectToSystemPolicy:
                        WinJS.log && WinJS.log("Background task is already registered. Waiting for next update...", "sample", "status");
                        break;

                    default:
                        WinJS.log && WinJS.log("Background tasks may be disabled for this app", "sample", "status");
                        break;
                }
                updateButtonStates(/*registered:*/ true);
            } else {
                updateButtonStates(/*registered:*/ false);
            }

            visitBackgroundEvents = new WinJS.Binding.List();
            var visitEventsListView = element.querySelector('#VisitBackgroundEventsListView').winControl;
            visitEventsListView.itemDataSource = visitBackgroundEvents.dataSource;
            visitEventsListView.itemTemplate = VisitBackgroundEventsItemTemplate;
            fillEventListBoxWithExistingEvents();
        }
    });

    // Handle background task completion
    function onCompleted() {
        try {
            // If the background task threw an exception, display the exception in
            // the error text box.
            visitTask.checkResult();

            // add visit event to listbox
            fillEventListBoxWithExistingEvents();
        } catch (ex) {
            // The background task had an error
            WinJS.log && WinJS.log(ex.toString(), "sample", "error");
        }
    }

    function registerBackgroundTask() {
        // Get permission for a background task from the user. If the user has already answered once,
        // this does nothing and the user must manually update their preference via PC Settings.
        Background.BackgroundExecutionManager.requestAccessAsync().done(function (backgroundAccessStatus) {
            var builder = new Windows.ApplicationModel.Background.BackgroundTaskBuilder();

            // Regardless of the answer, register the background task. If the user later adds this application
            // to the lock screen, the background task will be ready to run.
            // Create a new background task builder
            builder.name = sampleBackgroundTaskName;
            builder.taskEntryPoint = sampleBackgroundTaskEntryPoint;

            // Create a new visit trigger
            var trigger = new Windows.ApplicationModel.Background.GeovisitTrigger();

            // Set the desired monitoring scope.
            // For higher granularity such as venue/building level changes, choose venue.
            // For lower granularity more or less in the range of zipcode level changes, choose city. 
            // Choosing Venue here as an example.
            trigger.monitoringScope = Windows.Devices.Geolocation.VisitMonitoringScope.venue;

            // Associate the trigger with the background task builder
            builder.setTrigger(trigger);

            // If it is important that there is user presence and/or
            // internet connection when OnCompleted is called
            // the following could be called before calling Register()
            // var condition = new SystemCondition(SystemConditionType.userPresent | SystemConditionType.internetAvailable);
            // builder.addCondition(condition);

            // Register the background task
            visitTask = builder.register();

            // Associate an event handler with the new background task
            visitTask.addEventListener("completed", onCompleted);

            updateButtonStates(/*registered:*/ true);

            switch (backgroundAccessStatus) {
                case Background.BackgroundAccessStatus.alwaysAllowed:
                case Background.BackgroundAccessStatus.allowedSubjectToSystemPolicy:
                    WinJS.log && WinJS.log("Background task registered.", "sample", "status");

                    // Need to request access to location
                    // This must be done with the background task registration
                    // because the background task cannot display UI.
                    requestLocationAccess();
                    break;

                default:
                    WinJS.log && WinJS.log("Background tasks may be disabled for this app", "sample", "error");
                    break;
            }
        });
    }

    function unregisterBackgroundTask() {
        // Unregister the background task
        if (visitTask) {
            visitTask.unregister(true);
            visitTask = null;
        }

        WinJS.log && WinJS.log("Background task unregistered", "sample", "status");

        clearExistingEvents();

        updateButtonStates(/*registered:*/ false);
    }

    function requestLocationAccess() {
        Windows.Devices.Geolocation.Geolocator.requestAccessAsync().done(function (accessStatus) {
            switch (accessStatus) {
                case Windows.Devices.Geolocation.GeolocationAccessStatus.allowed:
                    break;

                case Windows.Devices.Geolocation.GeolocationAccessStatus.denied:
                    WinJS.log && WinJS.log("Access to location is denied.", "sample", "error");
                    break;

                default:
                case Windows.Devices.Geolocation.GeolocationAccessStatus.unspecified:
                    WinJS.log && WinJS.log("Unspecified error!", "sample", "error");
                    break;
            }
        });
    }

    // Update the enable state of the register/unregister buttons.
    function updateButtonStates(registered) {
        registerBackgroundTaskButton.disabled = registered;
        unregisterBackgroundTaskButton.disabled = !registered;
    }

    function fillEventListBoxWithExistingEvents() {
        var settings = Windows.Storage.ApplicationData.current.localSettings;
        if (settings.values.hasKey("BackgroundVisitEventCollection")) {
            var visitEvent = settings.values["BackgroundVisitEventCollection"].toString();

            if (0 !== visitEvent.length) {
                // remove all entries and repopulate
                visitBackgroundEvents.splice(0, visitBackgroundEvents.length);

                var events = JSON.parse(visitEvent);

                events.forEach(function (element) {
                    visitBackgroundEvents.push(element.toString());
                });
            }
        }
    }

    function clearExistingEvents() {
        var settings = Windows.Storage.ApplicationData.current.localSettings;
        settings.values["BackgroundVisitEventCollection"] = "";

        // remove all background events
        visitBackgroundEvents.splice(0, visitBackgroundEvents.length);
    }
})();
