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
using System.Diagnostics;
using System.Linq;
using System.Text;
using Windows.Storage;

namespace BackgroundAudioShared
{
    /// <summary>
    /// Collection of string constants used in the entire solution. This file is shared for all projects
    /// </summary>
    public static class ApplicationSettingsHelper
    {
        /// <summary>
        /// Function to read a setting value and clear it after reading it
        /// </summary>
        public static object ReadResetSettingsValue(string key)
        {
            Debug.WriteLine(key);
            if (!ApplicationData.Current.LocalSettings.Values.ContainsKey(key))
            {
                Debug.WriteLine("null returned");
                return null;
            }
            else
            {
                var value = ApplicationData.Current.LocalSettings.Values[key];
                ApplicationData.Current.LocalSettings.Values.Remove(key);
                Debug.WriteLine("value found " + value.ToString());
                return value;
            }
        }

        /// <summary>
        /// Save a key value pair in settings. Create if it doesn't exist
        /// </summary>
        public static void SaveSettingsValue(string key, object value)
        {
            Debug.WriteLine(key + ":" + (value == null ? "null" : value.ToString()));

            if (!ApplicationData.Current.LocalSettings.Values.ContainsKey(key))
            {
                ApplicationData.Current.LocalSettings.Values.Add(key, value);
            }
            else
            {
                ApplicationData.Current.LocalSettings.Values[key] = value;
            }
        }

    }
}
