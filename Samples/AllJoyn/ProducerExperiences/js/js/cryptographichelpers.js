//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {

    "use strict";

    var cryptoBuffer = Windows.Security.Cryptography.CryptographicBuffer;

    var generateHighEntropyKey = function (desiredKeyLength) {
        if (desiredKeyLength <= 0) {
            throw ("Argument Exception: The desired key length must be a positive integer.");
        } else {
            var maxAttempts = 100;
            var attemptCount = 0;
            var randomBuffer;
            var str;
            var generatedKey = new String();

            // Base64 uses 6 bits to encode a single character. Number of bits required = length * 6.
            var numberOfRandomBytesRequired = Math.round((desiredKeyLength * 6.0) / 8.0);

            while ((generatedKey.length !== desiredKeyLength) && (attemptCount < maxAttempts)) {
                randomBuffer = cryptoBuffer.generateRandom(numberOfRandomBytesRequired);
                str = cryptoBuffer.encodeToBase64String(randomBuffer);

                for (var i = 0; i < str.length; i++) {
                    var c = str.charAt(i);
                    if (/^[a-zA-Z0-9]+$/i.test(c)) {
                        generatedKey += c;
                        if (generatedKey.length === desiredKeyLength) {
                            break;
                        }
                    }
                }
                attemptCount++;
            }

            if (generatedKey.length === desiredKeyLength) {
                return generatedKey;
            } else {
                throw ("Exception: Unable to generate a high entropy key.");
            }
        }
    }

    var insertHyphens = function (key, insertHyphenAfterPosition) {
        if (!key || 0 === key.length) {
            throw ("Argument Exception: Input key must be a valid string.");
        } else if (insertHyphenAfterPosition <= 0) {
            throw ("Argument Exception: Desired hyphen position must be a positive number.");
        } else if (insertHyphenAfterPosition > key.length) {
            throw ("Argument Exception: Desired hyphen position cannot be greater than key length.");
        } else {
            var count = 0;
            var resultString = new String();

            for (var i = 0; i < key.length; i++) {
                if (count === insertHyphenAfterPosition) {
                    resultString += "-";
                    count = 0;
                }

                resultString += key.charAt(i);
                count++;
            }

            return resultString;
        }
    }

    WinJS.Namespace.define("CryptographicHelpers", {
        generateHighEntropyKey: generateHighEntropyKey,
        insertHyphens: insertHyphens
    });
})();