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

    var lineDisplay;

    var page = WinJS.UI.Pages.define("/html/scenario2-displayText.html", {

        ready: async function (element, options) {
            blinkCheckBox = document.getElementById("blinkCheckBox");
            centerCheckBox = document.getElementById("centerCheckBox");
            displayTextButton = document.getElementById("displayTextButton");

            displayTextButton.disabled = true;
            displayTextButton.addEventListener("click", displayText);

            await initializeAsync();
        },
        unload: function () {
            lineDisplay && lineDisplay.close();
            lineDisplay = null;
        }
    });

    async function initializeAsync() {
        lineDisplay = await SdkSample.claimScenarioLineDisplayAsync();
        blinkCheckBox.disabled = !lineDisplay || (lineDisplay.capabilities.canBlink === LineDisplayTextAttributeGranularity.notSupported);
        displayTextButton.disabled = !lineDisplay;
    }

    async function displayText() {
        var text = "Hello from UWP";

        var position = { x: 0, y: 0 };
        if (centerCheckBox.checked) {
            var length = text.length;
            if (length < lineDisplay.defaultWindow.sizeInCharacters.width) {
                position.x = (lineDisplay.defaultWindow.sizeInCharacters.width - length) / 2;
            }
        }

        var attribute = LineDisplayTextAttribute.normal;

        if (blinkCheckBox.checked) {
            attribute = LineDisplayTextAttribute.blink;
        }

        if (await lineDisplay.defaultWindow.tryClearTextAsync() &&
            await lineDisplay.defaultWindow.tryDisplayTextAsync(text, attribute, position)) {
            WinJS.log("Text displayed sucessfully", "sample", "status");
        } else {
            // We probably lost our claim.
            WinJS.log("Unable to display text", "sample", "error");
        }
    }

})();