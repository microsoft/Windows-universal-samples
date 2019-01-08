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

    var newWindowButton;
    var refreshWindowButton;
    var destroyWindowButton;
    var displayTextButton;
    var displayTextTextbox;

    var viewportBoundsWidthTextbox;
    var viewportBoundsHeightTextbox;
    var viewportBoundsXTextbox;
    var viewportBoundsYTextbox;
    var windowWidthTextbox;
    var windowHeightTextbox;

    var windowListView;

    var lineDisplay;
    var windows = [];
    var nextWindowId = 0;
    var maxWindows;

    WinJS.Namespace.define("SdkSample.Scenario3", {
        windowList: new WinJS.Binding.List(windows)
    });

    var page = WinJS.UI.Pages.define("/html/scenario3-usingWindows.html", {

        ready: async function (element, options) {
            WinJS.UI.processAll();

            newWindowButton = document.getElementById("newWindowButton");
            refreshWindowButton = document.getElementById("refreshWindowButton");
            destroyWindowButton = document.getElementById("destroyWindowButton");
            displayTextButton = document.getElementById("displayTextButton");
            displayTextTextbox = document.getElementById("displayTextTextbox");

            viewportBoundsWidthTextbox = document.getElementById("viewportBoundsWidthTextbox");
            viewportBoundsHeightTextbox = document.getElementById("viewportBoundsHeightTextbox");
            viewportBoundsXTextbox = document.getElementById("viewportBoundsXTextbox");
            viewportBoundsYTextbox = document.getElementById("viewportBoundsYTextbox");
            windowWidthTextbox = document.getElementById("windowWidthTextbox");
            windowHeightTextbox = document.getElementById("windowHeightTextbox");

            windowListView = document.getElementById("windowListView");

            newWindowButton.addEventListener("click", newWindowButton_click);
            refreshWindowButton.addEventListener("click", refreshWindowButton_click);
            destroyWindowButton.addEventListener("click", destroyWindowButton_click);
            displayTextButton.addEventListener("click", displayTextButton_click);

            windowListView.winControl.addEventListener("selectionchanged", updateButtons);

            await initializeAsync();
        },
        unload: function () {
            lineDisplay && lineDisplay.close();
            lineDisplay = null;
            clearWindowList();
        }
    });

    async function initializeAsync() {
        maxWindows = 0;
        newWindowButton.disabled = true;
        lineDisplay = await SdkSample.claimScenarioLineDisplayAsync();

        maxWindows = lineDisplay && lineDisplay.capabilities.supportedWindows;
        if (maxWindows >= 1) {
            // Create an entry to represent the default window.
            addWindowToList(lineDisplay.defaultWindow);
            newWindowButton.disabled = false;
        }
        updateButtons();
    }

    function updateButtons() {
        // Disable the "New Window" button if there is no room for another window.
        newWindowButton.disabled = (SdkSample.Scenario3.windowList.length >= maxWindows);

        // Disable the "Refresh" and "Display" buttons if there is no selection.
        var selectedWindow = getSingleSelectedWindow();
        refreshWindowButton.disabled = !selectedWindow;
        displayTextButton.disabled = !selectedWindow;

        // Disable the "Destroy" button if there is no selection,
        // or the selection is the default window.
        destroyWindowButton.disabled = !selectedWindow || selectedWindow.id == 0;
    }

    async function newWindowButton_click() {
        if (SdkSample.Scenario3.windowList.length >= maxWindows) {
            WinJS.log(`Too many windows created. The line display only supports ${maxWindows} windows.`, "sample", "error");
            return;
        }

        var viewportBounds = {
            x: SdkSample.parseIntWithFallback(viewportBoundsXTextbox, -1),
            y: SdkSample.parseIntWithFallback(viewportBoundsYTextbox, -1),
            width: SdkSample.parseIntWithFallback(viewportBoundsWidthTextbox, 0),
            height: SdkSample.parseIntWithFallback(viewportBoundsHeightTextbox, 0)
        };

        if ((viewportBounds.width <= 0) || (viewportBounds.height <= 0)) {
            WinJS.log("Viewport size must be be positive.", "sample", "error");
            return;
        }

        // Viewport cannot extend beyond the screen.
        var screenSizeInCharacters = lineDisplay.getAttributes().screenSizeInCharacters;
        if ((viewportBounds.x < 0) || (viewportBounds.y < 0) ||
            (viewportBounds.x + viewportBounds.width > screenSizeInCharacters.width) ||
            (viewportBounds.y + viewportBounds.height > screenSizeInCharacters.height)) {
            WinJS.log("Viewport cannot extend beyond the screen.", "sample", "error");
            return;
        }

        var windowSize = {
            width: SdkSample.parseIntWithFallback(windowWidthTextbox, 0),
            height: SdkSample.parseIntWithFallback(windowHeightTextbox, 0)
        };

        if ((windowSize.width <= 0) || (windowSize.height <= 0)) {
            WinJS.log("Window size must be be positive.", "sample", "error");
            return;
        }

        // Windows must be at least as large as their viewports.
        if ((viewportBounds.width > windowSize.width) || (viewportBounds.height > windowSize.height)) {
            WinJS.log("Window must be at least as large as viewport.", "sample", "error");
            return;
        }

        // Window taller than viewport requires IsVerticalMarqueeSupported.
        if ((windowSize.height > viewportBounds.height) && !lineDisplay.capabilities.isVerticalMarqueeSupported) {
            WinJS.log("Window cannot be taller than viewport on this line display.", "sample", "error");
            return;
        }

        // Window wider than viewport requires IsHorizontalMarqueeSupported.
        if ((windowSize.width > viewportBounds.width) && !lineDisplay.capabilities.isHorizontalMarqueeSupported) {
            WinJS.log("Window cannot be wider than viewport on this line display.", "sample", "error");
            return;
        }

        // Window cannot be larger than viewport in both directions.
        // (Two-dimensional scrolling is not supported.)
        if ((windowSize.width > viewportBounds.width) && (windowSize.height > viewportBounds.height)) {
            WinJS.log("Window cannot be larger than viewport in both dimensions.", "sample", "error");
            return;
        }

        var displayWindow = await lineDisplay.tryCreateWindowAsync(viewportBounds, windowSize);
        if (displayWindow) {
            var newWindowInfo = addWindowToList(displayWindow);
            WinJS.log(`Created window ${newWindowInfo.id}.`, "sample", "status");
        } else {
            // We probably lost our claim.
            WinJS.log("Failed to create a new window.", "sample", "error");
        }
    }

    function getSingleSelectedWindow() {
        var selectedWindows = windowListView.winControl.selection.getIndices();
        if (selectedWindows.length) {
            return SdkSample.Scenario3.windowList.getAt(selectedWindows[0]);
        }
        return null;
    }

    function refreshWindowButton_click() {
        var selectedWindow = getSingleSelectedWindow();
        selectedWindow.window.tryRefreshAsync();
    }

    function destroyWindowButton_click() {
        var selectedWindow = getSingleSelectedWindow();
        removeWindowFromList(selectedWindow);
    }

    function displayTextButton_click() {
        var selectedWindow = getSingleSelectedWindow();
        selectedWindow.window.tryDisplayTextAsync(displayTextTextbox.value);
    }

    function clearWindowList() {
        nextWindowId = 0;
        SdkSample.Scenario3.windowList.splice(0, SdkSample.Scenario3.windowList.length);
        updateButtons();
    }

    function addWindowToList(window) {
        var nameSuffix = (nextWindowId == 0) ? " (Default)" : "";
        var name = "Window " + nextWindowId + nameSuffix;

        var newWindowInfo = { id: nextWindowId, name: name, window: window };
        SdkSample.Scenario3.windowList.push(newWindowInfo);
        nextWindowId++;
        updateButtons();

        return newWindowInfo;
    }

    function removeWindowFromList(windowInfo) {
        // The LineDisplayWindow class implements IDispose. Closing a LineDisplayWindow results 
        // in its destruction on the line display device and invalidation of the object.
        windowInfo.window.close();
        var index = SdkSample.Scenario3.windowList.indexOf(windowInfo);
        SdkSample.Scenario3.windowList.splice(index, 1);
        updateButtons();
    }
})();