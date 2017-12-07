//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Namespaces and type names
    var CertificateStores = Windows.Security.Cryptography.Certificates.CertificateStores;
    var StandardCertificateStoreNames = Windows.Security.Cryptography.Certificates.StandardCertificateStoreNames;
    var CryptographicBuffer = Windows.Security.Cryptography.CryptographicBuffer;

    // Well-known certificate stores.
    var appStore = CertificateStores.getStoreByName(StandardCertificateStoreNames.personal);
    var userStore = CertificateStores.getUserStoreByName(StandardCertificateStoreNames.personal);

    // Table of certificates. We use this to convert option.value back to a certificate.
    var knownCertificates = [];

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {

        ready: function (element, options) {
            displayCerts();

            document.getElementById("moveUpButton").addEventListener("click", moveUserCertificateToApp, false);
            document.getElementById("moveDownButton").addEventListener("click", moveAppCertificateToUser, false);
        }
    });

    function displayCerts() {
        CertificateStores.findAllAsync().done(function (certs) {
            certs.forEach(function (cert) {
                var option = document.createElement("option");
                var serialNumber = CryptographicBuffer.encodeToHexString(CryptographicBuffer.createFromByteArray(cert.serialNumber));
                option.text = "Subject: " + cert.subject + ", Serial Number: " + serialNumber;
                option.value = knownCertificates.length;
                knownCertificates.push(cert);
                if (cert.isPerUser) {
                    userCertificateListView.add(option);
                } else {
                    appCertificateListView.add(option);
                }
            });
        });
    }

    function moveUserCertificateToApp() {
        var index = userCertificateListView.selectedIndex;
        if (index >= 0) {
            var option = userCertificateListView.options[index];
            var cert = knownCertificates[option.value];

            appStore.add(cert);

            userStore.requestDeleteAsync(cert).then(function (succeeded) {
                if (succeeded) {
                    userCertificateListView.removeChild(option);
                    appCertificateListView.add(option);
                } else {
                    appStore.delete(cert);
                }
            });
        }
    }
     
    function moveAppCertificateToUser() {
        var index = appCertificateListView.selectedIndex;
        if (index >= 0) {
            var option = appCertificateListView.options[index];
            var cert = knownCertificates[option.value];

            userStore.requestAddAsync(cert).then(function (succeeded) {
                if (succeeded) {
                    appStore.delete(cert);
                    appCertificateListView.remove(option);

                    userCertificateListView.add(option);
                }
            });
        }
    }
})();
