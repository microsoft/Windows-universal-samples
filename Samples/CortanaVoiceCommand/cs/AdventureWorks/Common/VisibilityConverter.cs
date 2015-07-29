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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;

namespace AdventureWorks.Common
{
    /// <summary>
    /// Convert a Boolean status to Visibility.Visible (true) or Visibility.Collapsed (false).
    /// Assists in avoiding having the view model keep references to UI types.
    /// </summary>
    public class VisibilityConverter : IValueConverter
    {
        /// <summary>
        /// Convert a boolean into a member of the Visibility enum, true for Visible, false for Collapsed.
        /// </summary>
        /// <param name="value">The bool passed in</param>
        /// <param name="targetType">Ignored.</param>
        /// <param name="parameter">Ignored</param>
        /// <param name="language">Ignored</param>
        /// <returns>Visibility.Visible if value was a true bool, otherwise Visibility.Collapsed</returns>
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if(value is bool)
            {
                if((bool)value == true)
                {
                    return Visibility.Visible;
                }
            }
            return Visibility.Collapsed;
        }

        /// <summary>
        /// Not used, converter is not intended for two-way binding. 
        /// </summary>
        /// <param name="value">Ignored</param>
        /// <param name="targetType">Ignored</param>
        /// <param name="parameter">Ignored</param>
        /// <param name="language">Ignored</param>
        /// <returns></returns>
        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
