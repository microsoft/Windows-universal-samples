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

var voiceCommandManager = Windows.ApplicationModel.VoiceCommands.VoiceCommandDefinitionManager;

(function () {
    "use strict";

    var trip;
    var update;

    WinJS.Namespace.define("SdkSample.DateConverter", {
        dateConverter: WinJS.Binding.converter(function (type) {

            if (new Date(type) === "Invalid Date" || isNaN(new Date(type))) {
                return "";
            }

            var dateObj = new Date(type);
            return dateObj.toDateString();
        })
    })

    WinJS.UI.Pages.define("/html/tripDetails.html", {
        ready: function (element, options) {
            /// <summary> Displays a trip's details. If no trip is provided, allows creation of a new trip.
            /// </summary>
            if (options != null) {

                update = true;
                if (options.trip) {
                    trip = options.trip;
                } else {
                    trip = options;
                }
                // show the delete button only for existing trips.
                deleteButton.style.visibility = "visible";
            } else {
                update = false;
                trip = {
                    destination: "",
                    description: "",
                    startDate: "",
                    endDate: "",
                    notes: ""
                };
            }

            var b = WinJS.Binding.as(trip);

            this.bindTextBox("#startDate", b.startDate,
                function (value) {
                    if (new Date(value) === "Invalid Date" || isNaN(new Date(value))) {
                        startDate.innerText = "";
                        return;
                    }
                    b.startDate = new Date(value).toJSON();
                });

            this.bindTextBox("#endDate", b.endDate,
                function (value) {
                    if (new Date(value) === "Invalid Date" || isNaN(new Date(value))) {
                        endDate.innerText = "";
                        return;
                    }
                    b.endDate = new Date(value).toJSON();
                });

            this.bindTextBox("#destination", b.destination,
                function (value) { b.destination = value; });

            this.bindTextBox("#description", b.description,
                function (value) { b.description = value; });

            this.bindTextBox("#notes", b.notes,
                function (value) { b.notes = value; });

            WinJS.Binding.processAll(element.querySelector("#tripDetailsSection"), trip);

            saveButton.addEventListener("click", this.saveButtonHandler.bind(this), false);
            deleteButton.addEventListener("click", this.deleteButtonHandler.bind(this), false);

        },

        bindTextBox: function (selector, initialValue, setterCallback) {
            var textBox = this.element.querySelector(selector);
            textBox.addEventListener("change", function (evt) {
                setterCallback(evt.target.value);
            }, false);
            textBox.value = initialValue;
        },

        saveButtonHandler: function (evt) {
            /// <summary> Handle saving a trip, or adding a new one. </summary>
            if (trip.destination == "") {
                requireDestinationValidation.innerText = "Destination is required";
                requireDestinationValidation.style.visibility = "visible";
                return;
            }

            if (SdkSample.DataStore.TripStore.Trips.indexOf(trip) != -1) {
                SdkSample.DataStore.TripStore.saveTrip(trip);
            } else {
                SdkSample.DataStore.TripStore.addTrip(trip);
            }

            this.updatePhraseList();

            setImmediate(function () {
                WinJS.Navigation.navigate("/html/tripListView.html", null);
            })
        },

        deleteButtonHandler: function () {
            /// <summary> delete an existing trip. Otherwise, do nothing (although this button should be hidden
            /// if creating a new trip</summary>
            if (update) {
                SdkSample.DataStore.TripStore.deleteTrip(trip);

                this.updatePhraseList();

                setImmediate(function () {
                    WinJS.Navigation.navigate("/html/tripListView.html", null);
                })
            }
        },

        updatePhraseList: function () {
            /// <summary> Once a trip is added, update the phrase list in Cortana. This may also be implemented
            /// in a background task, to handle data coming in from a web service, etc. </summary>

            // check to make sure the VCD has been installed.
            var language = window.navigator.userLanguage || window.navigator.language;

            var commandSetName = "AdventureWorksCommandSet_" + language.toLowerCase();
            if (voiceCommandManager.installedCommandDefinitions.hasKey(commandSetName)) {
                var vcd = voiceCommandManager.installedCommandDefinitions.lookup(commandSetName);
                var phraseList = [];
                SdkSample.DataStore.TripStore.Trips.forEach(function (trip) {
                    phraseList.push(trip.destination);
                });
                // 'destination' matches up with the name of the phrase list in AdventureWorksCommands.xml
                vcd.setPhraseListAsync("destination", phraseList).done();
            }
        }


    });
})();
