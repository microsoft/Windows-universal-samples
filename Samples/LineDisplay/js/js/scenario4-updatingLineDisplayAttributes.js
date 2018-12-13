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
    var ClaimedLineDisplay = Windows.Devices.PointOfService.ClaimedLineDisplay;
    var LineDisplayTextAttribute = Windows.Devices.PointOfService.LineDisplayTextAttribute;
    var LineDisplayTextAttributeGranularity = Windows.Devices.PointOfService.LineDisplayTextAttributeGranularity;

    var blinkRateSlider;
    var blinkIntervalSpan;
    var brightnessSlider;
    var brightnessSpan;
    var screenSizeInCharactersSelect;
    var characterSetMappingEnabledCheckbox;

    var lineDisplay;

    var page = WinJS.UI.Pages.define("/html/scenario4-updatingLineDisplayAttributes.html", {

        ready: async function (element, options) {
            WinJS.UI.processAll();

            blinkRateSlider = document.getElementById("blinkRateSlider");
            blinkIntervalSpan = document.getElementById("blinkIntervalSpan");
            brightnessSlider = document.getElementById("brightnessSlider");
            brightnessSpan = document.getElementById("brightnessSpan");
            screenSizeInCharactersSelect = document.getElementById("screenSizeInCharactersSelect");
            characterSetMappingEnabledCheckbox = document.getElementById("characterSetMappingEnabledCheckbox");

            blinkRateSlider.addEventListener("input", updateBlinkRateSliderValue);
            brightnessSlider.addEventListener("input", updateBrightnessSliderValue);
            updateButton.addEventListener("click", updateButton_click);

            await initializeAsync();

            setValuesFromLineDisplay();
        },

        unload: function () {
            lineDisplay && lineDisplay.close();
            lineDisplay = null;
        }
    });

    async function initializeAsync() {
        lineDisplay = await SdkSample.claimScenarioLineDisplayAsync();

        blinkRateSlider.disabled = !lineDisplay || !lineDisplay.capabilities.canChangeBlinkRate;
        brightnessSlider.disabled = !lineDisplay || !lineDisplay.capabilities.isBrightnessSupported;

        screenSizeInCharactersSelect.disabled = !lineDisplay || !lineDisplay.capabilities.canChangeScreenSize;
        if (!screenSizeInCharactersSelect.disabled) {
            lineDisplay.supportedScreenSizesInCharacters.forEach(screenSize => {
                SdkSample.addOption(screenSizeInCharactersSelect, `${screenSize.width} x ${screenSize.height}`);
            });

            screenSizeInCharactersSelect.selectedIndex = 0;
        }

        characterSetMappingEnabledCheckbox.disabled = !lineDisplay || !lineDisplay.capabilities.canMapCharacterSets;

        updateButton.disabled = !lineDisplay;

        if (!updateButton.disabled) {
            if (lineDisplay.capabilities.canBlink === LineDisplayTextAttributeGranularity.notSupported) {
                await lineDisplay.defaultWindow.tryDisplayTextAsync("Regular Text");
            } else {
                await lineDisplay.defaultWindow.tryDisplayTextAsync("Blinking Text", LineDisplayTextAttribute.blink);
            }
        }
    }

    function setValuesFromLineDisplay() {
        if (lineDisplay) {
            var attributes = lineDisplay.getAttributes();
            blinkRateSlider.value = attributes.blinkRate;
            brightnessSlider.value = attributes.brightness;
            characterSetMappingEnabledCheckbox.checked = attributes.isCharacterSetMappingEnabled;
        }
        updateBlinkRateSliderValue();
        updateBrightnessSliderValue();
    }

    function updateBlinkRateSliderValue() {
        blinkIntervalSpan.innerHTML = blinkRateSlider.value;
    }

    function updateBrightnessSliderValue() {
        brightnessSpan.innerHTML = brightnessSlider.value;
    }

    async function updateButton_click() {
        var attributes = lineDisplay.getAttributes();

        if (lineDisplay.capabilities.canChangeBlinkRate) {
            attributes.blinkRate = blinkRateSlider.value;
        }

        if (lineDisplay.capabilities.isBrightnessSupported) {
            attributes.brightness = brightnessSlider.value;
        }

        if (lineDisplay.capabilities.canChangeScreenSize) {
            attributes.screenSizeInCharacters = lineDisplay.supportedScreenSizesInCharacters[screenSizeInCharactersSelect.selectedIndex];
        }

        if (lineDisplay.capabilities.canMapCharacterSets) {
            attributes.isCharacterSetMappingEnabled = characterSetMappingEnabledCheckbox.checked;
        }

        if (await lineDisplay.tryUpdateAttributesAsync(attributes)) {
            WinJS.log("Attributes updated successfully.", "sample", "status");

            // The resulting attributes may not match our request.
            // For example, the Line Display will choose the closest available blink rate.
            // Update the controls to show what the Line Display actually used.
            setValuesFromLineDisplay();
        } else {
            // We probably lost our claim.
            WinJS.log("Failed to update attributes.", "sample", "error");
        }
    }
}) ();