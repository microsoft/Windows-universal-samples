//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {

    "use strict";

    var localSettings = Windows.Storage.ApplicationData.current.localSettings;
    var onboarding = org.alljoyn.Onboarding;

    var isUpperCaseEnabledPropertyKey = "IsUpperCaseEnabled";
    var onboardingConfigureSsidKey = "OnboardingConfigureSsid";
    var onboardingConfigurePassphraseKey = "OnboardingConfigurePassphrase";
    var onboardingConfigureAuthenticationTypeKey = "OnboardingConfigureAuthenticationType";
    var onboardingConfigurationStateKey = "OnboardingConfigurationState";
    var onboardingLastErrorCodeKey = "OnboardingLastErrorCode";
    var onboardingLastErrorMessageKey = "OnboardingLastErrorMessage";

    var secureInterfaceProperties = {};

    Object.defineProperty(secureInterfaceProperties, 'isUpperCaseEnabled', {
        get: function () {
            if (!localSettings.values.hasKey(isUpperCaseEnabledPropertyKey)) {
                localSettings.values[isUpperCaseEnabledPropertyKey] = false;
            }
            return localSettings.values[isUpperCaseEnabledPropertyKey];
        },
        set: function (value) {
            localSettings.values[isUpperCaseEnabledPropertyKey] = value;
        }
    });

    var onboardingConfiguration = {};

    Object.defineProperty(onboardingConfiguration, 'ssid', {
        get: function () {
            if (!localSettings.values.hasKey(onboardingConfigureSsidKey)) {
                return null;
            } else {
                return localSettings.values[onboardingConfigureSsidKey];
            }
        },
        set: function (value) {
            localSettings.values[onboardingConfigureSsidKey] = value;
        }
    });

    Object.defineProperty(onboardingConfiguration, 'passphrase', {
        get: function () {
            if (!localSettings.values.hasKey(onboardingConfigurePassphraseKey)) {
                return null;
            } else {
                return localSettings.values[onboardingConfigurePassphraseKey];
            }
        },
        set: function (value) {
            localSettings.values[onboardingConfigurePassphraseKey] = value;
        }
    });

    Object.defineProperty(onboardingConfiguration, 'authenticationType', {
        get: function () {
            if (!localSettings.values.hasKey(onboardingConfigureAuthenticationTypeKey)) {
                return OnboardingEnumTypes.AuthenticationType.Any;
            } else {
                return localSettings.values[onboardingConfigureAuthenticationTypeKey];
            }
        },
        set: function (value) {
            localSettings.values[onboardingConfigureAuthenticationTypeKey] = value;
        }
    });

    var onboardingProperties = {};

    Object.defineProperty(onboardingProperties, 'configurationState', {
        get: function () {
            if (!localSettings.values.hasKey(onboardingConfigurationStateKey)) {
                localSettings.values[onboardingConfigurationStateKey] = OnboardingEnumTypes.ConfigurationState.NotConfigured;
            }
            return localSettings.values[onboardingConfigurationStateKey];
        },
        set: function (value) {
            localSettings.values[onboardingConfigurationStateKey] = value;
        }
    });

    Object.defineProperty(onboardingProperties, 'lastErrorCode', {
        get: function () {
            if (!localSettings.values.hasKey(onboardingLastErrorCodeKey)) {
                return OnboardingEnumTypes.ConnectionResultCode.ErrorMessage;
            } else {
                return localSettings.values[onboardingLastErrorCodeKey];
            }
        },
        set: function (value) {
            localSettings.values[onboardingLastErrorCodeKey] = value;
        }
    });

    Object.defineProperty(onboardingProperties, 'lastErrorMessage', {
        get: function () {
            if (!localSettings.values.hasKey(onboardingLastErrorMessageKey)) {
                return "No Last Error available";
            } else {
                return localSettings.values[onboardingLastErrorMessageKey];
            }
        },
        set: function (value) {
            localSettings.values[onboardingLastErrorMessageKey] = value;
        }
    });

    var getSampleNetworkScanList = function () {
        var network1 = new onboarding.OnboardingScanListItem();
        network1.value1 = "SampleNetwork1";
        network1.value2 = OnboardingEnumTypes.AuthenticationType.Open;

        var network2 = new onboarding.OnboardingScanListItem();
        network2.value1 = "SampleNetwork2";
        network2.value2 = OnboardingEnumTypes.AuthenticationType.Wep;

        var network3 = new onboarding.OnboardingScanListItem();
        network3.value1 = "SampleNetwork3";
        network3.value2 = OnboardingEnumTypes.AuthenticationType.WpaTkip;

        var network4 = new onboarding.OnboardingScanListItem();
        network4.value1 = "SampleNetwork4";
        network4.value2 = OnboardingEnumTypes.AuthenticationType.WpaCcmp;

        var network5 = new onboarding.OnboardingScanListItem();
        network5.value1 = "SampleNetwork5";
        network5.value2 = OnboardingEnumTypes.AuthenticationType.Wpa2Tkip;

        var network6 = new onboarding.OnboardingScanListItem();
        network6.value1 = "SampleNetwork6";
        network6.value2 = OnboardingEnumTypes.AuthenticationType.Wpa2Ccmp;

        var network7 = new onboarding.OnboardingScanListItem();
        network7.value1 = "SampleNetwork7";
        network7.value2 = OnboardingEnumTypes.AuthenticationType.Wps;

        var networkScanList = [];
        networkScanList.push(network1);
        networkScanList.push(network2);
        networkScanList.push(network3);
        networkScanList.push(network4);
        networkScanList.push(network5);
        networkScanList.push(network6);
        networkScanList.push(network7);

        return networkScanList;
    }

    var sampleNetworkPassword = "SamplePassword";

    WinJS.Namespace.define("AppData", {
        secureInterfaceProperties: secureInterfaceProperties,
        onboardingConfiguration: onboardingConfiguration,
        onboardingProperties: onboardingProperties,
        getSampleNetworkScanList: getSampleNetworkScanList,
        sampleNetworkPassword: sampleNetworkPassword
    });
})();