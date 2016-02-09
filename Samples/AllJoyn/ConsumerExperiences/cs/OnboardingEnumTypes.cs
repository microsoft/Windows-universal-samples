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

using System;
using Windows.UI.Xaml.Data;

namespace AllJoynConsumerExperiences
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
        NotConcurrent = 1,
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

    public class AuthTypeToStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return value.ToString();
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            OnboardingAuthenticationType returnValue;

            if (Enum.TryParse(value.ToString(), true, out returnValue))
            {
                return returnValue;
            }
            else
            {
                return OnboardingAuthenticationType.Any;
            }
        }
    }
}
