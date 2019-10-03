//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1-metrics.html", {
        ready: function (element, options) {
            animationType.addEventListener("change", getAnimationDescriptionString, false);
        }
    });
    function getAnimationDescriptionString() {
        var animationMetrics = Windows.UI.Core.AnimationMetrics;
        var effect;
        var target;

        // Get animation and animation target specified by user control.
        switch (animationType.value) {
            case "added":
                effect = animationMetrics.AnimationEffect.addToList;
                target = animationMetrics.AnimationEffectTarget.added;
                break;
            case "affected":
                effect = animationMetrics.AnimationEffect.addToList;
                target = animationMetrics.AnimationEffectTarget.affected;
                break;
            case "enterPage":
                effect = animationMetrics.AnimationEffect.enterPage;
                target = animationMetrics.AnimationEffectTarget.primary;
                break;
        }

        // Get top level animation storyboard description values
        var animationDescription = new animationMetrics.AnimationDescription(effect, target);
        var s = "Stagger delay = " + animationDescription.staggerDelay + "\n";
        s += "Stagger delay factor = " + animationDescription.staggerDelayFactor + "\n";
        s += "Delay limit = " + animationDescription.delayLimit + "\n";
        s += "ZOrder = " + animationDescription.zorder + "\n";

        // Get animation type and parameters for each animation primitive that makes up the animation storyboard.
        var animations = animationDescription.animations;
        for (var i = 0; i < animations.size; i++) {
            s += "\nAnimation #" + (i + 1) + ":" + "\n";
            var animation = animations[i];

            // Get animation parameters based on type of animation. Animation Library storyboards are made up of scale, translation, and opacity animations.
            switch (animation.type) {
                case animationMetrics.PropertyAnimationType.scale:
                    s += "Type = Scale\n";
                    if (animation.initialScaleX !== null) {
                        s += "InitialScaleX = " + animation.initialScaleX + "\n";
                    }
                    if (animation.initialScaleY !== null) {
                        s += "InitialScaleY = " + animation.initialScaleY + "\n";
                    }
                    s += "FinalScaleX = " + animation.finalScaleX + "\n";
                    s += "FinalScaleY = " + animation.finalScaleY + "\n";
                    s += "Origin = " + animation.normalizedOrigin.x + ", " + animation.normalizedOrigin.y + "\n";
                    break;
                case animationMetrics.PropertyAnimationType.translation:
                    s += "Type = Translation\n";
                    break;
                case animationMetrics.PropertyAnimationType.opacity:
                    s += "Type = Opacity\n";

                    if (animation.initialOpacity !== null) {
                        s += "InitialOpacity = " + animation.initialOpacity + "\n";
                    }
                    s += "FinalOpacity = " + animation.finalOpacity + "\n";
                    break;
            }

            s += "Delay = " + animation.delay + "\n";
            s += "Duration = " + animation.duration + "\n";
            s += "Cubic Bezier control points\n";
            s += "    X1 = " + animation.control1.x + ", Y1 = " + animation.control1.y + "\n";
            s += "    X2 = " + animation.control2.x + ", Y2 =" + animation.control2.y + "\n";

        }
        outputText.innerText = s;
    }
})();
