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
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
using Windows.ApplicationModel.Core;
using PhoneCall.Helpers;

namespace PhoneCall.ViewModels
{
    public class StatusViewModel : ViewModelBase
    {
        private string deviceType;
        private string visibilityState = "Collapsed";
        private int simSlotIndex;
        private string simState;
        private string networkState;
        private string displayName;
        private string operatorName;
        private string displayColor;
        private SolidColorBrush displayAccentBrush;
        private string doesPhoneCallExist;
        private string extendedPropertiesString = "Loading...";

        /// <summary>
        /// A view model class for the status panel/module.
        /// </summary>
        public StatusViewModel()
        {
            UpdateStatusInfo();
            CallingInfo.CallingInfoInstance.CellInfoUpdateCompleted += CallingInfoInstance_CellInfoUpdateCompletedAsync;
            CallingInfo.CallingInfoInstance.ActivePhoneCallStateChanged += CallingInfoInstance_ActivePhoneCallStateChangedAsync;
        }

        /// <summary>
        /// Ensures the UI is updated upon initilization of the cellular details singleton.
        /// Even if the status module is loaded before the singleton is done intializaing
        /// This is fired from the calling info singleton
        /// </summary>
        private async void CallingInfoInstance_CellInfoUpdateCompletedAsync()
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                () =>
                {
                    UpdateStatusInfo();
                });
        }

        /// <summary>
        /// Ensures the call state UI is updated upon any change in the call state.
        /// This is fired from the calling info singleton
        /// </summary>
        private async void CallingInfoInstance_ActivePhoneCallStateChangedAsync()
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                () =>
                {
                    UpdateCallState();
                });
        }

        /// <summary>
        /// Updates the status view with information from the calling info singleton.
        /// </summary>
        public void UpdateStatusInfo()
        {
            //Color Values to be used during update
            Color NoColor = Color.FromArgb(0, 0, 0, 0);
            Color DisplayAccentColor = (Color)Application.Current.Resources["SystemAccentColor"];

            //Set default device type, will be overriden below if error or multi line
            DeviceType = "Single Line Device";

            //Check # of lines to make sure its indeed a single line device
            if (CallingInfo.CallingInfoInstance.NoOfLines < 1)
            {
                DeviceType = "Error";
            }
            //Show and setup multiSIM components
            if (CallingInfo.CallingInfoInstance.NoOfLines >= 2)
            {
                //Set Device type
                DeviceType = "Multi Line Device";

                //Show Multi line controls
                VisibilityState = "Visible";

                //Set SIM slot index
                SimSlotIndex = CallingInfo.CallingInfoInstance.CurrentSIMSlotIndex;
            }

            //Set SIM state
            SimState = CallingInfo.CallingInfoInstance.CurrentSIMState;

            //Set network state
            NetworkState = CallingInfo.CallingInfoInstance.CurrentNetworkState;

            //Set branding info
            DisplayName = CallingInfo.CallingInfoInstance.CurrentDisplayName;
            OperatorName = CallingInfo.CallingInfoInstance.CurrentOperatorName;
            Color DisplayColorObject = CallingInfo.CallingInfoInstance.CurrentDisplayColor;
            DisplayColor = DisplayColorObject.ToString();
            
            //Set Accent highlight based on display
            if (DisplayColorObject == NoColor)
            {
                DisplayColorObject = DisplayAccentColor;
            }
            DisplayAccentBrush = new SolidColorBrush(DisplayColorObject);

            //Set call state
            UpdateCallState();

            //Get and Set extended properties
            if (CallingInfo.CallingInfoInstance.CurrentPhoneLine != null)
            {
                ExtendedPropertiesString = "Line found but extended properties not found";
                var eProp = CallingInfo.CallingInfoInstance.CurrentPhoneLine.LineConfiguration.ExtendedProperties;
                if (eProp != null)
                {
                    ExtendedPropertiesString = "";
                    foreach (var pair in eProp)
                    {
                        ExtendedPropertiesString += string.Format(CultureInfo.InvariantCulture, "{0}: {1}", pair.Key, pair.Value == null ? "null" : pair.Value.ToString()) + Environment.NewLine;
                    }
                }
            }
        }

        /// <summary>
        /// Updates the "Is there an active call" status updated when changed.
        /// </summary>
        private void UpdateCallState()
        {
            DoesPhoneCallExist = CallingInfo.CallingInfoInstance.DoesPhoneCallExist ? "Yes" : "No";
        }

        #region Setters and Getters
        /// <summary>
        /// Gets and sets the Device type (Single vs Multi line device).
        /// </summary>
        public string DeviceType
        {
            get
            {
                return deviceType;
            }
            private set
            {
                if (deviceType != value)
                {
                    deviceType = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the if we should show/hide multi line elements.
        /// </summary>
        public string VisibilityState
        {
            get
            {
                return visibilityState;
            }
            private set
            {
                if (visibilityState != value)
                {
                    visibilityState = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the sim slot index on the current line.
        /// </summary>
        public int SimSlotIndex
        {
            get
            {
                return simSlotIndex;
            }
            private set
            {
                if (simSlotIndex != value)
                {
                    simSlotIndex = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the sim state on the current line.
        /// </summary>
        public string SimState
        {
            get
            {
                return simState;
            }
            private set
            {
                if (simState != value)
                {
                    simState = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the network state on the current line.
        /// </summary>
        public string NetworkState
        {
            get
            {
                return networkState;
            }
            private set
            {
                if (networkState != value)
                {
                    networkState = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the display name on the current line.
        /// </summary>
        public string DisplayName
        {
            get
            {
                return displayName;
            }
            private set
            {
                if (displayName != value)
                {
                    displayName = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the operator name on the current line.
        /// </summary>
        public string OperatorName
        {
            get
            {
                return operatorName;
            }
            private set
            {
                if (operatorName != value)
                {
                    operatorName = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the display color on the current line.
        /// </summary>
        public string DisplayColor
        {
            get
            {
                return displayColor;
            }
            private set
            {
                if (displayColor != value)
                {
                    displayColor = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the display color brush on the current line.
        /// </summary>
        public SolidColorBrush DisplayAccentBrush
        {
            get
            {
                return displayAccentBrush;
            }
            private set
            {
                if (displayAccentBrush != value)
                {
                    displayAccentBrush = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the "Is there an active call".
        /// </summary>
        public string DoesPhoneCallExist
        {
            get
            {
                return doesPhoneCallExist;
            }
            private set
            {
                if (doesPhoneCallExist != value)
                {
                    doesPhoneCallExist = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the extended properties strings on the current line.
        /// </summary>
        public string ExtendedPropertiesString
        {
            get
            {
                return extendedPropertiesString;
            }
            private set
            {
                if (extendedPropertiesString != value)
                {
                    extendedPropertiesString = value;
                    RaisePropertyChanged();
                }
            }
        }
        #endregion
    }
}
