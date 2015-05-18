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
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Data;

namespace AdventureWorks.Common
{
    /// <summary>
    /// Convert a DateTime to a DateTimeOffset for use by a DatePicker, and back
    /// to allow for two-day data-binding.
    /// </summary>
    public class DateTimeOffsetConverter : IValueConverter
    {
        /// <summary>
        /// Convert a DateTime to a DateTimeOffset.
        /// </summary>
        /// <param name="value">The DateTime to convert</param>
        /// <param name="targetType">Ignored.</param>
        /// <param name="parameter">Ignored.</param>
        /// <param name="language">Ignored.</param>
        /// <returns></returns>
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            try
            {
                if(value == null)
                {
                    return "";
                }

                DateTime date = (DateTime)value;
                return string.Format(
                    "{0:dddd MMMM dd yyyy}",
                    date);
            }
            catch (Exception)
            {
                return "";
            }
        }

        /// <summary>
        /// Convert a DateTimeOffset to a DateTime.
        /// </summary>
        /// <param name="value">DateTimeOffset to convert.</param>
        /// <param name="targetType">Ignored.</param>
        /// <param name="parameter">Ignored.</param>
        /// <param name="language">Ignored.</param>
        /// <returns></returns>
        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            try
            {
                string dateText = (string)value;
                DateTime date;
                if (DateTime.TryParse(dateText, out date))
                {
                    return date;
                }

                return null;
            }
            catch (Exception)
            {
                return null;
            }

        }
    }
}
