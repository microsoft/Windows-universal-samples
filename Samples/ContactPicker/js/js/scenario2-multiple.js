//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Contacts = Windows.ApplicationModel.Contacts;
    var ContactPicker = Contacts.ContactPicker;
    var ContactFieldType = Contacts.ContactFieldType;

    var page = WinJS.UI.Pages.define("/html/scenario2-multiple.html", {
        ready: function (element, options) {
            document.getElementById("pickContactsEmail").addEventListener("click", pickContactsEmail);
            document.getElementById("pickContactsPhone").addEventListener("click", pickContactsPhone);
        }
    });

    function clearOutputs() {
        // Reset the output fields.
        outputText.innerText = "";
        WinJS.log && WinJS.log("", "samples", "status");
    }

    function reportContactResults(contacts) {
        if (contacts && contacts.length > 0) {
            outputText.innerText = contacts.map(function(contact) {
                return SdkSample.getContactResult(contact);
            }).join("\n");
        } else {
            WinJS.log && WinJS.log("No contacts were selected.", "samples", "error");
        }
    }

    function pickContactsEmail() {
        clearOutputs();

        // Ask the user to pick contact email addresses.
        var contactPicker = new ContactPicker();
        contactPicker.desiredFieldsWithContactFieldType.append(ContactFieldType.email);

        contactPicker.pickContactsAsync().then(function (contacts) {
            reportContactResults(contacts);
        });
    }

    function pickContactsPhone() {
        clearOutputs();

        // Ask the user to pick contact phone numbers.
        var contactPicker = new ContactPicker();
        contactPicker.desiredFieldsWithContactFieldType.append(ContactFieldType.phoneNumber);

        contactPicker.pickContactsAsync().then(function (contacts) {
            reportContactResults(contacts);
        });
    }

})();
