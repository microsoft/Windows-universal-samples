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

using org.alljoyn.Onboarding;
using System;
using System.Collections.Generic;
using Windows.Storage;

namespace AllJoynProducerExperiences
{
    class AppData
    {
        private const string isUpperCaseEnabledPropertyKey = "IsUpperCaseEnabled";
        private const string onboardingConfigureSsidKey = "OnboardingConfigureSsid";
        private const string onboardingConfigurePassphraseKey = "OnboardingConfigurePassphrase";
        private const string onboardingConfigureAuthenticationTypeKey = "OnboardingConfigureAuthenticationType";
        private const string onboardingConfigurationStateKey = "OnboardingConfigurationState";
        private const string onboardingLastErrorCodeKey = "OnboardingLastErrorCode";
        private const string onboardingLastErrorMessageKey = "OnboardingLastErrorMessage";
        private static ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;

        public static event EventHandler IsUpperCaseEnabledPropertyDataChanged;

        public static bool IsUpperCaseEnabled
        {
            get
            {
                if (!localSettings.Values.ContainsKey(isUpperCaseEnabledPropertyKey))
                {
                    localSettings.Values[isUpperCaseEnabledPropertyKey] = false;
                }
                return (bool)localSettings.Values[isUpperCaseEnabledPropertyKey];
            }
            set
            {
                if (IsUpperCaseEnabled != value)
                {
                    localSettings.Values[isUpperCaseEnabledPropertyKey] = value;
                    EventHandler handler = IsUpperCaseEnabledPropertyDataChanged;
                    if (handler != null)
                    {
                        handler(new object(), new EventArgs());
                    }
                }
            }
        }

        public static string OnboardingConfigureSsid
        {
            get
            {
                if (!localSettings.Values.ContainsKey(onboardingConfigureSsidKey))
                {
                    return null;
                }
                else
                {
                    return (string)localSettings.Values[onboardingConfigureSsidKey];
                }
            }
            set
            {
                localSettings.Values[onboardingConfigureSsidKey] = value;
            }
        }

        public static string OnboardingConfigurePassphrase
        {
            get
            {
                if (!localSettings.Values.ContainsKey(onboardingConfigurePassphraseKey))
                {
                    return null;
                }
                else
                {
                    return (string)localSettings.Values[onboardingConfigurePassphraseKey];
                }
            }
            set
            {
                localSettings.Values[onboardingConfigurePassphraseKey] = value;
            }
        }

        public static short OnboardingConfigureAuthType
        {
            get
            {
                if (!localSettings.Values.ContainsKey(onboardingConfigureAuthenticationTypeKey))
                {
                    return (short)OnboardingAuthenticationType.Any;
                }
                else
                {
                    return (short)localSettings.Values[onboardingConfigureAuthenticationTypeKey];
                }
            }
            set
            {
                localSettings.Values[onboardingConfigureAuthenticationTypeKey] = value;
            }
        }

        public static short OnboardingConfigurationState
        {
            get
            {
                if (!localSettings.Values.ContainsKey(onboardingConfigurationStateKey))
                {
                    localSettings.Values[onboardingConfigurationStateKey] = (short)ConfigurationState.NotConfigured;
                }
                return (short)localSettings.Values[onboardingConfigurationStateKey];
            }
            set
            {
                localSettings.Values[onboardingConfigurationStateKey] = value;
            }
        }

        public static short LastErrorCode
        {
            get
            {
                if (localSettings.Values.ContainsKey(onboardingLastErrorCodeKey))
                {
                    return (short)localSettings.Values[onboardingLastErrorCodeKey];
                }
                else
                {
                    return (short)ConnectionResultCode.ErrorMessage;
                }
            }
            set
            {
                localSettings.Values[onboardingLastErrorCodeKey] = value;
            }
        }

        public static string LastErrorMessage
        {
            get
            {
                if (localSettings.Values.ContainsKey(onboardingLastErrorMessageKey))
                {
                    return (string)localSettings.Values[onboardingLastErrorMessageKey];
                }
                else
                {
                    return "No Last Error available";
                }
            }
            set
            {
                localSettings.Values[onboardingLastErrorMessageKey] = value;
            }
        }

        public static List<OnboardingScanListItem> SampleNetworkScanList
        {
            get
            {
                // Simulated network scan data.

                OnboardingScanListItem network1 = new OnboardingScanListItem();
                network1.Value1 = "SampleNetwork1";
                network1.Value2 = (short)OnboardingAuthenticationType.Open;

                OnboardingScanListItem network2 = new OnboardingScanListItem();
                network2.Value1 = "SampleNetwork2";
                network2.Value2 = (short)OnboardingAuthenticationType.Wep;

                OnboardingScanListItem network3 = new OnboardingScanListItem();
                network3.Value1 = "SampleNetwork3";
                network3.Value2 = (short)OnboardingAuthenticationType.WpaTkip;

                OnboardingScanListItem network4 = new OnboardingScanListItem();
                network4.Value1 = "SampleNetwork4";
                network4.Value2 = (short)OnboardingAuthenticationType.WpaCcmp;

                OnboardingScanListItem network5 = new OnboardingScanListItem();
                network5.Value1 = "SampleNetwork5";
                network5.Value2 = (short)OnboardingAuthenticationType.Wpa2Tkip;

                OnboardingScanListItem network6 = new OnboardingScanListItem();
                network6.Value1 = "SampleNetwork6";
                network6.Value2 = (short)OnboardingAuthenticationType.Wpa2Ccmp;

                OnboardingScanListItem network7 = new OnboardingScanListItem();
                network7.Value1 = "SampleNetwork7";
                network7.Value2 = (short)OnboardingAuthenticationType.Wps;

                List<OnboardingScanListItem> networkScanList = new List<OnboardingScanListItem>();
                networkScanList.Add(network1);
                networkScanList.Add(network2);
                networkScanList.Add(network3);
                networkScanList.Add(network4);
                networkScanList.Add(network5);
                networkScanList.Add(network6);
                networkScanList.Add(network7);

                return networkScanList;
            }
        }

        public const string SampleNetworkPassword = "SamplePassword";
    }
}
