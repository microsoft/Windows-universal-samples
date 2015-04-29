//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    WinJS.Namespace.define("SdkSample", {
        onDeviceConnectedEventArgsClass: WinJS.Class.define(
            function (isDeviceSuccessfullyConnected, deviceInformation) {
                this._isDeviceSuccessfullyConnected = isDeviceSuccessfullyConnected;
                this._deviceInformation = deviceInformation;
            }, {
                _isDeviceSuccessfullyConnected: null,
                _deviceInformation: null,
                isDeviceSuccessfullyConnected: {
                    get: function () {
                        return this._isDeviceSuccessfullyConnected;
                    }
                },
                deviceInformation: {
                    get: function () {
                        return this._deviceInformation;
                    }
                }
            },
            null)
    });

    WinJS.Namespace.define("SdkSample.Constants", {
        sampleNamespace: "SdkSample.CustomSerialDeviceAccess",
        deviceType: {
            osrFx2: 0,
            superMutt: 1,
            all: 2,
            none: 3
        },
        descriptor: {
            deviceDescriptor: 0,
            configurationDescriptor: 1,
            interfaceDescriptor: 2,
            endpointDescriptor: 3,
            stringDescriptor: 4,
            customDescriptor: 5
        },
        localSettingKeys: {
            syncBackgroundTaskStatus: "SyncBackgroundTaskStatus",
            syncBackgroundTaskResult: "SyncBackgroundTaskResult"
        },
        syncBackgroundTaskInformation: {
            name: "SyncBackgroundTask",
            taskEntryPoint: "js\\ioSyncBackgroundTask.js",
            taskCanceled: "Canceled",
            taskCompleted: "Completed"
        },
        deviceProperties: {
            deviceInstanceId: "System.Devices.DeviceInstanceId",
        },
        osrFx2: {
            vendorCommand: {
                getSevenSegment: 0xD4,
                getSwitchState: 0xD6,
                setSevenSegment: 0xDB
            },
            pipe:
            {
                interruptInPipeIndex: 0,
                bulkInPipeIndex: 0,
                bulkOutPipeIndex: 0
            },
            sevenLedSegmentMask: [
                0xD7,   // 0
                0x06,   // 1
                0xB3,   // 2
                0xA7,   // 3
                0x66,   // 4
                0xE5,   // 5
                0xF4,   // 6
                0x07,   // 7
                0xF7,   // 8
                0x67    // 0
            ],
            deviceVid: 0x0547,
            devicePid: 0x1002
        },
        superMutt: {
            vendorCommand: {
                getLedBlinkPattern: 0x03,
                setLedBlinkPattern: 0x03
            },
            pipe:
            {
                interruptInPipeIndex: 0,
                interruptOutPipeIndex: 0,
                bulkInPipeIndex: 0,
                bulkOutPipeIndex: 0
            },
            deviceVid: 0x045E,
            devicePid: 0x0611,
            deviceInterfaceClass: "{875D47FC-D331-4663-B339-624001A2DC5E}"
        },
        sync: {
            bytesToWriteAtATime: 512,
            numberOfTimesToWrite: 2
        }
    });
})();
