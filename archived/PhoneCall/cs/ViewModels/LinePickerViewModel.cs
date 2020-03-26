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
using System.Collections.ObjectModel;
using System.Linq;
using Windows.ApplicationModel.Calls;
using Windows.ApplicationModel.Core;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
using PhoneCall.Helpers;

namespace PhoneCall.ViewModels
{
    class LinePickerViewModel : ViewModelBase
    {
        private string visibilityState = "Collapsed";
        private ObservableCollection<PhoneLinePickerItem> phoneLines = new ObservableCollection<PhoneLinePickerItem>();
        private string displayName;
        private SolidColorBrush displayAccentBrush;

        /// <summary>
        /// A view model class for the line picker.
        /// </summary>
        public LinePickerViewModel()
        {
            UpdateLinePickerInfo();
            CallingInfo.CallingInfoInstance.CellInfoUpdateCompleted += CallingInfoInstance_CellInfoUpdateCompletedAsync;
        }

        /// <summary>
        /// Ensures the UI is updated upon initilization of the cellular details singleton.
        /// Even if the line picker module is loaded before the singleton is done intializaing
        /// This is fired from the calling info singleton
        /// </summary>
        private async void CallingInfoInstance_CellInfoUpdateCompletedAsync()
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                () =>
                {
                    UpdateLinePickerInfo();
                });
        }

        /// <summary>
        /// Updates the line picker view with information from the calling info singleton.
        /// </summary>
        public void UpdateLinePickerInfo()
        {
            //Color Values to be used during update
            Color NoColor = Color.FromArgb(0, 0, 0, 0);
            Color DisplayAccentColor = (Color)Application.Current.Resources["SystemAccentColor"];

            //Show and setup multiSIM components
            if (CallingInfo.CallingInfoInstance.NoOfLines >= 2)
            {
                //Show Multi line controls
                VisibilityState = "Visible";

                //Set Phone Lines                
                if (CallingInfo.CallingInfoInstance.AllPhoneLines != null)
                {
                    PhoneLines.Clear();

                    //Sortphonelines by slot index to ensure cosistent line picker order
                    var phoneLinesQuery = from phoneLine in CallingInfo.CallingInfoInstance.AllPhoneLines.Values
                                orderby ((PhoneLine)phoneLine).CellularDetails.SimSlotIndex
                                select phoneLine;

                    foreach (var line in phoneLinesQuery)
                    {
                        Color ActualLineColor = line.DisplayColor;
                        if (ActualLineColor == NoColor)
                        {
                            ActualLineColor = DisplayAccentColor;
                        }
                        SolidColorBrush ActualLineColorBrush = new SolidColorBrush(ActualLineColor);
                        PhoneLines.Add(new PhoneLinePickerItem(line.DisplayName, line.Id, ActualLineColorBrush));
                    }
                }
            }

            //Set branding info
            DisplayName = CallingInfo.CallingInfoInstance.CurrentDisplayName;
            Color DisplayColorObject = CallingInfo.CallingInfoInstance.CurrentDisplayColor;

            //Set Accent highlight based on display
            if (DisplayColorObject == NoColor)
            {
                DisplayColorObject = DisplayAccentColor;
            }
            DisplayAccentBrush = new SolidColorBrush(DisplayColorObject);
        }

        /// <summary>
        /// Changes the current line to the one selected by the line picker.
        /// </summary>
        internal void ChangePhoneLine(Guid lineGuid)
        {
            CallingInfo.CallingInfoInstance.ChangePhoneLine(lineGuid);
            //Note: this fires an event in celling info when 
            //Line update is made
            //Line update -> info update -> event fire -> update this UI
        }

        #region Setters and Getters
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
        /// Gets and sets the list of active cellular lines on the device.
        /// </summary>
        public ObservableCollection<PhoneLinePickerItem> PhoneLines
        {
            get
            {
                return phoneLines;
            }
            private set
            {
                if (phoneLines != value)
                {
                    phoneLines = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the line display name for the current line.
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
        /// Gets and sets the display color brush for the current line.
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
        #endregion
    }
}
