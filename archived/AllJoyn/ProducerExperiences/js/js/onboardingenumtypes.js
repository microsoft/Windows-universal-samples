//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {

    "use strict";

    var AuthenticationType = {
        Wpa2Auto: -3,
        WpaAuto: -2,
        Any: -1,
        Open: 0,
        Wep: 1,
        WpaTkip: 2,
        WpaCcmp: 3,
        Wpa2Tkip: 4,
        Wpa2Ccmp: 5,
        Wps: 6,
        properties: {
            0: { name: "Wpa2Auto" },
            1: { name: "WpaAuto" },
            2: { name: "Any" },
            3: { name: "Open" },
            4: { name: "Wep" },
            5: { name: "WpaTkip" },
            6: { name: "WpaCcmp" },
            7: { name: "Wpa2Tkip" },
            8: { name: "Wpa2Ccmp" },
            9: { name: "Wps" },
        }
    };

    // To de-serialize the authentication type enumeration value,   
    // this offset will need to be added to the properties identifier.
    // This is required because the identifiers cannot be negative numbers.
    var authenticationIndexOffset = 3;

    var ConfigureWiFiResultStatus = {
        NotConcurrent: 1,
        Concurrent: 2,
        properties: {
            1: { name: "NotConcurrent" },
            2: { name: "Concurrent" },
        }
    }

    var ConnectionResultCode = {
        Validated: 0,
        Unreachable: 1,
        UnsupportedProtocol: 2,
        Unauthorized: 3,
        ErrorMessage: 4,
        properties: {
            0: { name: "Validated" },
            1: { name: "Unreachable" },
            2: { name: "UnsupportedProtocol" },
            3: { name: "Unauthorized" },
            4: { name: "ErrorMessage" },
        }
    }

    var ConfigurationState = {
        NotConfigured: 0,
        NotValidated: 1,
        Validating: 2,
        Validated: 3,
        Error: 4,
        Retry: 5,
        properties: {
            0: { name: "NotConfigured" },
            1: { name: "NotValidated" },
            2: { name: "Validating" },
            3: { name: "Validated" },
            4: { name: "Error" },
            5: { name: "Retry" },
        }
    }

    WinJS.Namespace.define("OnboardingEnumTypes", {
        AuthenticationType: AuthenticationType,
        ConfigureWiFiResultStatus: ConfigureWiFiResultStatus,
        ConnectionResultCode: ConnectionResultCode,
        ConfigurationState: ConfigurationState,
        authenticationIndexOffset: authenticationIndexOffset
    });
})();