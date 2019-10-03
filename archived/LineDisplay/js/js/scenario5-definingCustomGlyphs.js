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
    var CryptographicBuffer = Windows.Security.Cryptography.CryptographicBuffer;

    var glyphSelect;
    var defineGlyphButton;
    var resetButton;

    var lineDisplay;
    var glyphBuffer;

    var page = WinJS.UI.Pages.define("/html/scenario5-definingCustomGlyphs.html", {

        ready: async function (element, options) {
            WinJS.UI.processAll();

            glyphSelect = document.getElementById("glyphSelect");
            defineGlyphButton = document.getElementById("defineGlyphButton");
            resetButton = document.getElementById("resetButton");

            defineGlyphButton.addEventListener("click", defineGlyphButton_click);
            resetButton.addEventListener("click", resetButton_click);

            await initializeAsync();
        },

        unload: function () {
            lineDisplay && lineDisplay.close();
            lineDisplay = null;
        }
    });

    async function initializeAsync() {
        resetButton.disabled = true;
        defineGlyphButton.disabled = true;
        glyphSelect.innerHTML = "";

        lineDisplay = await SdkSample.claimScenarioLineDisplayAsync();

        defineGlyphButton.disabled = !lineDisplay || !lineDisplay.capabilities.canDisplayCustomGlyphs;
        if (!defineGlyphButton.disabled) {
            var sampleText = "ABC";
            lineDisplay.customGlyphs.supportedGlyphCodes.forEach(glyphCode => {
                var glyphString = String.fromCodePoint(glyphCode);
                SdkSample.addOption(glyphSelect, `Code ${glyphCode} (UTF-32 '${glyphString}')`, glyphCode);
                if (sampleText.length < 10) {
                    sampleText += glyphString;
                }
            });
            glyphSelect.selectedIndex = 0;

            var glyphSize = lineDisplay.customGlyphs.sizeInPixels;
            glyphBuffer = createSolidGlyphBuffer(glyphSize.width, glyphSize.height);

            await lineDisplay.defaultWindow.tryDisplayTextAsync(sampleText);
        }

        resetButton.disabled = false;
    }

    async function defineGlyphButton_click() {
        var selectedGlyphCode = glyphSelect[glyphSelect.selectedIndex].value;

        if (await lineDisplay.customGlyphs.tryRedefineAsync(selectedGlyphCode, glyphBuffer) &&
            await lineDisplay.defaultWindow.tryRefreshAsync()) {
            // Success. See results on the line display.
            WinJS.log(`Redefined glyph ${selectedGlyphCode}.`, "sample", "status");
        } else {
            WinJS.log("Unable to redefine glyph and refresh.", "sample", "error");
        }
    }

    async function resetButton_click() {
        lineDisplay && lineDisplay.close();
        lineDisplay = null;
        await initializeAsync();
    }

    function createSolidGlyphBuffer(widthInPixels, heightInPixels) {
        // Data is sent in one byte chunks, so find the minimum number of whole bytes 
        // needed to define each row of the glyph.
        var bytesPerRow = (widthInPixels + 7) >> 3;

        var newGlyphBuffer = new Array(bytesPerRow * heightInPixels);
        for (var i = 0; i < newGlyphBuffer.length; i++)
        {
            // Set every bit so that we draw a solid rectangle.
            newGlyphBuffer[i] = 0xFF;
        }

        return CryptographicBuffer.createFromByteArray(newGlyphBuffer);
    }
})();