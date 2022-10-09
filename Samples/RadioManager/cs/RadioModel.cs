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
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Windows.Devices.Radios;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace SDKTemplate
{
    public class RadioModel : INotifyPropertyChanged
    {
        private Radio radio;
        private bool isEnabled;
        private CoreDispatcher dispatcher;

        public RadioModel(Radio radio, CoreDispatcher dispatcher)
        {
            this.radio = radio;
            // Controlling the mobile broadband radio requires the cellularDeviceControl restricted capability, which we do not have.
            this.isEnabled = (radio.Kind != RadioKind.MobileBroadband);
            this.dispatcher = dispatcher;
            this.radio.StateChanged += Radio_StateChanged;
        }

        private async void Radio_StateChanged(Radio sender, object args)
        {
            // The Radio StateChanged event doesn't run from the UI thread, so we must use the dispatcher
            // to run NotifyPropertyChanged
            await this.dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                NotifyPropertyChanged("IsRadioOn");
            });
        }

        public string Name
        {
            get
            {
                return this.radio.Name;
            }
        }

        public bool IsRadioOn
        {
            get
            {
                return this.radio.State == RadioState.On;
            }
            set
            {
                SetRadioState(value);
            }
        }

        public bool IsEnabled
        {
            get
            {
                return this.isEnabled;
            }
            set
            {
                this.isEnabled = value;
                NotifyPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged([CallerMemberName] string propertyName = "")
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private async void SetRadioState(bool isRadioOn)
        {
            if (isRadioOn != IsRadioOn)
            {
                var radioState = isRadioOn ? RadioState.On : RadioState.Off;
                IsEnabled = false;
                await this.radio.SetStateAsync(radioState);
                NotifyPropertyChanged("IsRadioOn");
                IsEnabled = true;
            }
        }
    }
}
