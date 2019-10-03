//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var registerButton;
    var unregisterButton;

    var page = WinJS.UI.Pages.define("/html/scenario6_customTrigger.html", {
        ready: function (element, options) {
            registerButton = document.getElementById("custom-trigger-register");
            unregisterButton = document.getElementById("custom-trigger-unregister");

            registerButton.addEventListener("click", onRegister, false);
            unregisterButton.addEventListener("click", onUnregister, false);
            updateUI();
        }
    });

    var customTriggerTaskName = "Osrusbfx2Task.ConnectedTask";
    var guidCustomSystemEventId_OsrUsbFx2 = "{C7BDD245-5CDA-4BBD-B68D-B5E36F7911A3}";

    var taskRegistration = null;

    function updateUI() {
        registerButton.disabled = taskRegistration;
        unregisterButton.disabled = !taskRegistration;
    }


    function IsFx2CustomTriggerTaskRegistered() {
        var iter = Windows.ApplicationModel.Background.BackgroundTaskRegistration.allTasks.first();
        while (iter.hasCurrent) {
            var task = iter.current.value;
            if (task.name === customTriggerTaskName) {
                return true;
            }
            iter.moveNext();
        }

        return false;
    }

    function onRegister() {
        if (IsFx2CustomTriggerTaskRegistered()) {
            return;
        }

        var builder = new Windows.ApplicationModel.Background.BackgroundTaskBuilder();
        builder.name = customTriggerTaskName;
        builder.taskEntryPoint = customTriggerTaskName;

        var customTrigger = new Windows.ApplicationModel.Background.CustomSystemEventTrigger(guidCustomSystemEventId_OsrUsbFx2, // Trigger Qualifier
                                    Windows.ApplicationModel.Background.CustomSystemEventTriggerRecurrence.Once); // One-shot trigger

        builder.setTrigger(customTrigger);
        taskRegistration = builder.register();

        updateUI();
    }

    function UnregisterTask()
    {
        if (taskRegistration) {
            taskRegistration.unregister(true);
            taskRegistration = null;
        }
    }

    function onUnregister() {
        UnregisterTask();
        updateUI();
    }

})();
