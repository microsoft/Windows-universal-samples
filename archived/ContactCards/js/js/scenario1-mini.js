//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Contacts = Windows.ApplicationModel.Contacts;
    var ContactManager = Contacts.ContactManager;
    var ContactCardOptions = Contacts.ContactCardOptions;
    var ContactCardTabKind = Contacts.ContactCardTabKind;
    var Placement = Windows.UI.Popups.Placement;

    var inputEmailAddress;
    var inputPhoneNumber;

    var page = WinJS.UI.Pages.define("/html/scenario1-mini.html", {
        ready: function (element, options) {
            document.getElementById("showContactCard").addEventListener("click", onShowContactCard);
            document.getElementById("showContactCardWithPlacement").addEventListener("click", onShowContactCardWithPlacement);
            document.getElementById("showContactCardWithOptions").addEventListener("click", onShowContactCardWithOptions);

            inputEmailAddress = document.getElementById("inputEmailAddress");
            inputPhoneNumber = document.getElementById("inputPhoneNumber");

            if (!ContactManager.isShowContactCardSupported())
            {
                document.getElementById("notSupportedWarning").style.display = "block";
            }
        }
    });

    function onShowContactCard(evt) {
        var contact = SdkSample.createContactFromUserInput(inputEmailAddress, inputPhoneNumber);
        if (contact) {
            // Show the contact card next to the button.
            var rect = SdkSample.getElementRect(evt.srcElement);

            // Show with default placement.
            ContactManager.showContactCard(contact, rect);
        }
    }

    function onShowContactCardWithPlacement(evt) {
        var contact = SdkSample.createContactFromUserInput(inputEmailAddress, inputPhoneNumber);
        if (contact) {
            // Show the contact card next to the button.
            var rect = SdkSample.getElementRect(evt.srcElement);

            // Show with preferred placement to the right.
            ContactManager.showContactCard(contact, rect, Placement.right);
        }
    }

    function onShowContactCardWithOptions(evt) {
        var contact = SdkSample.createContactFromUserInput(inputEmailAddress, inputPhoneNumber);
        if (contact) {
            // Show the contact card next to the button.
            var rect = SdkSample.getElementRect(evt.srcElement);

            // Ask for the initial tab to be Phone.
            var options = new ContactCardOptions();
            options.initialTabKind = ContactCardTabKind.phone;

            // Show with preferred placement to the right.
            ContactManager.showContactCard(contact, rect, Placement.right, options);
        }
    }
})();
