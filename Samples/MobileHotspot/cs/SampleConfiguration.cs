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
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Windows.Networking.Connectivity;
using Windows.Networking.NetworkOperators;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "MobileHotspot C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Configure Mobile Hotspot", ClassType=typeof(Scenario1_ConfigureMobileHotspot)},
            new Scenario() { Title="Toggle Mobile Hotspot", ClassType=typeof(Scenario2_ToggleMobileHotspot)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    public static class Helpers
    {
        public static NetworkOperatorTetheringManager TryGetCurrentNetworkOperatorTetheringManager()
        {
            // Get the connection profile associated with the internet connection currently used by the local machine.
            ConnectionProfile currentConnectionProfile = NetworkInformation.GetInternetConnectionProfile();
            if (currentConnectionProfile == null)
            {
                MainPage.Current.NotifyUser("System is not connected to the Internet.", NotifyType.ErrorMessage);
                return null;
            }

            TetheringCapability tetheringCapability =
                NetworkOperatorTetheringManager.GetTetheringCapabilityFromConnectionProfile(currentConnectionProfile);

            if (tetheringCapability != TetheringCapability.Enabled)
            {
                string message;
                switch (tetheringCapability)
                {
                    case TetheringCapability.DisabledByGroupPolicy:
                        message = "Tethering is disabled due to group policy.";
                        break;
                    case TetheringCapability.DisabledByHardwareLimitation:
                        message = "Tethering is not available due to hardware limitations.";
                        break;
                    case TetheringCapability.DisabledByOperator:
                        message = "Tethering operations are disabled for this account by the network operator.";
                        break;
                    case TetheringCapability.DisabledByRequiredAppNotInstalled:
                        message = "An application required for tethering operations is not available.";
                        break;
                    case TetheringCapability.DisabledBySku:
                        message = "Tethering is not supported by the current account services.";
                        break;
                    case TetheringCapability.DisabledBySystemCapability:
                        // This will occur if the "wiFiControl" capability is missing from the App.
                        message = "This app is not configured to access Wi-Fi devices on this machine.";
                        break;
                    default:
                        message = $"Tethering is disabled on this machine. (Code {(int)tetheringCapability}).";
                        break;
                }
                MainPage.Current.NotifyUser(message, NotifyType.ErrorMessage);
                return null;
            }

            const int E_NOT_FOUND = unchecked((int)0x80070490); // HRESULT_FROM_WIN32(ERROR_NOT_FOUND)

            try
            {
                return NetworkOperatorTetheringManager.CreateFromConnectionProfile(currentConnectionProfile);
            }
            catch (Exception ex) when (ex.HResult == E_NOT_FOUND)
            {
                MainPage.Current.NotifyUser("System has no Wi-Fi adapters.", NotifyType.ErrorMessage);
                return null;
            }
        }

        public static string GetFriendlyName(TetheringWiFiBand value)
        {
            switch (value)
            {
                case TetheringWiFiBand.Auto: return "Any available";
                case TetheringWiFiBand.TwoPointFourGigahertz: return "2.4 GHz";
                case TetheringWiFiBand.FiveGigahertz: return "5 Ghz";
                case TetheringWiFiBand.SixGigahertz: return "6 GHz";
                default: return $"Unknown ({(uint)value})";
            }
        }

        public static string GetFriendlyName(TetheringWiFiAuthenticationKind value)
        {
            switch (value)
            {
                case TetheringWiFiAuthenticationKind.Wpa2:
                    return "WPA2";
                case TetheringWiFiAuthenticationKind.Wpa3TransitionMode:
                    return "WPA2/WPA3";
                case TetheringWiFiAuthenticationKind.Wpa3:
                    return "WPA3";
                default:
                    return $"Unknown ({(uint)value})";
            }
        }

        const int E_INVALID_STATE = unchecked((int)0x8007139f); // HRESULT_FROM_WIN32(ERROR_INVALID_STATE)

        // Calls NetworkOperatorTetheringAccessPointConfiguration.IsBandSupported but handles
        // certain boundary cases.

        public static bool IsBandSupported(NetworkOperatorTetheringAccessPointConfiguration configuration, TetheringWiFiBand band)
        {
            // "Auto" mode is always supported even though it is technically not a frequency band.
            if (band == TetheringWiFiBand.Auto)
            {
                return true;
            }

            try
            {
                return configuration.IsBandSupported(band);
            }
            catch (Exception ex) when (ex.HResult == E_INVALID_STATE)
            {
                // The WiFi device has been disconnected. Report that we support nothing.
                return false;
            }
        }
    }
}
