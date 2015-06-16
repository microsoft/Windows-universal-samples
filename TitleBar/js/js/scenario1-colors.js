//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var titleBar = Windows.UI.ViewManagement.ApplicationView.getForCurrentView().titleBar;

    var page = WinJS.UI.Pages.define("/html/scenario1-colors.html", {
        ready: function (element, options) {

            useStandardColors.addEventListener("click", changeColors);
            useCustomColors.addEventListener("click", changeColors);
            transparentWhenExtended.addEventListener("click", changeColors);

            // Infer the radio button selection from the title bar colors.
            if (!titleBar.buttonBackgroundColor) {
                useStandardColors.checked = true;
            } else if (titleBar.buttonBackgroundColor.a > 0) {
                useCustomColors.checked = true;
            } else {
                transparentWhenExtended.checked = true;
            }
            reportColors();
        }
    });

    var customColors = {
        backgroundColor: { a: 255, r: 54, g: 60, b: 116 },
        foregroundColor: { a: 255, r: 232, g: 211, b: 162 },
        buttonBackgroundColor: { a: 255, r: 54, g: 60, b: 116 },
        buttonForegroundColor: { a: 255, r: 232, g: 211, b: 162 },
        buttonHoverBackgroundColor: { a: 255, r: 19, g: 21, b: 40 },
        buttonHoverForegroundColor: { a: 255, r: 255, g: 255, b: 255 },
        buttonPressedBackgroundColor: { a: 255, r: 232, g: 211, b: 162 },
        buttonPressedForegroundColor: { a: 255, r: 54, g: 60, b: 116 },
        inactiveBackgroundColor: { a: 255, r: 135, g: 141, b: 199 },
        inactiveForegroundColor: { a: 255, r: 232, g: 211, b: 162 },
        buttonInactiveBackgroundColor: { a: 255, r: 135, g: 141, b: 199 },
        buttonInactiveForegroundColor: { a: 255, r: 232, g: 211, b: 162 },
    };

    function changeColors() {
        if (useStandardColors.checked) {
            // Setting colors to null returns them to system defaults.
            titleBar.backgroundColor = null;
            titleBar.foregroundColor = null;
            titleBar.inactiveBackgroundColor = null;
            titleBar.inactiveForegroundColor = null;

            titleBar.buttonBackgroundColor = null;
            titleBar.buttonHoverBackgroundColor = null;
            titleBar.buttonPressedBackgroundColor = null;
            titleBar.buttonInactiveBackgroundColor = null;

            titleBar.buttonForegroundColor = null;
            titleBar.buttonHoverForegroundColor = null;
            titleBar.buttonPressedForegroundColor = null;
            titleBar.buttonInactiveForegroundColor = null;
        } else {
            // Title bar colors. Alpha must be 255.
            titleBar.backgroundColor = customColors.backgroundColor;
            titleBar.foregroundColor = customColors.foregroundColor;
            titleBar.inactiveBackgroundColor = customColors.inactiveBackgroundColor;
            titleBar.inactiveForegroundColor = customColors.inactiveForegroundColor;

            // Title bar button background colors. Alpha is respected when the view is extended
            // into the title bar (see scenario 2). Otherwise, Alpha is ignored and treated as if it were 255.
            var buttonAlpha = transparentWhenExtended.checked ? 0 : 255;
            customColors.buttonBackgroundColor.a = buttonAlpha;
            customColors.buttonHoverBackgroundColor.a = buttonAlpha;
            customColors.buttonPressedBackgroundColor.a = buttonAlpha;
            customColors.buttonInactiveBackgroundColor.a = buttonAlpha;

            titleBar.buttonBackgroundColor = customColors.buttonBackgroundColor;
            titleBar.buttonHoverBackgroundColor = customColors.buttonHoverBackgroundColor;
            titleBar.buttonPressedBackgroundColor = customColors.buttonPressedBackgroundColor;
            titleBar.buttonInactiveBackgroundColor = customColors.buttonInactiveBackgroundColor;

            // Title bar button foreground colors. Alpha must be 255.
            titleBar.buttonForegroundColor = customColors.buttonForegroundColor;
            titleBar.buttonHoverForegroundColor = customColors.buttonHoverForegroundColor;
            titleBar.buttonPressedForegroundColor = customColors.buttonPressedForegroundColor;
            titleBar.buttonInactiveForegroundColor = customColors.buttonInactiveForegroundColor;
        }

        reportColors();
    }

    function reportColors() {
        var result = "";
        for (var p in customColors) {
            var v = titleBar[p];
            result += "Custom " + p + " = " + JSON.stringify(v) + "\n";
        }
        currentColorsDiv.innerText = result;
    }
})();
