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
    var LineDisplayScrollDirection = Windows.Devices.PointOfService.LineDisplayScrollDirection;
    var LineDisplayMarqueeFormat = Windows.Devices.PointOfService.LineDisplayMarqueeFormat;

    var marqueeFormatSelect;
    var scrollDirectionSelect;
    var repeatWaitIntervalSlider;
    var repeatWaitIntervalSpan;
    var scrollWaitIntervalSlider;
    var scrollWaitIntervalSpan;

    var startScrollingButton;
    var stopScrollingButton;

    var lineDisplay;
    var horizontalScrollableWindow;
    var verticalScrollableWindow;

    var page = WinJS.UI.Pages.define("/html/scenario7-scrollingContentUsingMarquee.html", {

        ready: async function (element, options) {
            WinJS.UI.processAll();

            marqueeFormatSelect = document.getElementById("marqueeFormatSelect");
            scrollDirectionSelect = document.getElementById("scrollDirectionSelect");
            repeatWaitIntervalSlider = document.getElementById("repeatWaitIntervalSlider");
            repeatWaitIntervalSpan = document.getElementById("repeatWaitIntervalSpan");
            scrollWaitIntervalSlider = document.getElementById("scrollWaitIntervalSlider");
            scrollWaitIntervalSpan = document.getElementById("scrollWaitIntervalSpan");

            startScrollingButton = document.getElementById("startScrollingButton");
            stopScrollingButton = document.getElementById("stopScrollingButton");

            repeatWaitIntervalSlider.addEventListener("input", updateRepeatWaitIntervalSliderValue);
            scrollWaitIntervalSlider.addEventListener("input", updateScrollWaitIntervalSliderValue);

            startScrollingButton.addEventListener("click", startScrollingButton_click);
            stopScrollingButton.addEventListener("click", stopScrollingButton_click);

            SdkSample.addOption(marqueeFormatSelect, "None", LineDisplayMarqueeFormat.none);
            SdkSample.addOption(marqueeFormatSelect, "Place", LineDisplayMarqueeFormat.place);
            SdkSample.addOption(marqueeFormatSelect, "Walk", LineDisplayMarqueeFormat.walk);

            await initializeAsync();

            updateRepeatWaitIntervalSliderValue();
            updateScrollWaitIntervalSliderValue();
        },

        unload: function () {
            horizontalScrollableWindow && horizontalScrollableWindow.close();
            horizontalScrollableWindow = null;
            verticalScrollableWindow && verticalScrollableWindow.close();
            verticalScrollableWindow = null;
            lineDisplay && lineDisplay.close();
            lineDisplay = null;
        }
    });

    async function initializeAsync() {
        startScrollingButton.disabled = true;
        stopScrollingButton.disabled = true;
        scrollDirectionSelect.innerHTML = "";

        lineDisplay = await SdkSample.claimScenarioLineDisplayAsync();

        if (lineDisplay) {
            var screenSize = lineDisplay.getAttributes().screenSizeInCharacters;

            var maxWindows = lineDisplay.capabilities.supportedWindows;
            var windowCount = 1;

            if (lineDisplay.capabilities.isHorizontalMarqueeSupported && windowCount < maxWindows) {
                // Create a horizontal scrollable window by creating a window
                // whose width is wider than the viewport.
                horizontalScrollableWindow = await lineDisplay.tryCreateWindowAsync(
                    {
                        x: 0,
                        y: 0,
                        width: screenSize.width,
                        height: screenSize.height
                    }, {
                        width: screenSize.width * 2,
                        height: screenSize.height
                    });

                if (horizontalScrollableWindow) {
                    windowCount++;
                    SdkSample.addOption(scrollDirectionSelect, "Left", LineDisplayScrollDirection.left);
                    SdkSample.addOption(scrollDirectionSelect, "Right", LineDisplayScrollDirection.right);
                }
            }

            if (lineDisplay.capabilities.isVerticalMarqueeSupported && windowCount < maxWindows) {
                // Create a vertical scrollable window by creating a window
                // whose height is taller than the viewport.
                verticalScrollableWindow = await lineDisplay.tryCreateWindowAsync(
                    {
                        x: 0,
                        y: 0,
                        width: screenSize.width,
                        height: screenSize.height
                    }, {
                        width: screenSize.width,
                        height: screenSize.height * 2
                    });
                if (verticalScrollableWindow) {
                    windowCount++;
                    SdkSample.addOption(scrollDirectionSelect, "Up", LineDisplayScrollDirection.up);
                    SdkSample.addOption(scrollDirectionSelect, "Down", LineDisplayScrollDirection.down);
                }
            }

            if (scrollDirectionSelect.firstChild) {
                scrollDirectionSelect.selectedIndex = 0;
                startScrollingButton.disabled = false;
                stopScrollingButton.disabled = false;
            }
        }
    }

    function updateRepeatWaitIntervalSliderValue() {
        repeatWaitIntervalSpan.innerHTML = repeatWaitIntervalSlider.value;
    }

    function updateScrollWaitIntervalSliderValue() {
        scrollWaitIntervalSpan.innerHTML = scrollWaitIntervalSlider.value;
    }

    async function startScrollingButton_click() {
        var selectedScrollDirection = parseInt(scrollDirectionSelect[scrollDirectionSelect.selectedIndex].value);
        var selectedMarqueeFormat = parseInt(marqueeFormatSelect[marqueeFormatSelect.selectedIndex].value);
        var selectedWindow;

        if ((selectedScrollDirection === LineDisplayScrollDirection.left) || (selectedScrollDirection === LineDisplayScrollDirection.right)) {
            selectedWindow = horizontalScrollableWindow;
        } else {
            selectedWindow = verticalScrollableWindow;
        }

        // Cannot make changes while scrolling is active.
        await stopAllScrollingAsync();

        // Bring the window to the front, clear its old contents,
        // and display some text that will require scrolling.
        await selectedWindow.tryRefreshAsync();
        await selectedWindow.tryClearTextAsync();
        await selectedWindow.tryDisplayTextAsync(
            "An incredibly lengthy string of arbitrary text that cannot be displayed on most line displays " +
            "without being scrolled into view. It will need to be scrolled into view left, right, up or down.");

        // Configure the scrolling behavior and start scrolling.
        selectedWindow.marquee.format = selectedMarqueeFormat;
        selectedWindow.marquee.repeatWaitInterval = repeatWaitIntervalSlider.value;
        selectedWindow.marquee.scrollWaitInterval = scrollWaitIntervalSlider.value;
        await selectedWindow.marquee.tryStartScrollingAsync(selectedScrollDirection);
    }

    async function stopAllScrollingAsync() {
        if (horizontalScrollableWindow) {
            await horizontalScrollableWindow.marquee.tryStopScrollingAsync();
        }

        if (verticalScrollableWindow) {
            await verticalScrollableWindow.marquee.tryStopScrollingAsync();
        }
    }

    async function stopScrollingButton_click() {
        await stopAllScrollingAsync();
    }
})();