//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    //  "use strict";
    var Globalization = Windows.Globalization;
    var Geolocation = Windows.Devices.Geolocation;
    var App = WinJS.Application;

    var formatterShortDateLongTime = new Globalization.DateTimeFormatting.DateTimeFormatter("{month.integer}/{day.integer}/{year.full} {hour.integer}:{minute.integer(2)}:{second.integer(2)}", ["en-US"], "US", Globalization.CalendarIdentifiers.gregorian, Globalization.ClockIdentifiers.twentyFourHour);
    var defaultDwellTimeSeconds = 10;
    var permissionsChecked = false;
    var registeredGeofenceData = new WinJS.Binding.List();

    // Value determined by how many max length event descriptors (91 chars) 
    // stored as a JSON string can fit in 8K (max allowed for local settings)
    var maxEventDescriptors = 42;
    var foregroundGeofenceEventCollectionKey = "ForegroundGeofenceEventCollection";
    var eventsData = new WinJS.Binding.List();

    var geofenceMonitor = Geolocation.Geofencing.GeofenceMonitor.current;
    var disposed;

    var page = WinJS.UI.Pages.define("/html/scenario4_ForegroundGeofence.html", {
        ready: function (element, options) {
            disposed = false;
            permissionsChecked = false;

            // Request location permission and register for events if allowed
            requestLocationAccess();

            // register for visibility change event to know when the app is minimized
            document.addEventListener("visibilitychange", onVisibilityChanged);

            // Display currently registered geofences in a ListView
            var registeredGeofenceListView = document.querySelector('#RegisteredGeofenceListView').winControl;
            registeredGeofenceListView.itemTemplate = RegisteredGeofencesItemTemplate;
            registeredGeofenceListView.itemDataSource = registeredGeofenceData.dataSource;
            registeredGeofenceListView.addEventListener("iteminvoked", function onRegisteredGeofenceListViewItemInvoked(eventInfo) {
                eventInfo.detail.itemPromise.done(function completed(item) {
                    if (!disposed) {
                        registeredGeofenceListView.selection.set([item.index]);

                        // update form values with the values from this geofence item
                        setInputValuesForGeofenceItem(item.data);
                    }
                });
            }, false);
            repopulateRegisteredGeofenceData();

            // Display geofence event data in a ListView
            var geofenceEventsListView = document.querySelector('#GeofenceEventsListView').winControl;
            geofenceEventsListView.itemTemplate = GeofenceEventsItemTemplate;
            geofenceEventsListView.itemDataSource = eventsData.dataSource;
            repopulateEventData();

            // Bind handlers to the input elements and buttons
            validateInputValues();
            var createGeofence = document.getElementById("createGeofenceButton");
            createGeofence.addEventListener("click", function addGeofenceItem() {

                // Generate a geofence using the input data
                var geofenceValues = getValidatedInputValuesForGeofence();
                var geofence = addGeofence(geofenceValues);

                if (geofence) {
                    // add geofence to listview
                    createGeofenceItemAndAddToListView(geofence);
                    WinJS.log && WinJS.log("", "sample", "status");
                }
            });

            var nameElement = document.getElementById("name");
            var charCount = document.getElementById("charCount");
            nameElement.addEventListener("input", function onNameChanged() {
                // get number of characters
                var value = nameElement.value;
                if (value && value.length > 0) {
                    var count = value.length;
                    charCount.innerText = count.toString() + " characters";
                } else {
                    charCount.innerText = "0 characters";
                }

                validateInputValues();
            });

            var latitude = document.getElementById("latitude");
            latitude.addEventListener("input", function onLatitudeChanged() {
                validateInputValues();
            });

            var longitude = document.getElementById("longitude");
            longitude.addEventListener("input", function onLongitudeChanged() {
                validateInputValues();
            });

            var radius = document.getElementById("radius");
            radius.addEventListener("input", function onRadiusChanged() {
                validateInputValues();
            });

            var setPositionToHereBtn = document.getElementById("setPositionToHereButton");
            setPositionToHereBtn.addEventListener("click", function setPositionToHere(eventObject) {
                function showWaitingUI(waiting) {
                    // set cursor to wait style
                    document.body.style.cursor = waiting ? 'wait' : "";
                    setPositionToHereBtn.style.cursor = waiting ? 'wait' : "";
                    latitude.style.cursor = waiting ? 'wait' : "";
                    longitude.style.cursor = waiting ? 'wait' : "";
                    radius.style.cursor = waiting ? 'wait' : "";

                    // disable the boxes that are about to be filled
                    setPositionToHereBtn.disabled = !!waiting;
                    latitude.disabled = !!waiting;
                    longitude.disabled = !!waiting;
                    radius.disabled = !!waiting;
                }

                showWaitingUI(true);

                getGeoposition().done(
                    function (pos) {

                        // Verify that the scenario has not been thrown away
                        if (!disposed) {
                            var coord = pos.coordinate;

                            // restore cursor and re-enable controls
                            showWaitingUI(false);

                            // Update the values in the page
                            latitude.value = coord.point.position.latitude;
                            longitude.value = coord.point.position.longitude;
                            validateInputValues();

                            // Clear status
                            WinJS.log && WinJS.log("", "sample", "status");
                        }
                    },
                    function (err) {
                        if (!disposed) {
                            showWaitingUI(false);
                            WinJS.log && WinJS.log(err.message, "sample", "error");
                        }
                    }
                );
            });

            var dwellTimeField = document.getElementById("dwellTime");
            dwellTimeField.addEventListener("focusout", function onDwellTimeChanged() {
                var validatedValue = parseTimeSpan(dwellTimeField, defaultDwellTimeSeconds);
                dwellTimeField.value = getDurationString(validatedValue);
            });

            var durationField = document.getElementById("duration");
            durationField.addEventListener("focusout", function onDurationChanged() {
                var validatedValue = parseTimeSpan(durationField, 0);
                durationField.value = getDurationString(validatedValue);
            });

            var setStartTimeToNow = document.getElementById("setStartTimeToNowButton");
            setStartTimeToNow.addEventListener("click", function onSetStartTimeToNow() {
                var startTimeField = document.getElementById("startTime");
                var startTime = new Date(); // with no params the startTime is Now
                startTimeField.value = formatterShortDateLongTime.format(startTime);
            });

            var removeGeofenceItem = document.getElementById("removeGeofenceItem");
            removeGeofenceItem.addEventListener("click", function removeSelectedGeofenceItems() {
                registeredGeofenceListView.selection.getItems().
                    done(function completed(result) {
                        for (var i = 0; i < result.length; i++) {

                            // get selected item and remove it
                            var geofenceItem = result[i].data;
                            removeGeofence(geofenceItem.geofence);

                            // Remove from ListView
                            var index = registeredGeofenceData.indexOf(geofenceItem);
                            registeredGeofenceData.splice(index, 1);
                        }
                    });
            });
        },
        dispose: function () {

            // unregister from geofence events
            geofenceMonitor.removeEventListener("geofencestatechanged", onGeofenceStateChanged);
            geofenceMonitor.removeEventListener("statuschanged", onGeofenceStatusChanged);
            document.removeEventListener("visibilitychange", onVisibilityChanged);

            disposed = true;

            // save off the contents of the event collection
            saveExistingEvents();
        }
    });

    function requestLocationAccess() {
        Windows.Devices.Geolocation.Geolocator.requestAccessAsync().done(
        function (accessStatus) {
            switch (accessStatus) {
                case Windows.Devices.Geolocation.GeolocationAccessStatus.allowed:

                    // register for geofence state change events
                    geofenceMonitor.addEventListener("geofencestatechanged", onGeofenceStateChanged);
                    geofenceMonitor.addEventListener("statuschanged", onGeofenceStatusChanged);
                    break;

                case Windows.Devices.Geolocation.GeolocationAccessStatus.denied:
                    WinJS.log && WinJS.log("Access to location is denied.", "sample", "error");
                    break;

                case Windows.Devices.Geolocation.GeolocationAccessStatus.unspecified:
                    WinJS.log && WinJS.log("Unspecified error!", "sample", "error");
                    break;
            }
        },
        function (err) {
            WinJS.log && WinJS.log(err, "sample", "error");
        });
    }

    function addGeofence(geofenceValues) {
        var geofence;
        if (!disposed) {

            try {
                var fenceKey = geofenceValues.fenceKey;
                var singleUse = geofenceValues.singleUse;
                var dwellTimeSpan = geofenceValues.dwellTimeSpan;
                var startDateTime = geofenceValues.startDateTime;
                var durationTimeSpan = geofenceValues.durationTimeSpan;

                // the geofence is a circular region
                var geocircle = new Windows.Devices.Geolocation.Geocircle(geofenceValues.position, geofenceValues.radius);

                // mask to listen for enter geofence, exit geofence and remove geofence events
                var mask = Windows.Devices.Geolocation.Geofencing.MonitoredGeofenceStates.entered
                    | Windows.Devices.Geolocation.Geofencing.MonitoredGeofenceStates.exited
                    | Windows.Devices.Geolocation.Geofencing.MonitoredGeofenceStates.removed;

                // Add the geofence to the GeofenceMonitor's collection of fences
                geofence = new Windows.Devices.Geolocation.Geofencing.Geofence(fenceKey, geocircle, mask, singleUse, dwellTimeSpan, startDateTime, durationTimeSpan);

                geofenceMonitor.geofences.push(geofence);
                return geofence;
            }
            catch (ex) {
                WinJS.log && WinJS.log(ex.message, "sample", "error");
                return null;
            }
        }
    }

    function removeGeofence(geofenceToRemoveArg) {

        // IVector<T>.IndexOf returns two values - a Boolean return value and an index.
        // Multiple return values for JS projections are modeled as an object.
        var item = geofenceMonitor.geofences.indexOf(geofenceToRemoveArg);
        if (item.returnValue) {
            geofenceMonitor.geofences.removeAt(item.index);
            WinJS.log && WinJS.log("", "sample", "status");
        } else {
            var msg = "Could not find Geofence " + geofenceToRemoveArg.id + " in the geofences collection";
            WinJS.log && WinJS.log(msg, "sample", "status");
        }
    }

    function onGeofenceStateChanged(args) {
        args.target.readReports().forEach(function processReport(report) {
            var state = report.newState;
            var geofence = report.geofence;
            var eventDescription = getTimeStampedMessage(geofence.id);

            if (state === Geolocation.Geofencing.GeofenceState.removed) {
                var reason = report.removalReason;

                if (reason === Geolocation.Geofencing.GeofenceRemovalReason.expired) {
                    eventDescription += " (Removed/Expired)";
                } else if (reason === Geolocation.Geofencing.GeofenceRemovalReason.used) {
                    eventDescription += " (Removed/Used)";
                }

                // remove the geofence from the client side geofences collection
                removeGeofence(geofence);

                // empty the registered geofence listbox and repopulate
                repopulateRegisteredGeofenceData();
            } else if (state === Geolocation.Geofencing.GeofenceState.entered) {

                // NOTE: You might want to write your app to take particular
                // action based on whether the app has internet connectivity.
                eventDescription += " (Entered)";
            } else if (state === Geolocation.Geofencing.GeofenceState.exited) {
                eventDescription += " (Exited)";
            } else {
                eventDescription += " (Unknown)";
            }

            addEventDescription(eventDescription);
        });
    }

    function onGeofenceStatusChanged(args) {
        var eventDescription = getTimeStampedMessage("Geofence Status");
        var geofenceStatus = args.target.status;

        if (Geolocation.Geofencing.GeofenceMonitorStatus.ready === geofenceStatus) {
            eventDescription += " (Ready)";
        } else if (Geolocation.Geofencing.GeofenceMonitorStatus.initializing === geofenceStatus) {
            eventDescription += " (Initializing)";
        } else if (Geolocation.Geofencing.GeofenceMonitorStatus.noData === geofenceStatus) {
            eventDescription += " (NoData)";
        } else if (Geolocation.Geofencing.GeofenceMonitorStatus.disabled === geofenceStatus) {
            eventDescription += " (Disabled)";
        } else if (Geolocation.Geofencing.GeofenceMonitorStatus.notInitialized === geofenceStatus) {
            eventDescription += " (NotInitialized)";
        } else if (Geolocation.Geofencing.GeofenceMonitorStatus.notAvailable === geofenceStatus) {
            eventDescription += " (NotAvailable)";
        } else {
            eventDescription += " (Unknown)";
        }

        addEventDescription(eventDescription);
    }

    function onVisibilityChanged(eventObject) {

        // NOTE: After the app is no longer visible on the screen and before the app is suspended
        // you might want your app to use toast notification for any geofence activity (not shown here).
        // By registering for VisibiltyChanged the app is notified when the app is no longer visible in the foreground.
        if (document.msVisibilityState === "visible") {

            // register for foreground events
            geofenceMonitor.addEventListener("geofencestatechanged", onGeofenceStateChanged);
            geofenceMonitor.addEventListener("statuschanged", onGeofenceStatusChanged);
        } else {

            // unregister foreground events (let background capture events)
            geofenceMonitor.removeEventListener("geofencestatechanged", onGeofenceStateChanged);
            geofenceMonitor.removeEventListener("statuschanged", onGeofenceStatusChanged);
        }
    }

    function getGeoposition() {
        var geolocator = new Windows.Devices.Geolocation.Geolocator();
        var promise = geolocator.getGeopositionAsync();
        promise.done(function (pos) {
            if (!disposed) {

                // clear status
                WinJS.log && WinJS.log("", "sample", "status");
            }
        }, function (err) {
            WinJS.log && WinJS.log(err.message, "sample", "error");
        }
        );
        return promise;
    }

    function getValidatedInputValuesForGeofence() {
        var nameElement = document.getElementById("name");
        var longitude = document.getElementById("longitude");
        var latitude = document.getElementById("latitude");
        var radius = document.getElementById("radius");
        var startTimeField = document.getElementById("startTime");
        var dwellTimeField = document.getElementById("dwellTime");
        var durationField = document.getElementById("duration");
        var geofenceSingleUse = document.getElementById("geofenceSingleUse");

        var inputValues = {
            position: {
                longitude: parseFloat(longitude.value),
                latitude: parseFloat(latitude.value),
                altitude: 0,
            },
            radius: parseFloat(radius.value),
            fenceKey: nameElement.value,
            singleUse: !!geofenceSingleUse.checked,
            dwellTimeSpan: parseTimeSpan(dwellTimeField, defaultDwellTimeSeconds),
            startDateTime: new Date(startTimeField.value || Date.now()),
            durationTimeSpan: parseTimeSpan(durationField, 0)
        };
        return inputValues;
    }

    function setInputValuesForGeofenceItem(item) {
        if (item) {
            var nameElement = document.getElementById("name");
            var latitude = document.getElementById("latitude");
            var longitude = document.getElementById("longitude");
            var radius = document.getElementById("radius");
            var startTimeField = document.getElementById("startTime");
            var dwellTimeField = document.getElementById("dwellTime");
            var durationField = document.getElementById("duration");
            var geofenceSingleUse = document.getElementById("geofenceSingleUse");
            nameElement.value = item.id;
            latitude.value = item.latitude;
            longitude.value = item.longitude;
            radius.value = item.radius;
            startTimeField.value = formatterShortDateLongTime.format(item.startTime);
            dwellTimeField.value = getDurationString(item.dwellTime);
            durationField.value = getDurationString(item.duration);
            geofenceSingleUse.checked = item.singleUse;

            // Update flags used to enable Create Geofence button
            validateInputValues();
        }
    }

    function repopulateRegisteredGeofenceData() {
        registeredGeofenceData.length = 0;
        geofenceMonitor.geofences.forEach(createGeofenceItemAndAddToListView);
    }

    function createGeofenceItemAndAddToListView(fence) {
        var geofenceItem = new GeofenceItem(fence);
        registeredGeofenceData.unshift(geofenceItem);
    }

    function getTimeStampedMessage(eventCalled) {
        return eventCalled + " " + (new Date().toLocaleTimeString());
    }

    function repopulateEventData() {
        var settings = Windows.Storage.ApplicationData.current.localSettings;
        if (settings.values.hasKey(foregroundGeofenceEventCollectionKey)) {
            try {
                var geofenceEvent = settings.values[foregroundGeofenceEventCollectionKey];
                var events = JSON.parse(geofenceEvent);

                // NOTE: the events are accessed in reverse order
                // because the events were added to JSON from
                // newer to older.  addEventDescription() adds
                // each new entry to the beginning of the collection.
                eventsData.length = 0;
                for (var pos = events.length - 1; pos >= 0; pos--) {
                    var element = events[pos].toString();
                    addEventDescription(element);
                }
            } finally {
                settings.values[foregroundGeofenceEventCollectionKey] = null;
            }
        }
    }

    function saveExistingEvents() {
        var settings = Windows.Storage.ApplicationData.current.localSettings;
        var stringifiedEventData;
        try {
            stringifiedEventData = JSON.stringify(eventsData.map(function (item) {
                return item;
            }));
        } finally {
            settings.values[foregroundGeofenceEventCollectionKey] = stringifiedEventData;
        }
    }

    function addEventDescription(eventDescription) {
        if (eventsData.length === maxEventDescriptors) {
            eventsData.pop();
        }
        eventsData.unshift(eventDescription);
    }

    // Checks the input values to verify that enough information
    // has been supplied to create a geofence
    function validateInputValues() {
        var nameElement = document.getElementById("name");
        var longitude = document.getElementById("longitude");
        var latitude = document.getElementById("latitude");
        var radius = document.getElementById("radius");

        // validate name
        var nameSet = (nameElement.value.length > 0);

        // validate GPS coordinates
        var latitudeSet = validateFloatInput(latitude);
        var longitudeSet = validateFloatInput(longitude);
        var radiusSet = validateFloatInput(radius);

        // Update Create Geofence button
        var isValid = nameSet && latitudeSet && longitudeSet && radiusSet;
        document.getElementById("createGeofenceButton").disabled = !isValid;

        return isValid;
    }

    // Utilities:
    var secondsPerMinute = 60;
    var secondsPerHour = 60 * secondsPerMinute;
    var secondsPerDay = 24 * secondsPerHour;
    var millisecondsPerSecond = 1000;
    var TimeSpanFields = Object.freeze({
        day: 0,
        hour: 1,
        minute: 2,
        second: 3
    });

    function getDurationString(duration) {
        function getTimeComponentAsString(timeComponent, appendLeadingZero) {
            if (timeComponent < 10 && appendLeadingZero) {
                return "0" + timeComponent;
            } else {
                return timeComponent;
            }
        }

        // note that double negation turns a float to an int
        var totalSeconds = ~~(duration / millisecondsPerSecond);
        var days = ~~(totalSeconds / secondsPerDay);
        totalSeconds -= days * secondsPerDay;
        var hours = ~~(totalSeconds / secondsPerHour);
        totalSeconds -= hours * secondsPerHour;
        var minutes = ~~(totalSeconds / secondsPerMinute);
        totalSeconds -= minutes * secondsPerMinute;
        var seconds = totalSeconds;

        var str = (days > 0) ? getTimeComponentAsString(days, false) + ":" : "";
        str += (hours > 0 || str.length > 0) ? getTimeComponentAsString(hours, (str.length > 0)) + ":" : "";
        str += (minutes > 0 || str.length > 0) ? getTimeComponentAsString(minutes, (str.length > 0)) + ":" : "";
        str += getTimeComponentAsString(seconds, (str.length > 0));

        return str;
    }

    function parseTimeSpan(field, defaultValue) {
        var timeSpanValue = 0;
        var parts = field.value.split(':');
        var numOfParts = parts.length;
        if (numOfParts <= 4) {
            for (var i = 0; i < numOfParts; i++) {
                var currentPart = parts[i];
                if (currentPart.length > 0) {
                    switch (4 - numOfParts + i) {
                        case TimeSpanFields.day:
                            timeSpanValue += parseInt(currentPart) * secondsPerDay;
                            break;
                        case TimeSpanFields.hour:
                            timeSpanValue += parseInt(currentPart) * secondsPerHour;
                            break;
                        case TimeSpanFields.minute:
                            timeSpanValue += parseInt(currentPart) * secondsPerMinute;
                            break;
                        case TimeSpanFields.second:
                            timeSpanValue += parseInt(currentPart);
                            break;
                    }
                }
            }
        }

        if (0 === timeSpanValue || isNaN(timeSpanValue)) {
            timeSpanValue = defaultValue;
        }

        timeSpanValue *= millisecondsPerSecond;

        return timeSpanValue;
    }

    function validateFloatInput(inputElement) {
        var stringValue = inputElement.value;
        var parsedValue = parseFloat(stringValue);
        if (!isNaN(parsedValue) && parsedValue.toString() !== stringValue) {
            inputElement.value = parsedValue.toString();
        }
        return !isNaN(parsedValue);
    }
})();