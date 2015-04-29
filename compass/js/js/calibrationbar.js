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

    // Constants specifying when the calibration bar is shown:
    //   * suppress duration determines how long (in milliseconds) to wait before 
    //     showing the calibration bar again 
    //   * auto dismiss time determines when to auto dismiss the bar
    //     if it's shown too long
    var SUPPRESS_DURATION_APPROXIMATE_MSEC = 10 * 60 * 1000; // 10 minutes
    var SUPPRESS_DURATION_UNRELIABLE_MSEC = 5 * 60 * 1000; // 5 minutes
    var CALIBRATION_POPUP_AUTO_DIMSMISS_TIME_MSEC = 30 * 1000; // 30 seconds

    var CalibrationBar = WinJS.UI.Pages.define("/html/calibrationBar.html", {
        ready: function (element, options) {
            document.getElementById("showGuidanceButton").addEventListener("click", this.onGuidanceButtonClick.bind(this), false);
            document.getElementById("hideGuidanceButton").addEventListener("click", this.onGuidanceButtonClick.bind(this), false);
            document.getElementById("dismissButton").addEventListener("click", this.onDismissButtonClick.bind(this), false);
            document.getElementById("calibrationBar").style.display = "none";
            this._hideGuidance();
            this._lastCalibrationTime = new Date(0);
            this._barDismissed = false;
        },

        // Called when the application would like to show the calibration bar to the user 
        // to improve sensor accuracy.
        requestCalibration: function (accuracy) {
            // Only show the calibration bar if it is hidden and sufficient
            // time has passed since it was last hidden.
            if (document.getElementById("calibrationBar").style.display === "none" && this._canShowCalibrationBar(accuracy)) {
                this._hideGuidance(); // Calibration image hidden by default
                document.getElementById("calibrationBar").style.display = "block";
                this._calibrationTimerId = window.setTimeout(function () { this.hide(); }.bind(this), CALIBRATION_POPUP_AUTO_DIMSMISS_TIME_MSEC);
            }
        },

        // Called when sensor accuracy is sufficient and the application would like to 
        // hide the calibration bar.
        hide: function () {
            if (document.getElementById("calibrationBar")) {
                if (document.getElementById("calibrationBar").style.display === "block") {
                    this._lastCalibrationTime = new Date();
                    this._hideGuidance();
                    document.getElementById("calibrationBar").style.display = "none";
                    window.clearTimeout(this._calibrationTimerId);
                }
            }
        },

        // This is the click handler for the guidance button.
        onGuidanceButtonClick: function () {
            if (document.getElementById("calibrationImage").style.display === "none") {
                this._showGuidance();
            } else {
                this._hideGuidance();
            }
        },

        // This is the click handler for the dismiss popup button. It hides the 
        // calibration bar and prevents it from being shown again for the lifetime
        // of the app.
        onDismissButtonClick: function () {
            this._barDismissed = true;
            this.hide();
        },

        // Called to determine if sufficient time has passed since the calibration bar
        // was hidden to show the calibration bar again. This suppression time is
        // dependent on the current sensor accuracy.
        _canShowCalibrationBar: function (accuracy) {
            var showBar = false;
            if (!this._barDismissed) {
                if ((accuracy === Windows.Devices.Sensors.MagnetometerAccuracy.approximate) &&
                    (this._lastCalibrationTime.getTime() + SUPPRESS_DURATION_APPROXIMATE_MSEC <= new Date())) {
                    showBar = true;
                } else if ((accuracy === Windows.Devices.Sensors.MagnetometerAccuracy.unreliable) &&
                    (this._lastCalibrationTime.getTime() + SUPPRESS_DURATION_UNRELIABLE_MSEC <= new Date())) {
                    showBar = true;
                }
            }
            return showBar;
        },

        // Expands the calibration bar to show the calibration image to the user.
        _showGuidance: function () {
            document.getElementById("calibrationImage").style.display = "block";
            document.getElementById("hideGuidanceButton").style.display = "block";
            document.getElementById("showGuidanceButton").style.display = "none";
        },

        // Collapses the calibration bar to hide the calibration image from the user.
        _hideGuidance: function () {
            document.getElementById("calibrationImage").style.display = "none";
            document.getElementById("hideGuidanceButton").style.display = "none";
            document.getElementById("showGuidanceButton").style.display = "block";
        }
    });

    WinJS.Namespace.define("CalibrationControls", {
        CalibrationBar: CalibrationBar
    });

    function windowsPhone() {
        return navigator.userAgent.match(/Windows\sPhone/i);
    }

})();
