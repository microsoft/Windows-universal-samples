//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Contacts = Windows.ApplicationModel.Contacts;
    var ContactManager = Contacts.ContactManager;
    var FullContactCardOptions = Contacts.FullContactCardOptions;
    var ViewSizePreference = Windows.UI.ViewManagement.ViewSizePreference;

    var page = WinJS.UI.Pages.define("/html/scenario3-full.html", {
        ready: function (element, options) {
            document.getElementById("showContactCard").addEventListener("click", onShowContactCard);
        }
    });

    function onShowContactCard(evt) {
        var contact = SdkSample.createContactFromUserInput(inputEmailAddress, inputPhoneNumber);
        if (contact) {
            // Try to share the screen half/half with the full contact card.
            var options = new FullContactCardOptions();
            options.desiredRemainingView = ViewSizePreference.useHalf;

            // Show the full contact card.
            ContactManager.showFullContactCard(contact, options);
        }
    }
})();
