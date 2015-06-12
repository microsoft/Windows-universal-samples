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
using System.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;
using Windows.Devices.WiFiDirect.Services;

namespace SDKTemplate.Helpers
{
    public class ServiceStatusConverter : IValueConverter
    {
        /// <summary>
        /// Compares the bound value with an enum param. Returns true when they match.
        /// </summary>
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return ((WiFiDirectServiceStatus)value).ToString();
        }

        /// <summary>
        /// Converts the boolean back into an enum.
        /// </summary>
        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return Enum.Parse(typeof(WiFiDirectServiceStatus), (string)value);
        }
    }
}
