(function () {
    "use strict";
    
    var DevEnum = Windows.Devices.Enumeration;
    var taskInstance = Windows.UI.WebUI.WebUIBackgroundTaskInstance.current;
    var settings = Windows.Storage.ApplicationData.current.localSettings;
    var eventCount = 0;

    Debug.writeln("BackgroundDeviceWatcher method called");

    var triggerDetails = taskInstance.triggerDetails;

    Debug.writeln("Trigger contains " + triggerDetails.deviceWatcherEvents.count + " events");

    for (var i = 0; i < triggerDetails.deviceWatcherEvents.count; i++) {
        var e = triggerDetails.deviceWatcherEvents.getAt(i);

        switch(e.kind)
        {
            case DevEnum.DeviceWatcherEventKind.add:
                Debug.writeln("Add: " + e.deviceInformation.id)
                break;

            case DevEnum.DeviceWatcherEventKind.update:
                Debug.writeln("Update: " + e.deviceInformation.id)
                break;

            case DevEnum.DeviceWatcherEventKind.remove:
                Debug.writeln("Remove: " + e.deviceInformation.id)
                break;
        }
    }

    if (null != settings.values["eventCount"]) {
        eventCount = settings.values["eventCount"];
    }

    // Add the number of events for this trigger to the number of events received in the past
    eventCount += triggerDetails.deviceWatcherEvents.count;

    Debug.writeln(eventCount + " events processed for lifetime of trigger");

    settings.values["eventCount"] = eventCount;

})();