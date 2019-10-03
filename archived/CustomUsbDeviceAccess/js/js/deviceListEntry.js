//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    WinJS.Namespace.define(SdkSample.Constants.sampleNamespace, {
        deviceListEntry: WinJS.Class.define(
            /// <summary>
            /// The class is mainly used as a DeviceInformation wrapper so that the UI can bind to a list of these.
            /// </summary>
            /// <param name="deviceInformation"></param>
            /// <param name="deviceSelector">The AQS used to find this device</param>
            function (deviceInformation, deviceSelector) {
                this._deviceInformation = deviceInformation;
                this._deviceSelector = deviceSelector;
            }, {
                _deviceInformation: null,
                _deviceSelector: null,
                instanceId: {
                    get: function () {
                        return this._deviceInformation.properties[SdkSample.Constants.deviceProperties.deviceInstanceId];
                    }
                },
                deviceInformation: {
                    get: function () {
                        return this._deviceInformation;
                    }
                },
                deviceSelector: {
                    get: function () {
                        return this._deviceSelector;
                    }
                }
            },
            null)
    });
})();