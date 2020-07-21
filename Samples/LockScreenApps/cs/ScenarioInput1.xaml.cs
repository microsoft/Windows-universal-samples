// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

using System;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplateCS;

namespace LockScreenAppsCS
{
    public sealed partial class ScenarioInput1 : Page
    {
        // A pointer back to the main page which is used to gain access to the input and output frames and their content.
        MainPage rootPage = null;

        public ScenarioInput1()
        {
            InitializeComponent();
        }

        private async void OpenNotificationSettings()
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri("ms-settings:notifications"));
        }

        #region Template-Related Code - Do not remove
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Get a pointer to our main page
            rootPage = e.Parameter as MainPage;
        }

        #endregion
    }
}
