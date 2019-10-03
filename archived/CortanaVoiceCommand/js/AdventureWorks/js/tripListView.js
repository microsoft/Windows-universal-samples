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
    var tripList;
    
    WinJS.UI.Pages.define("/html/tripListView.html", {
        ready: function (element, options) {
            /// <summary> Initialize trip list view.</summary>
            tripList = element.querySelector("#tripListBox").winControl;
            tripList.addEventListener("selectionchanged", this.tripSelected);
            
            var addTripButton = document.getElementById("addTripButton");
            addTripButton.addEventListener("click", this.addTrip, false);

            tripList.itemDataSource = SdkSample.DataStore.TripStore.Trips.dataSource;
        },

        tripSelected: function (eventInfo) {
            /// <summary>Handle click event on individual trips in the list, navigate to a details page</summary>
            tripList.selection.getItems().then(function (result) {
                setImmediate(function () {
                    WinJS.Navigation.navigate("/html/tripDetails.html", result[0].data);
                });
                
            }).done();
            
        },

        addTrip: function (eventInfo) {
            /// <summary>Navigate to the trip details page, without supplying an existing trip</summary>
            WinJS.Navigation.navigate("/html/tripDetails.html", null);
        }
    });
})();
