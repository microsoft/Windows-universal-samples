//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Custom = Windows.Devices.Custom;
    var deviceType = 0x5500;
    var functionBase = 0x800;

    var sevenSegmentValues = [
        0xD7, // 0
        0x06, // 1
        0xB3, // 2
        0xA7, // 3
        0x66, // 4
        0xE5, // 5
        0xF4, // 6
        0x07, // 7
        0xF7, // 8
        0x67, // 9
    ];

    //
    // Define a class to hold entries in the list of devices.
    //
    WinJS.Namespace.define(
        "Fx2Driver",
        {
            //deviceType:             deviceType, 
            //functionBase:           functionBase,

            getSevenSegmentDisplay: new Custom.IOControlCode(deviceType, 
                                                             functionBase + 7,
                                                             Custom.IOControlAccessMode.read,
                                                             Custom.IOControlBufferingMethod.buffered),

            setSevenSegmentDisplay: new Custom.IOControlCode(deviceType,
                                                             functionBase + 8,
                                                             Custom.IOControlAccessMode.write,
                                                             Custom.IOControlBufferingMethod.buffered),

            readSwitches:           new Custom.IOControlCode(deviceType,
                                                             functionBase + 6,
                                                             Custom.IOControlAccessMode.read,
                                                             Custom.IOControlBufferingMethod.buffered),

            getInterruptMessage:    new Custom.IOControlCode(deviceType, 
                                                             functionBase + 9,
                                                             Custom.IOControlAccessMode.read,
                                                             Custom.IOControlBufferingMethod.directOutput),

            digitToSevenSegment:    function (value) { return sevenSegmentValues[value]; },

            sevenSegmentToDigit:    function (value) {
                                        for (var i = 0; i < sevenSegmentValues.length; i += 1) {
                                            if (sevenSegmentValues[i] === value) {
                                                return i;
                                            }
                                        }
                                        return NaN;
                                    },

            deviceInterfaceGuid:    "573E8C73-0CB4-4471-A1BF-FAB26C31D384"
        }
    );
})();
