//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario7.html", {
        ready: function (element, options) {
            document.getElementById("ListSmartCards").addEventListener("click", ListSmartCards_Click, false);
        }
    });

    function ListSmartCards_Click() {

        // First, we create a new WinJS.Binding.List.  This list has a
        // dataSource member which allows it to be bound to a
        // WinJS.UI.ListView control so that items in the list are
        // shown on the UI.  We create a new list for each function call
        // as there is no efficient way to clear a WinJS.Binding.List, and
        // hence no point in reusing one.
        var dataList = new WinJS.Binding.List();

        WinJS.log("Enumerating smart cards...", "sample", "status");

        var button = document.getElementById("ListSmartCards");
        button.disabled = true;

        // First we get the device selector for smart card readers using
        // the static getDeviceSelector method of the SmartCardReader
        // class.  The selector is a string which describes a class of
        // devices to query for, and is used as the argument to
        // DeviceInformation.FindAllAsync.  getDeviceSelector can be called
        // with no argument to find all smart cards, or you can provide a
        // Windows.Devices.SmartCards.SmartCardReaderKind if you only want
        // to find a specific type of card (e.g. TPM virtual smart card).
        // In this case we will list all cards and readers.
        var selector =
            Windows.Devices.SmartCards.SmartCardReader.getDeviceSelector();
        Windows.Devices.Enumeration.DeviceInformation.findAllAsync(selector, null)
            .then(
        function (devices) {
            // DeviceInformation.findAllAsync returns a collection of
            // DeviceInformation objects, each of which has an ID that
            // can be used to instantiate a new smart card reader.
            // For each DeviceInformation we receive, we need to
            // instantiate a SmartCardReader object, but to do so
            // requires an async call, and we want to return all of the
            // readers to our next callback, so we cannot use a simple
            // for loop.  The solution is to use map to create a list
            // of SmartCardReader promises, and then use WinJS.Promise.join
            // to wait for all the promises to complete before passing the
            // list of readers to the next callback.
            return WinJS.Promise.join(devices.map(
            function (device) {
                return Windows.Devices.SmartCards.SmartCardReader.fromIdAsync(device.id);
            }));
        }).then(
        function (readers) {
            // We now have a list of SmartCardReaders.  However, we need to
            // find all the cards in each reader, so again we use
            // map and WinJS.Promise.join to return a list of all cards for
            // all readers to the next callback.
            return WinJS.Promise.join(readers.map(
            function (reader) {
                return reader.findAllCardsAsync();
            }));
        }).then(
        function (cardEnumerationList) {
            // Here our task is further complicated by the fact that we are
            // now working with a list of lists (i.e. one list of cards for
            // each reader).  We need SmartCardProvisioning objects to get
            // the card names, but again we must compose all the provisioning
            // objects into a list of Promises that can be waited on  using
            // WinJS.Promise.join so that we can return a list of
            // SmartCardProvisioning objects to our next callback.  There are
            // a few ways that this could be done, but the simplest is just to
            // create an empty list and then, in a nested for loop, add
            // promises for SmartCardProvisioning objects to the list.
            // Then we can call WinJS.Promise.join on the list.
            var provisioningPromises = [];

            for (var cardListIndex = 0; cardListIndex < cardEnumerationList.length; ++cardListIndex) {
                var cardList = cardEnumerationList[cardListIndex];
                for (var cardIndex = 0; cardIndex < cardList.length; ++cardIndex) {
                    provisioningPromises.push(Windows.Devices.SmartCards.SmartCardProvisioning.fromSmartCardAsync(cardList[cardIndex]));
                }
            }

            return WinJS.Promise.join(provisioningPromises);
        }).then(
        function (provisioningList) {
            // Finally, we have a list of SmartCardProvisioning objects, from
            // which we can get both card and reader names.  We again use
            // map to call a function on each item in the list, and
            // WinJS.Promise.join to for completion of each promise.
            return WinJS.Promise.join(provisioningList.map(
            function (provisioning) {
                return provisioning.getNameAsync().done(
                function (cardName) {
                    dataList.push({title: provisioning.smartCard.reader.name, text: cardName});
                });
            }));
        }).done(
        function () {
            // Our list of card data objects has now been generated, so we
            // need to bind the data source of our list to the corresponding
            // UI control.
            var listView = document.getElementById("ItemListView").winControl;
            listView.itemDataSource = dataList.dataSource;

            WinJS.log("Enumerating smart cards completed.", "sample", "status");

            button.disabled = false;
        },
        function (error) {
            WinJS.log && WinJS.log("Enumerating smart cards failed with exception: " + error.toString(), "sample", "error");
            button.disabled = false;
        });
    }
})();
