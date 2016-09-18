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
using Windows.ApplicationModel.Calls.Provider;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_SetDefault : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_SetDefault()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (MainPage.FindBackgroundTaskRegistration() == null)
            {
                rootPage.NotifyUser(TaskNotRegisteredMessage.Text, NotifyType.ErrorMessage);
                OpenSettingsButton.IsEnabled = false;
            }
        }

        private void OpenSettings()
        {
            // Open the call origin settings page.
            PhoneCallOriginManager.ShowPhoneCallOriginSettingsUI();
        }

        private void CheckState()
        {
            bool isActive = PhoneCallOriginManager.IsCurrentAppActiveCallOriginApp;

            if (isActive)
            {
                rootPage.NotifyUser("This app is the active call origin app.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("This app is not the active call origin app.", NotifyType.StatusMessage);
            }
        }
    }
}
