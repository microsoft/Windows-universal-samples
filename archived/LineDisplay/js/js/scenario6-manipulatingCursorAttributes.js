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
    var LineDisplayCursorType = Windows.Devices.PointOfService.LineDisplayCursorType;

    var cursorTypeSelect;
    var autoAdvanceEnabledCheckbox;
    var blinkEnabledCheckbox;
    var setCursorPositionCheckbox;
    var cursorPositionXTextbox;
    var cursorPositionYTextbox;
    var updateAttributesButton;
    var displayTextButton;
    var resetButton;

    var lineDisplay;
    var supportedCursorTypes = [];

    var page = WinJS.UI.Pages.define("/html/scenario6-manipulatingCursorAttributes.html", {

        ready: async function (element, options) {
            cursorTypeSelect = document.getElementById("cursorTypeSelect");
            autoAdvanceEnabledCheckbox = document.getElementById("autoAdvanceEnabledCheckbox");
            blinkEnabledCheckbox = document.getElementById("blinkEnabledCheckbox");
            setCursorPositionCheckbox = document.getElementById("setCursorPositionCheckbox");
            cursorPositionXTextbox = document.getElementById("cursorPositionXTextbox");
            cursorPositionYTextbox = document.getElementById("cursorPositionYTextbox");

            updateAttributesButton = document.getElementById("updateAttributesButton");
            displayTextButton = document.getElementById("displayTextButton");
            resetButton = document.getElementById("resetButton");

            updateAttributesButton.addEventListener("click", updateAttributesButton_click);
            displayTextButton.addEventListener("click", displayTextButton_click);
            resetButton.addEventListener("click", resetButton_click);

            await initializeAsync();
        },

        unload: function () {
            lineDisplay && lineDisplay.close();
            lineDisplay = null;
        }
    });

    async function initializeAsync() {
        updateAttributesButton.disabled = true;
        displayTextButton.disabled = true;
        resetButton.disabled = true;
        cursorTypeSelect.innerHTML = "";

        lineDisplay = await SdkSample.claimScenarioLineDisplayAsync();

        cursorTypeSelect.disabled = !lineDisplay || !lineDisplay.defaultWindow.cursor.canCustomize;

        if (!cursorTypeSelect.disabled) {
            if (lineDisplay.defaultWindow.cursor.isBlockSupported) {
                SdkSample.addOption(cursorTypeSelect, "Block", LineDisplayCursorType.block);
            }

            if (lineDisplay.defaultWindow.cursor.isHalfBlockSupported) {
                SdkSample.addOption(cursorTypeSelect, "HalfBlock", LineDisplayCursorType.halfBlock);
            }

            if (lineDisplay.defaultWindow.cursor.isOtherSupported) {
                SdkSample.addOption(cursorTypeSelect, "Other", LineDisplayCursorType.other);
            }

            if (lineDisplay.defaultWindow.cursor.isReverseSupported) {
                SdkSample.addOption(cursorTypeSelect, "Reverse", LineDisplayCursorType.reverse);
            }

            if (lineDisplay.defaultWindow.cursor.isUnderlineSupported) {
                SdkSample.addOption(cursorTypeSelect, "Underline", LineDisplayCursorType.underline);
            }

            cursorTypeSelect.selectedIndex = 0;
        }

        autoAdvanceEnabledCheckbox.disabled = !lineDisplay;
        blinkEnabledCheckbox.disabled = !lineDisplay || !lineDisplay.defaultWindow.cursor.isBlinkSupported;
        setCursorPositionCheckbox.disabled = !lineDisplay;
        updateAttributesButton.disabled = !lineDisplay;
        displayTextButton.disabled = !lineDisplay;

        resetButton.disabled = false;
    }

    async function updateAttributesButton_click() {
            var attributes = lineDisplay.defaultWindow.cursor.getAttributes();
            attributes.isAutoAdvanceEnabled = autoAdvanceEnabledCheckbox.checked;

            if (lineDisplay.defaultWindow.cursor.canCustomize) {
                attributes.cursorType = parseInt(cursorTypeSelect[cursorTypeSelect.selectedIndex].value);
            }

            if (lineDisplay.defaultWindow.cursor.isBlinkSupported) {
                attributes.isBlinkEnabled = blinkEnabledCheckbox.checked;
            }

        if (setCursorPositionCheckbox.checked) {
            var position = {
                x: SdkSample.parseIntWithFallback(cursorPositionXTextbox, -1),
                y: SdkSample.parseIntWithFallback(cursorPositionYTextbox, -1)
            };

            var windowSizeInCharacters = lineDisplay.defaultWindow.sizeInCharacters;

            if ((position.x < 0) || (position.x >= windowSizeInCharacters.width) ||
                (position.y < 0) || (position.y >= windowSizeInCharacters.height)) {
                WinJS.log("Cursor position must be within the window.", "sample", "error");
                return;
            }

            attributes.position = position;
        }

        if (await lineDisplay.defaultWindow.cursor.tryUpdateAttributesAsync(attributes)) {
            WinJS.log("Cursor updated.", "sample", "status");
        } else {
            WinJS.log("Failed to update the cursor.", "sample", "error");
        }
    }

    async function displayTextButton_click() {
        if (await lineDisplay.defaultWindow.tryDisplayTextAsync("Text")) {
            WinJS.log("Text displayed.", "sample", "status");
        } else {
            WinJS.log("Failed to display text.", "sample", "error");
        }
    }

    async function resetButton_click() {
        lineDisplay && lineDisplay.close();
        lineDisplay = null;
        await initializeAsync();
    }
})();