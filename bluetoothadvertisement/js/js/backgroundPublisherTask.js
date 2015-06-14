//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    // This var is used to get information about the current instance of the background task.
    var backgroundTaskInstance = Windows.UI.WebUI.WebUIBackgroundTaskInstance.current;

    // Trigger details contain the information surrounding the reason why the this task was triggered
    var details = backgroundTaskInstance.triggerDetails;

    function run() {
        // If the background publisher stopped unexpectedly, an error will be available here.
        var error = details.error;

        // The status of the publisher is useful to determine whether the advertisement payload is being serviced
        // It is possible for a publisher to stay in a Waiting state while radio resources are in use.
        var status = details.status;

        // In this example, the background task simply constructs a message communicated
        // to the App. For more interesting applications, a notification can be sent from here instead.
        var eventMessage = "Publisher Status: " + status.toString() + ", Error: " + status.toString();

        // Store the message in a local settings indexed by this task's name so that the foreground App
        // can display this message.
        Windows.Storage.ApplicationData.current.localSettings.values[backgroundTaskInstance.task.name] = eventMessage;

        // A JavaScript background task must call close when done
        close();
    }

    run();
})();
