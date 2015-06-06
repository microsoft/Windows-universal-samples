//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Smartcard";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Create and Provision a TPM Virtual Smart Card" },
        { url: "/html/scenario2.html", title: "Change Smart Card PIN" },
        { url: "/html/scenario3.html", title: "Reset Smart Card PIN" },
        { url: "/html/scenario4.html", title: "Change Smart Card Admin Key" },
        { url: "/html/scenario5.html", title: "Verify Response" },
        { url: "/html/scenario6.html", title: "Delete TPM Virtual Smart Card" },
        { url: "/html/scenario7.html", title: "List All Smart Cards" },
        { url: "/html/scenario8.html", title: "Transmit APDU to Smart Card" }
    ];

    // Calculates the response by encrypting the challenge with the admin key,
    // using the Triple DES (3DES) algorithm. When the card attempts
    // authentication, it compares the response to the expected response.
    // If the resulting values are the same, the authentication is successful.
    function calculateChallengeResponse(challenge, adminkey) {
        var objAlg =
            Windows.Security.Cryptography.Core.SymmetricKeyAlgorithmProvider
            .openAlgorithm(
                Windows.Security.Cryptography.Core.SymmetricAlgorithmNames
                .tripleDesCbc);

        var symetricKey = objAlg.createSymmetricKey(adminkey);
        var buffEncrypted =
            Windows.Security.Cryptography.Core.CryptographicEngine.encrypt(
            symetricKey,
            challenge,
            null);

        return buffEncrypted;
    };

    function getSmartCard() {
        return SdkSample.reader.findAllCardsAsync().then(
        function (smartCardList) {
            return smartCardList.getAt(0);
        });
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        adminKey: null,
        reader: null,
        calculateChallengeResponse: calculateChallengeResponse,
        getSmartCard: getSmartCard,
        ADMIN_KEY_LENGTH_IN_BYTES: 24
    });
})();