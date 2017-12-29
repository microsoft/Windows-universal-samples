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

    // Trigger details contain the information surrounding the reason why the task was triggered.
    var triggerDetails = backgroundTaskInstance.triggerDetails;

    var visitBackgroundEvents = [];
    var maxEventDescriptors = 42;   // Value determined by how many max length event descriptors (91 chars) 
    // stored as a JSON string can fit in 8K (max allowed for local settings)

    function run() {
        backgroundTaskInstance.succeeded = false;

        // Get the visit reports from the trigger details.
        getVisitReports(triggerDetails);

        backgroundTaskInstance.succeeded = true;

        // A JavaScript background task must call close when it is done
        close();
    }

    function toStateChangeString(stateChange) {
        switch (stateChange) {
            case Windows.Devices.Geolocation.VisitStateChange.arrived:
                return "Arrived";
            case Windows.Devices.Geolocation.VisitStateChange.departed:
                return "Departed";
            case Windows.Devices.Geolocation.VisitStateChange.otherMovement:
                return "Other Movement";
            case Windows.Devices.Geolocation.VisitStateChange.trackingLost:
                return "Tracking Lost";
            default:
                return "unknown";
        }
    }

    function getVisitReports(triggerDetails) {

        var formatterLongTimeBackground = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("{hour.integer}:{minute.integer(2)}:{second.integer(2)}", ["en-US"], "US", Windows.Globalization.CalendarIdentifiers.gregorian, Windows.Globalization.ClockIdentifiers.twentyFourHour);

        // First, fill visitbackground reports with the previously saved reports from local settings.
        fillReportCollectionWithExistingReports();

        var eventItem;

        // Now read the new visit reports from triggerDetails.
        var reports = triggerDetails.readReports();
        var numReports = reports.length;

        reports.forEach(function (report) {

            eventItem = formatterLongTimeBackground.format(report.timestamp);

            eventItem += " " + toStateChangeString(report.stateChange);

            if (report.position) {
                eventItem += "(" +
                    String(report.position.coordinate.point.position.latitude) + "," +
                    String(report.position.coordinate.point.position.longitude) +
                    ")";
            }

            // Add each new visit report to visitbackground reports.
            addVisitReport(eventItem);
        });

        if (numReports) {
            saveExistingReports();

            // NOTE: Other notification mechanisms can be used here, such as Badge and/or Tile updates.
            doToast(numReports, eventItem);
        }
        
    }

    function doToast(numReportsArg, eventName) {
        // pop a toast for each visit event
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

        if (1 === numReportsArg) {
            nodeList.item(1).appendChild(toastXml.createTextNode(eventName));
        } else {
            var secondLine = "There are " + numReportsArg + " new visit events";
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

    function fillReportCollectionWithExistingReports() {
        // empty the background event array and repopulate
        while (visitBackgroundEvents.length > 0) {
            visitBackgroundEvents.pop();
        }

        var settings = Windows.Storage.ApplicationData.current.localSettings;
        if (settings.values.hasKey("BackgroundVisitEventCollection")) {
            var visitEvent = settings.values["BackgroundVisitEventCollection"].toString();

            if (0 !== visitEvent.length) {
                var events = JSON.parse(visitEvent);

                // NOTE: the events are accessed in reverse order
                // because the events were added to JSON from
                // newer to older.  addVisitReport() adds
                // each new entry to the beginning of the collection
                // and throws away the last entry if the collection gets too large.
                for (var pos = events.length - 1; pos >= 0; pos--) {
                    var element = events[pos].toString();
                    addVisitReport(element);
                }
            }
        }
    }

    function saveExistingReports() {
        var settings = Windows.Storage.ApplicationData.current.localSettings;
        settings.values["BackgroundVisitEventCollection"] = JSON.stringify(visitBackgroundEvents);
    }

    function addVisitReport(eventDescription) {
        if (visitBackgroundEvents.length === maxEventDescriptors) {
            visitBackgroundEvents.pop();
        }
        visitBackgroundEvents.unshift(eventDescription);
    }

    // Do the background task work.
    run();
})();
