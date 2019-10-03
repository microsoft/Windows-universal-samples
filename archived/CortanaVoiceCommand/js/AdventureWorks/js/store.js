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

var appData = Windows.Storage.ApplicationData;

var Store = WinJS.Class.define(
    function () { "use strict"; },
    {
        Trips: null,

        loadTrips: function () {
            /// <summary>Looks for, and loads a json blob from local state storage if it exists. If it does not exist, 
            /// pre-initialize some sample data instead. Stores the data in the store's Trips list, as well as returning them. </summary>
            /// <returns>The trips loaded/pre-initialized.</returns>
            "use strict";
            var that = this;
            if (that.Trips != null) {
                return new WinJS.Promise(function (completed, error) {
                    completed(that.Trips);
                });
            }

            return appData.current.localFolder.tryGetItemAsync("trips.json").then(
                function (file) {
                    if (file == null) {
                        // pre-initialized data, matching the initial VCD configuration
                        that.Trips = new WinJS.Binding.List([
                          { destination: "London", description: "Bring an umbrella", startDate: (new Date(2015, 5, 10)).toJSON(), endDate: (new Date(2015, 5, 20)).toJSON(), notes: "" },
                          { destination: "Melbourne", description: "Bring sunscreen!", startDate: (new Date(2015, 10, 15)).toJSON(), endDate: (new Date(2015, 10, 25)).toJSON(), notes: "" },
                          { destination: "Yosemite National Park", description: "Trip to Yosemite", startDate: (new Date(2016, 8, 12)).toJSON(), endDate: (new Date(2016, 8, 22)).toJSON(), notes: "" }
                        ]);

                        /// write out the initial config to a file.
                        appData.current.localFolder.createFileAsync("trips.json", Windows.Storage.CreationCollisionOption.replaceExisting)
                            .then(function (outputFile) {
                                var outText = JSON.stringify(that.Trips.slice(0));
                                Windows.Storage.FileIO.writeTextAsync(outputFile, outText).done();
                            }).done();

                        return that.Trips;
                    } else {
                        return Windows.Storage.FileIO.readTextAsync(file).then(function (text) {
                            var jsonBlob = JSON.parse(text);
                            that.Trips = new WinJS.Binding.List(jsonBlob);
                            return that.Trips;
                        });
                    }
                });
        },
        addTrip: function (trip) {
            /// <summary>Add a trip to the store, and save the json blob out to local state.</summary>
            /// <param name="trip">The trip object to add</param>
            "use strict";
            var that = this;
            if (that.Trips.indexOf(trip) == -1) {
                that.Trips.push(trip);

                appData.current.localFolder.createFileAsync("trips.json", Windows.Storage.CreationCollisionOption.replaceExisting)
                .then(function (outputFile) {
                    var outText = JSON.stringify(that.Trips.slice(0));
                    Windows.Storage.FileIO.writeTextAsync(outputFile, outText).done();
                }).done();
            }
        },
        saveTrip: function (trip) {
            /// <summary>Update an existing trip in the store, and save the json blob out to local state.</summary>
            /// <param name="trip">The trip object to save</param>
            "use strict";
            var that = this;
            appData.current.localFolder.createFileAsync("trips.json", Windows.Storage.CreationCollisionOption.replaceExisting)
                .then(function (outputFile) {
                    var outText = JSON.stringify(that.Trips.slice(0));
                    Windows.Storage.FileIO.writeTextAsync(outputFile, outText).done();
                }).done();
        },
        deleteTrip: function (trip) {
            /// <summary>Remove a trip from the store, and save the json blob out to local state.</summary>
            /// <param name="trip">The trip object to remove</param>
            "use strict";
            var that = this;

            var i = that.Trips.indexOf(trip);
            if (i != -1) {
                that.Trips.splice(i, 1);
                appData.current.localFolder.createFileAsync("trips.json", Windows.Storage.CreationCollisionOption.replaceExisting)
                    .then(function (outputFile) {
                        var outText = JSON.stringify(that.Trips.slice(0));
                        Windows.Storage.FileIO.writeTextAsync(outputFile, outText).done();
                    }).done();
            }
        }
    },
    {})