//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Contact cards JS sample";

    var Contacts = Windows.ApplicationModel.Contacts;
    var Contact = Contacts.Contact;
    var ContactEmail = Contacts.ContactEmail;
    var ContactPhone = Contacts.ContactPhone;

    var scenarios = [
        { url: "/html/scenario1-mini.html", title:  "Show mini contact card" },
        { url: "/html/scenario2-delayMini.html", title: "Show mini contact card with delayed information" },
        { url: "/html/scenario3-full.html", title: "Show full contact card" },
    ];

    function createContactFromUserInput(inputEmailAddress, inputPhoneNumber) {
        var emailAddress = inputEmailAddress.value;
        var phoneNumber = inputPhoneNumber.value;

        if (!emailAddress.length && !phoneNumber.length) {
            WinJS.log && WinJS.log("You must enter an email address and/or phone number.", "sample", "error");
            return null;
        }

        var contact = new Contact();

        // Maximum length for email address is 321, enforced by HTML markup.
        if (emailAddress.length) {
            var email = new ContactEmail();
            email.address = emailAddress;
            contact.emails.append(email);
        }

        // Maximum length for phone number is 50, enforced by HTML markup.
        if (phoneNumber.length) {
            var phone = new ContactPhone();
            phone.number = phoneNumber;
            contact.phones.append(phone);
        }

        return contact;
    }

    function getElementRect(e) {
        var rect = e.getBoundingClientRect();

        // Convert from HTML rect to WinRT rect.
        return { x: rect.left, y: rect.top, width: rect.width, height: rect.height };
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        createContactFromUserInput: createContactFromUserInput,
        getElementRect: getElementRect
    });
})();