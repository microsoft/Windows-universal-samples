//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Contact picker JS sample";

    var scenarios = [
        { url: "/html/scenario1-single.html", title: "Pick a contact" },
        { url: "/html/scenario2-multiple.html", title: "Pick multiple contacts" },
    ];

    var contactEmailKinds = [ "Personal", "Work", "Other" ];
    var contactPhoneKinds = [ "Home", "Mobile", "Work", "Other", "Pager", "BusinessFax", "HomeFax", "Company", "Assistant", "Radio" ];

    function getContactResult(contact) {
        var result = "Display name: " + contact.displayName + "\n";

        contact.emails.forEach(function (email) {
            result += "Email address: (" + contactEmailKinds[email.kind] + "): " + email.address + "\n";
        });

        contact.phones.forEach(function (phone) {
            result += "Phone: (" + contactPhoneKinds[phone.kind] + "): " + phone.number + "\n";
        });

        return result;
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        getContactResult: getContactResult
    });
})();