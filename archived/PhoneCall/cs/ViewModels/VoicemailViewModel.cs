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
using System.Windows.Input;
using Windows.ApplicationModel.Core;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
using PhoneCall.Helpers;

namespace PhoneCall.ViewModels
{
    class VoicemailViewModel : ViewModelBase
    {
        private int voicemailCount;
        private string voicemailNumber;
        private SolidColorBrush displayAccentBrush;
        private RelayCommand dialVoicemailCommand;

        /// <summary>
        /// A view model class for the voicemail panel/module.
        /// </summary>
        public VoicemailViewModel()
        {
            UpdateVoicemailInfo();
            CallingInfo.CallingInfoInstance.CellInfoUpdateCompleted += CallingInfoInstance_CellInfoUpdateCompletedAsync;
        }

        /// <summary>
        /// Ensures the UI is updated upon initilization of the cellular details singleton.
        /// Even if the voicemail module is loaded before the singleton is done intializaing
        /// This is fired from the calling info singleton
        /// </summary>
        private async void CallingInfoInstance_CellInfoUpdateCompletedAsync()
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                () =>
                {
                    UpdateVoicemailInfo();
                });
        }

        /// <summary>
        /// Updates the voiemail view with information from the calling info singleton.
        /// </summary>
        public void UpdateVoicemailInfo()
        {
            //Color Values to be used during update
            Color NoColor = Color.FromArgb(0, 0, 0, 0);
            Color DisplayAccentColor = (Color)Application.Current.Resources["SystemAccentColor"];

            //Set voicemail number
            VoicemailNumber = CallingInfo.CallingInfoInstance.CurrentVoicemailNumber.Trim();
            VoicemailNumber = (VoicemailNumber == "") ? "N/A" : VoicemailNumber;

            //Set voicemail count
            VoicemailCount = CallingInfo.CallingInfoInstance.CurrentVoicemailCount;

            //Set Accent highlight based on display
            Color DisplayColorObject = CallingInfo.CallingInfoInstance.CurrentDisplayColor;
            if (DisplayColorObject == NoColor)
            {
                DisplayColorObject = DisplayAccentColor;
            }
            DisplayAccentBrush = new SolidColorBrush(DisplayColorObject);
        }

        /// <summary>
        /// Relay command action to dial the current line's voicemail number.
        /// </summary>
        private async void DialVoicemailAsync(object CommandParam)
        {
            if (CallingInfo.CallingInfoInstance.CurrentPhoneLine != null)
            {
                await CallingInfo.CallingInfoInstance.CurrentPhoneLine.Voicemail.DialVoicemailAsync();
            }
        }

        /// <summary>
        /// Relay command to execute the dial voicemail action from the voicemail view.
        /// </summary>
        public ICommand ProcessDialVoicemail
        {
            get
            {
                if (dialVoicemailCommand == null)
                {
                    dialVoicemailCommand = new RelayCommand(
                        DialVoicemailAsync);
                }
                return dialVoicemailCommand;
            }
        }

        #region Setters and Getters
        /// <summary>
        /// Gets and sets the number of voicemails on the current line.
        /// </summary>
        public int VoicemailCount
        {
            get
            {
                return voicemailCount;
            }
            private set
            {
                if (voicemailCount != value)
                {
                    voicemailCount = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the voicemail number on the current line.
        /// </summary>
        public string VoicemailNumber
        {
            get
            {
                return voicemailNumber;
            }
            private set
            {
                if (voicemailNumber != value)
                {
                    voicemailNumber = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the display color of the current line.
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
