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

(function () {
    "use strict";

    // We use a DeviceWatcher instead of DeviceInformation.FindAllAsync because
    // the DeviceWatcher will let us see the devices as they are discovered,
    // whereas FindAllAsync returns results only after discovery is complete.
    //
    // The convertAsync function is passed a device ID and returns a Promise
    // whose result is some type of device.

    function getFirstDeviceAsync(selector, convertAsync) {
        var pendingPromises = [];
        var watcher = Windows.Devices.Enumeration.DeviceInformation.createWatcher(selector, null);
        return new WinJS.Promise(function (c) {
            watcher.addEventListener("added", function (device) {
                pendingPromises.push(convertAsync(device.id).then(function (t) {
                    if (t) {
                        c(t);
                    }
                }));
            });
            watcher.addEventListener("enumerationcompleted", function () {
                // Wait for completion of all the promises we created in our "added" event andler.
                WinJS.Promise.join(pendingPromises).done(function () {
                    // This sets the result to "null" if no promise was able to produce a device.
                    c(null);
                });
            });
            watcher.start();
        }).then(function (t) {
            watcher.stop();
            return t;
        });
    }

    WinJS.Namespace.define("DeviceHelpers", {
        getFirstDeviceAsync: getFirstDeviceAsync
    });
})();