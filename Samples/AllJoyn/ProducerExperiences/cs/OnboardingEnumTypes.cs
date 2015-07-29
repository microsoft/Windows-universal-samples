//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

namespace AllJoynProducerExperiences
{
    // All the Enums below as per the Onboarding interface spec
    // For more info: https://allseenalliance.org/developers/learn/base-services/onboarding/interface

    enum OnboardingAuthenticationType
    {
        Wpa2Auto = -3,
        WpaAuto = -2,
        Any = -1,
        Open = 0,
        Wep = 1,
        WpaTkip = 2,
        WpaCcmp = 3,
        Wpa2Tkip = 4,
        Wpa2Ccmp = 5,
        Wps = 6,
    }

    enum ConfigureWiFiResultStatus
    {
        NonConcurrent = 1,
        Concurrent = 2,
    }

    enum ConnectionResultCode
    {
        Validated = 0,
        Unreachable,
        UnsupportedProtocol,
        Unauthorized,
        ErrorMessage,
    }

    enum ConfigurationState
    {
        NotConfigured = 0,
        NotValidated,
        Validating,
        Validated,
        Error,
        Retry,
    }
}
