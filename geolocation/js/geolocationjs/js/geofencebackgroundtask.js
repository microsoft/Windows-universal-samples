//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

// A JavaScript background task runs a specified JavaScript file.
(function () {
    "use strict";
    var cancel = false;

    // Get the background task instance's activation parameters
    var backgroundTaskInstance = Windows.UI.WebUI.WebUIBackgroundTaskInstance.current;
    var promise;

    // Associate a cancellation handler with the background task.
    function onCanceled(cancelSender, cancelReason) {
        if (promise) {
            promise.operation.cancel();
        }
    }
    backgroundTaskInstance.addEventListener("canceled", onCanceled);

    var geolocator = new Windows.Devices.Geolocation.Geolocator();
    var geofenceBackgroundEvents = new Array();
    var maxEventDescriptors = 42;   // Value determined by how many max length event descriptors (91 chars) 
                                    // stored as a JSON string can fit in 8K (max allowed for local settings)

    backgroundTaskInstance.succeeded = false;

    getGeofenceStateChangedReports(Windows.Devices.Geolocation.Geofencing.GeofenceMonitor.current.lastKnownGeoposition);

    backgroundTaskInstance.succeeded = true;

    // A JavaScript background task must call close when it is done
    close();

    function getGeofenceStateChangedReports(pos) {
        var monitor = Windows.Devices.Geolocation.Geofencing.GeofenceMonitor.current;
        var posLastKnown = monitor.lastKnownGeoposition;
        var coordLastKnown = posLastKnown.coordinate;
        var eventDate = coordLastKnown.timestamp;
        var calendarBackground = new Windows.Globalization.Calendar();
        var formatterLongTimeBackground = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("{hour.integer}:{minute.integer(2)}:{second.integer(2)}", ["en-US"], "US", Windows.Globalization.CalendarIdentifiers.gregorian, Windows.Globalization.ClockIdentifiers.twentyFourHour);
        var eventArray = new Array();
        var loop = 0;
        var eventOfInterest = true;
        var dateResolutionToSeconds = 1000;  // conversion from 1 milli-second resolution to seconds

        fillEventCollectionWithExistingEvents();

        // NOTE TO DEVELOPER:
        // These events can be filtered out if the
        // geofence event time is stale.
        calendarBackground.setToNow();
        var nowDate = calendarBackground.getDateTime();
        var diffTimeSpan = nowDate.getTime() - eventDate.getTime();
        var deltaInSeconds = diffTimeSpan / dateResolutionToSeconds;

        // NOTE TO DEVELOPER:
        // If the time difference between the geofence event and now is too large
        // the eventOfInterest should be set to false.

        if (eventOfInterest) {
            // NOTE TO DEVELOPER:
            // This event can be filtered out if the
            // geofence event location is too far away.
            if ((posLastKnown.coordinate.point.position.latitude !== pos.coordinate.point.position.latitude) ||
                (posLastKnown.coordinate.point.position.longitude !== pos.coordinate.point.position.longitude)) {
                // NOTE TO DEVELOPER:
                // Use an algorithm like the Haversine formula or Vincenty's formulae to determine
                // the distance between the current location (pos.coordinate)
                // and the location of the geofence event (latitudeEvent/longitudeEvent).
                // If too far apart set eventOfInterest to false to
                // filter the event out.
            }

            if (eventOfInterest) {
                var eventItem = null;
                var numEventsOfInterest = 0;

                Windows.Devices.Geolocation.Geofencing.GeofenceMonitor.current.readReports().forEach(function (report) {
                    var state = report.newState;
                    var geofence = report.geofence;
                    var id = geofence.id;

                    eventItem = new String(report.geofence.id);
                    eventItem += " " + formatterLongTimeBackground.format(eventDate);

                    if (state === Windows.Devices.Geolocation.Geofencing.GeofenceState.removed) {
                        var reason = report.removalReason;

                        if (reason === Windows.Devices.Geolocation.Geofencing.GeofenceRemovalReason.expired) {
                            eventItem += " (Removed/Expired)";
                        } else if (reason === Windows.Devices.Geolocation.Geofencing.GeofenceRemovalReason.used) {
                            eventItem += " (Removed/Used)";
                        }
                    } else if (state === Windows.Devices.Geolocation.Geofencing.GeofenceState.entered) {
                        eventItem += " (Entered)";
                    } else if (state === Windows.Devices.Geolocation.Geofencing.GeofenceState.exited) {
                        eventItem += " (Exited)";
                    }

                    addEventDescription(eventItem);

                    numEventsOfInterest++;
                });

                if (eventOfInterest && 0 !== numEventsOfInterest) {
                    saveExistingEvents();

                    // NOTE: Other notification mechanisms can be used here, such as Badge and/or Tile updates.
                    doToast(numEventsOfInterest, eventItem);
                }
            }
        }
    }

    function doToast(numEventsOfInterestArg, eventName) {
        // pop a toast for each geofence event
        var toastNotifier = Windows.UI.Notifications.ToastNotificationManager.createToastNotifier();

        // Create a two line toast and add audio reminder

        // Here the xml that will be passed to the 
        // ToastNotification for the toast is retrieved
        // toastXml is an XmlDocument object
        var toastXml = Windows.UI.Notifications.ToastNotificationManager.getTemplateContent(Windows.UI.Notifications.ToastTemplateType.toastText02);

        // Set both lines of text
        // nodeList is an XmlNodeList object
        var nodeList = toastXml.getElementsByTagName("text");
        nodeList.item(0).appendChild(toastXml.createTextNode("Geolocation Sample"));

        if (1 === numEventsOfInterestArg) {
            nodeList.item(1).appendChild(toastXml.createTextNode(eventName));
        } else {
            var secondLine = "There are " + numEventsOfInterestArg + " new geofence events";
            nodeList.item(1).appendChild(toastXml.createTextNode(secondLine));
        }

        // now create a xml node for the audio source
        // toastNode is an IXmlNode object
        var toastNode = toastXml.selectSingleNode("/toast");
        // audio is an XmlElement
        var audio = toastXml.createElement("audio");
        audio.setAttribute("src", "ms-winsoundevent:Notification.SMS");

        // toast is a ToastNotification object
        var toast = new Windows.UI.Notifications.ToastNotification(toastXml);
        toastNotifier.show(toast);
    }

    function fillEventCollectionWithExistingEvents() {
        // empty the background event array and repopulate
        while (geofenceBackgroundEvents.length > 0) {
            geofenceBackgroundEvents.pop();
        }

        var settings = Windows.Storage.ApplicationData.current.localSettings;
        if (settings.values.hasKey("BackgroundGeofenceEventCollection")) {
            var geofenceEvent = settings.values["BackgroundGeofenceEventCollection"].toString();

            if (0 !== geofenceEvent.length) {
                var events = JSON.parse(geofenceEvent);

                // NOTE: the events are accessed in reverse order
                // because the events were added to JSON from
                // newer to older.  addEventDescription() adds
                // each new entry to the beginning of the collection.
                for (var pos = events.length - 1; pos >= 0; pos--) {
                    var element = events[pos].toString();
                    addEventDescription(element);
                }
            }
        }
    }

    function saveExistingEvents() {
        var settings = Windows.Storage.ApplicationData.current.localSettings;
        settings.values["BackgroundGeofenceEventCollection"] = JSON.stringify(geofenceBackgroundEvents);
    }

    function addEventDescription(eventDescription) {
        if (geofenceBackgroundEvents.length === maxEventDescriptors) {
            geofenceBackgroundEvents.pop();
        }
        geofenceBackgroundEvents.unshift(eventDescription);
    }
})();
