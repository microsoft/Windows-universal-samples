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
using System.Diagnostics;
using System.Threading;
using Windows.ApplicationModel.Background;
using Windows.Storage;
using Windows.Networking.NetworkOperators;

// The namespace for the background tasks.
namespace HotspotAuthenticationTask
{
    // A helper class for providing the application configuration.
    public sealed class ConfigStore
    {
        // For the sake of simplicity of the sample, the following authentication parameters are hard coded:
        public static string AuthenticationHost => "login.contoso.com";
        public static string UserName => "MyUserName";
        public static string Password => "MyPassword";
        public static string ExtraParameters => "";
        public static bool MarkAsManualConnect => false;

        public static bool UseNativeWISPr
        {
            get { return GetValueWithDefault("usenativewispr", true); }
            set { ApplicationData.Current.LocalSettings.Values["usenativewispr"] = value; }
        }

        // This flag is set by the foreground app to toogle authentication to be done by the
        // background task handler.
        public static bool AuthenticateThroughBackgroundTask
        {
            get { return GetValueWithDefault("background", true); }
            set { ApplicationData.Current.LocalSettings.Values["background"] = value; }
        }

        // This item is set by the background task handler to pass an authentication event
        // token to the foreground app.
        public static string AuthenticationToken
        {
            get { return GetValueWithDefault("token", ""); }
            set { ApplicationData.Current.LocalSettings.Values["token"] = value; }
        }

        static T GetValueWithDefault<T>(string name, T defaultValue)
        {
            object value;
            if (ApplicationData.Current.LocalSettings.Values.TryGetValue(name, out value) &&
                value is T)
            {
                return (T)value;
            }
            return defaultValue;
        }
    }
}
