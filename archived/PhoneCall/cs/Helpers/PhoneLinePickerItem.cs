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
using Windows.UI.Xaml.Media;

namespace PhoneCall.Helpers
{
    public class PhoneLinePickerItem
    {
        private string displayName;
        private Guid id;
        private SolidColorBrush displayColor;

        /// <summary>
        /// A simple class that holds phone line items used by the line picker.
        /// </summary>
        /// <param name="DisplayName">The display name of the line</param>
        /// <param name="Id">The unique guid of the line. Used to retrieve the line</param>
        /// <param name="DisplayColor">The chosen color of the line</param>
        public PhoneLinePickerItem(string DisplayName, Guid Id, SolidColorBrush DisplayColor)
        {
            this.displayName = DisplayName;
            this.id = Id;
            this.displayColor = DisplayColor;
        }

        #region Getters
        /// <summary>
        /// Gets the display name of the line.
        /// </summary>
        public string DisplayName
        {
            get
            {
                return displayName;
            }
        }

        /// <summary>
        /// Gets the id of the line.
        /// </summary>
        public Guid Id
        {
            get
            {
                return id;
            }
        }

        /// <summary>
        /// Gets the display color of the line.
        /// </summary>
        public SolidColorBrush DisplayColor
        {
            get
            {
                return displayColor;
            }
        }
        #endregion
    }
}
