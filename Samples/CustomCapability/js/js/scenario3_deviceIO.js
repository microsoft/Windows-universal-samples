//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3_deviceIO.html", {
        ready: function (element, options) {
            document.getElementById("device-properties-get").addEventListener("click", onGetSegmentValue, false);
            document.getElementById("device-properties-set").addEventListener("click", onSetSegmentValue, false);
        }
    });

    function onGetSegmentValue() {

        var fx2Device = DeviceList.getSelectedFx2Device();

        if (!fx2Device) {
            WinJS.log && WinJS.log("Fx2 device not connected or accessible", "sample", "error");
            return;
        }

        var outputBuffer = new Windows.Storage.Streams.Buffer(1);

        fx2Device.sendIOControlAsync(
                    Fx2Driver.getSevenSegmentDisplay,
                    null,
                    outputBuffer
                    ).
        then(
            function (bytesRead) {
                var reader = Windows.Storage.Streams.DataReader.fromBuffer(outputBuffer);
                var segment = reader.readByte();
                segment = Fx2Driver.sevenSegmentToDigit(segment);

                if (isNaN(segment)) {
                    WinJS.log && WinJS.log("The segment display value is not yet initialized.", "sample", "status");
                } else {
                    WinJS.log && WinJS.log("The segment display value is " + segment, "sample", "status");
                }
            },

            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            }
        );
    }

    // Sets the segment display value on the Fx2 Device
    function onSetSegmentValue() {

        var fx2Device = DeviceList.getSelectedFx2Device();

        if (!fx2Device) {
            WinJS.log && WinJS.log("Fx2 device not connected or accessible", "sample", "error");
            return;
        }

        var segmentSelector = document.getElementById("device-properties-segmentInput");
        // Get the selected value to be set on the device
        var val = segmentSelector.options[segmentSelector.selectedIndex].value;

        var writer = new Windows.Storage.Streams.DataWriter();
        writer.writeByte(Fx2Driver.digitToSevenSegment(val));
        var inputBuffer = writer.detachBuffer();

        fx2Device.sendIOControlAsync(
                    Fx2Driver.setSevenSegmentDisplay,
                    inputBuffer,
                    null
                    ).
        then(
            function (bytesRead) {
                WinJS.log && WinJS.log("The segment display is set to " + val, "sample", "status");
            },

            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            }
        );
    }

})();
