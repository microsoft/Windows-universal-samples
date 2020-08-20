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
using Windows.Media.Capture;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_AppCapture : Page
    {
        AppCapture appCapture;
        MainPage rootPage = MainPage.Current;

        public Scenario3_AppCapture()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            appCapture = AppCapture.GetForCurrentView();
            appCapture.CapturingChanged += AppCapture_CapturingChanged;
            UpdateCaptureStatus();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            appCapture.CapturingChanged -= AppCapture_CapturingChanged;

            // Re-enable app capture when navigating away.
            var task = AppCapture.SetAllowedAsync(true);
        }

        async void AllowAppCaptureCheckBox_Toggled(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = (ToggleSwitch)sender;
            await AppCapture.SetAllowedAsync(toggleSwitch.IsOn);
        }

        void AppCapture_CapturingChanged(AppCapture sender, object e)
        {
            UpdateCaptureStatus();
        }

        void UpdateCaptureStatus()
        {
            string audioStatus = appCapture.IsCapturingAudio ? "Audio is being captured." : "Audio is not being captured.";
            string videoStatus = appCapture.IsCapturingVideo ? "Video is being captured." : "Video is not being captured.";
            rootPage.NotifyUser(audioStatus + "\n" + videoStatus, NotifyType.StatusMessage);
        }
    }
}
