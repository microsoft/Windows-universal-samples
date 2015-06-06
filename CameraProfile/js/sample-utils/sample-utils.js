//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Sample infrastructure internals
    var currentScenarioUrl = null;

    WinJS.Navigation.addEventListener("navigating", function (evt) {
        currentScenarioUrl = evt.detail.location;
    });

    var lastError, lastStatus;
    WinJS.log = function (message, tag, type) {
        var isError = (type === "error");
        var isStatus = (type === "status");

        if (isError || isStatus) {
            var statusDiv = /* @type(HTMLElement) */ document.getElementById("statusMessage");
            if (statusDiv) {
                statusDiv.innerText = message;
                if (statusDiv.innerText.length > 0) {
                    if (isError) {
                        lastError = message;
                        statusDiv.style.backgroundColor = "red";
                    } else if (isStatus) {
                        lastStatus = message;
                        statusDiv.style.backgroundColor = "green";
                    }
                } else {
                    statusDiv.style.backgroundColor = "";
                }
            }
        }
    };

    var header = WinJS.UI.Pages.define("/sample-utils/header.html", {
        processed: function (element, options) {
            return WinJS.Binding.processAll(element);
        }
    });

    // Control that populates and runs the scenario selector
    var ScenarioSelect = WinJS.UI.Pages.define("/sample-utils/scenario-select.html", {
        ready: function (element, options) {
            var that = this;

            element.addEventListener("selectionchanging", function (evt) {
                if (evt.detail.newSelection.count() === 0) {
                    evt.preventDefault();
                }
            });
            element.addEventListener("iteminvoked", function (evt) {
                evt.detail.itemPromise.then(function (item) {
                    that._selectedIndex = item.index;
                    var newUrl = item.data.url;
                    if (currentScenarioUrl !== newUrl) {
                        WinJS.Navigation.navigate(newUrl);
                        var splitView = document.querySelector("#root.win-splitview-openeddisplayoverlay");
                        splitView && splitView.winControl.closePane();
                    }
                });
            });
            element.addEventListener("keyboardnavigating", function (evt) {
                var listview = evt.target.winControl;
                listview.elementFromIndex(evt.detail.newFocus).click();
            });

            this._selectedIndex = 0;

            var lastUrl = WinJS.Application.sessionState.lastUrl;
            SdkSample.scenarios.forEach(function (s, index) {
                s.scenarioNumber = index + 1;
                if (s.url === lastUrl && index !== that._selectedIndex) {
                    that._selectedIndex = index;
                }
            });

            this._listview = element.querySelector(".win-listview").winControl;
            this._listview.selection.set([this._selectedIndex]);
            this._listview.currentItem = { index: this._selectedIndex, hasFocus: true };
        }
    });

    // SDK Sample Test helper
    document.TestSdkSample = {
        getLastError: function () {
            return lastError;
        },

        getLastStatus: function () {
            return lastStatus;
        },

        selectScenario: function (scenarioID) {
            scenarioID = scenarioID >> 0;
            var scenarioIndex = scenarioID - 1;
            var scenarioControl = document.querySelector("#scenarioControl").winControl;
            scenarioControl.elementFromIndex(scenarioIndex).click();
        }
    };
})();