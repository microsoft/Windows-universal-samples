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
    var LineDisplay = Windows.Devices.PointOfService.LineDisplay;
    var ClaimedLineDisplay = Windows.Devices.PointOfService.ClaimedLineDisplay;
    var LineDisplayTextAttribute = Windows.Devices.PointOfService.LineDisplayTextAttribute;
    var LineDisplayTextAttributeGranularity = Windows.Devices.PointOfService.LineDisplayTextAttributeGranularity;

    var blinkCheckBox;
    var centerCheckBox;
    var displayTextButton;
    var blinkNotSupportedText;

    var page = WinJS.UI.Pages.define("/html/scenario2-displayText.html", {

        ready: function (element, options) {
            blinkCheckBox = document.getElementById("blinkCheckBox");
            centerCheckBox = document.getElementById("centerCheckBox");
            displayTextButton = document.getElementById("displayTextButton");
            blinkNotSupportedText = document.getElementById("blinkNotSupportedText");

            displayTextButton.addEventListener("click", displayText, false);

            if (!SdkSample.lineDisplayId) {
                WinJS.log("You must use scenario 1 to select a line display", "sample", "error");
                displayTextButton.disabled = true;
            }
        }
    });

    function displayText() {
        ClaimedLineDisplay.fromIdAsync(SdkSample.lineDisplayId).done(function (lineDisplay) {
            if (lineDisplay) {
                var text = "Hello from UWP";

                var position = { x: 0, y: 0 };
                if (centerCheckBox.checked) {
                    var length = text.length;
                    if (length < lineDisplay.defaultWindow.sizeInCharacters.width) {
                        position.x = (lineDisplay.defaultWindow.sizeInCharacters.width - length) / 2;
                    }
                }

                var attribute = LineDisplayTextAttribute.normal;

                // If blinking is requested, verify that the device supports blinking.
                if (blinkCheckBox.checked) {
                    if (lineDisplay.capabilities.canBlink === LineDisplayTextAttributeGranularity.notSupported) {
                        blinkNotSupportedText.style.display = "block";
                    } else {
                        // Device supports blinking.
                        attribute = LineDisplayTextAttribute.blink;
                    }
                }

                lineDisplay.defaultWindow.tryClearTextAsync().then(function (result) {
                    if (result) {
                        return lineDisplay.defaultWindow.tryDisplayTextAsync(text, attribute, position);
                    } else {
                        return result;
                    }
                }).done(function (result) {
                    if (result) {
                        WinJS.log("Text displayed sucessfully", "sample", "status");
                    } else {
                        // We probably lost our claim.
                        WinJS.log("Unable to display text", "sample", "error");
                    }

                    // Close the claimed line display.
                    lineDisplay.close();
                });
            } else {
                WinJS.log("Unable to connect to the selected Line Display", "sample", "error");
            }
        });
    }

})();