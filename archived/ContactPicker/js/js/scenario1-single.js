//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Contacts = Windows.ApplicationModel.Contacts;
    var ContactPicker = Contacts.ContactPicker;
    var ContactFieldType = Contacts.ContactFieldType;

    var page = WinJS.UI.Pages.define("/html/scenario1-single.html", {
        ready: function (element, options) {
            document.getElementById("pickContactEmail").addEventListener("click", pickContactEmail);
            document.getElementById("pickContactPhone").addEventListener("click", pickContactPhone);
        }
    });

    function clearOutputs() {
        // Reset the output fields.
        outputText.innerText = "";
        WinJS.log && WinJS.log("", "samples", "status");
    }

    function reportContactResult(contact) {
        if (contact) {
            outputText.innerText = SdkSample.getContactResult(contact);
        } else {
            WinJS.log && WinJS.log("No contact was selected.", "samples", "error");
        }
    }

    function pickContactEmail() {
        clearOutputs();

        // Ask the user to pick a contact email address.
        var contactPicker = new ContactPicker();
        contactPicker.desiredFieldsWithContactFieldType.append(ContactFieldType.email);

        contactPicker.pickContactAsync().then(function (contact) {
            reportContactResult(contact);
        });
    }

    function pickContactPhone() {
        clearOutputs();

        // Ask the user to pick a contact phone number..
        var contactPicker = new ContactPicker();
        contactPicker.desiredFieldsWithContactFieldType.append(ContactFieldType.phoneNumber);

        contactPicker.pickContactAsync().then(function (contact) {
            reportContactResult(contact);
        });
    }
})();
