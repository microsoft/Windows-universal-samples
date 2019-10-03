//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/disableAnimations.html", {
        ready: function (element, options) {
            incrementCount.addEventListener("click", incrementEnableCount, false);
            decrementCount.addEventListener("click", decrementEnableCount, false);
            runAnimation.addEventListener("click", runRepositionAnimation, false);
            refresh.addEventListener("click", updateStatus, false);

            // In the web compartment where WinRT is unavailable, the system animation setting
            // will be assumed to have the default value, which is animations are enabled.
            if (WinJS.Utilities.hasWinRT) {
                animationSettings = new Windows.UI.ViewManagement.UISettings();
            } else {
                animationSettings = { animationsEnabled: true };
            }

            animationTarget.style.marginLeft = "0px";
            updateStatus();
        }
    });

    var count = 0;
    var animationSettings;

    function incrementEnableCount() {
        // Enable Animations increases the internal enable count.
        WinJS.UI.enableAnimations();
        count++;
        updateStatus();
    }

    function decrementEnableCount() {
        // Disable Animations decreases the internal enable count.
        WinJS.UI.disableAnimations();
        count--;
        updateStatus();
    }

    function runRepositionAnimation() {
        updateStatus();

        var reposition = WinJS.UI.Animation.createRepositionAnimation(animationTarget);

        if (animationTarget.style.marginLeft === "0px") {
            animationTarget.style.marginLeft = "500px";
        } else {
            animationTarget.style.marginLeft = "0px";
        }

        reposition.execute();
    }

    function updateStatus() {
        enableCount.innerText = "Enable count = " + count;

        if (animationSettings.animationsEnabled) {
            systemAnimationSetting.innerText = "System 'Turn off unnecessary animations' setting = false.";
        } else {
            systemAnimationSetting.innerText = "System 'Turn off unnecessary animations' setting = true.";
        }

        // Whether animations are enabled depends on both the system animation setting and the internal enable count.
        // Is Animation Enabled returns whether or not animations are currently enabled.
        isAnimationEnabled.innerText = "Animations enabled = " + WinJS.UI.isAnimationEnabled();
    }
})();
