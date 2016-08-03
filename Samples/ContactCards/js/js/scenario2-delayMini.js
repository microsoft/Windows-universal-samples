//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Contacts = Windows.ApplicationModel.Contacts;
    var ContactManager = Contacts.ContactManager;
    var Contact = Contacts.Contact;
    var ContactAddress = Contacts.ContactAddress;
    var ContactAddressKind = Contacts.ContactAddressKind;
    var ContactEmail = Contacts.ContactEmail;
    var ContactEmailKind = Contacts.ContactEmailKind;
    var ContactPhone = Contacts.ContactPhone;
    var ContactPhoneKind = Contacts.ContactPhoneKind;
    var ContactCardOptions = Contacts.ContactCardOptions;
    var ContactCardHeaderKind = Contacts.ContactCardHeaderKind;
    var Placement = Windows.UI.Popups.Placement;

    var downloadSucceeded;

    var page = WinJS.UI.Pages.define("/html/scenario2-delayMini.html", {
        ready: function (element, options) {
            document.getElementById("showContactCard").addEventListener("click", onShowContactCard);

            downloadSucceeded = document.getElementById("downloadSucceeded");

            if (!ContactManager.isShowDelayLoadedContactCardSupported())
            {
                document.getElementById("notSupportedWarning").style.display = "block";
            }
        }
    });

    function createPlaceholderContact() {
        // Create contact object with small set of initial data to display.
        var contact = new Contact();
        contact.firstName = "Kim";
        contact.lastName = "Abercrombie";

        var email = new ContactEmail();
        email.address = "kim@contoso.com";
        contact.emails.append(email);

        return contact;
    }

    function downloadContactDataAsync(contact) {
        // Simulate the download latency by delaying the execution by 2 seconds.
        return WinJS.Promise.timeout(2000).then(function () {
            if (!downloadSucceeded.checked) {
                return null;
            }

            // Add more data to the contact object.
            var workEmail = new ContactEmail();
            workEmail.address = "kim@adatum.com";
            workEmail.kind = ContactEmailKind.work;
            contact.emails.append(workEmail);                   

            var homePhone = new ContactPhone();
            homePhone.number = "(444) 555-0001";
            homePhone.kind = ContactPhoneKind.home;
            contact.phones.append(homePhone);

            var workPhone = new ContactPhone();
            workPhone.number = "(245) 555-0123";
            workPhone.kind = ContactPhoneKind.work;
            contact.phones.append(workPhone);

            var mobilePhone = new ContactPhone();
            mobilePhone.number = "(921) 555-0187";
            mobilePhone.kind = ContactPhoneKind.mobile;
            contact.phones.append(mobilePhone);

            var address = new ContactAddress();
            address.streetAddress = "123 Main St";
            address.locality = "Redmond";
            address.region = "WA";
            address.country = "USA";
            address.postalCode = "23456";
            address.kind = ContactAddressKind.home;
            contact.addresses.append(address);

            return contact;
        });
    }

    function onShowContactCard(evt) {
        var contact = createPlaceholderContact();

        // Show the contact card next to the button.
        var rect = SdkSample.getElementRect(evt.srcElement);

        // The contact card placement can change when it is updated with more data. For improved user experience, specify placement 
        // of the card so that it has space to grow and will not need to be repositioned. In this case, default placement first places 
        // the card above the button because the card is small, but after the card is updated with more data, the operating system moves 
        // the card below the button to fit the card's expanded size. Specifying that the contact card is placed below at the beginning 
        // avoids this repositioning.
        var placement = Placement.below;

        // For demonstration purposes, we ask for the Enterprise contact card.
        var options = new ContactCardOptions();
        options.headerKind = ContactCardHeaderKind.enterprise;

        var dataLoader = ContactManager.showDelayLoadedContactCard(contact, rect, placement, options);

        if (dataLoader) {
            // Simulate downloading more data from the network for the contact.
            WinJS.log && WinJS.log("Simulating download...", "sample", "status");

            downloadContactDataAsync(contact).then(function (fullContact) {
                if (fullContact) {
                    // Update the contact card with the full set of contact data.
                    dataLoader.setData(fullContact);
                    WinJS.log && WinJS.log("Contact has been updated with downloaded data.", "sample", "status");
                } else {
                    WinJS.log && WinJS.log("No further information available.", "sample", "status");
                }

                // Close the object to indicate that the delay-loading operation has completed.
                dataLoader.close();
            });
        } else {
            WinJS.log && WinJS.log("ShowDelayLoadedContactCard is not supported by this device.", "sample", "error");
        }
    }

})();
