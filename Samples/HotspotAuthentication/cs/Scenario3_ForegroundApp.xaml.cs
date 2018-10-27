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

using HotspotAuthenticationTask;
using System;
using Windows.Networking.NetworkOperators;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// The page is used to handle authenticate by foreground app scenario
    /// </summary>
    public sealed partial class Scenario3_ForegroundApp : Page
    {
        // A pointer back to the main page.  This is needed to call methods in MainPage such as NotifyUser()
        MainPage rootPage = MainPage.Current;

        HotspotAuthenticationContext authenticationContext;

        public Scenario3_ForegroundApp()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Configure background task handler to trigger foregound app for authentication
            ConfigStore.AuthenticateThroughBackgroundTask = false;

            // Setup completion handler
            ScenarioCommon.Instance.RegisteredCompletionHandlerForBackgroundTask();

            // Register event to update UI state on authentication event
            ScenarioCommon.Instance.BackgroundAuthenticationCompleted += OnBackgroundAuthenticationCompleted;

            // Check current authentication state
            InitializeForegroundAppAuthentication();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ScenarioCommon.Instance.BackgroundAuthenticationCompleted -= OnBackgroundAuthenticationCompleted;
        }

        /// <summary>
        /// Handle authentication event triggered by background task
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async void OnBackgroundAuthenticationCompleted(ScenarioCommon sender, BackgroundAuthenticationCompletedEventArgs e)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                if (e.Succeeded)
                {
                    InitializeForegroundAppAuthentication();
                }
                else
                {
                    rootPage.NotifyUser($"Background task encountered exception: {e.ExtendedError}", NotifyType.ErrorMessage);
                }
            });
        }

        private async void AuthenticateButton_Click(object sender, RoutedEventArgs e)
        {
            AuthenticateButton.IsEnabled = false;

            HotspotCredentialsAuthenticationResult result = await authenticationContext.IssueCredentialsAsync(
                ConfigStore.UserName, ConfigStore.Password, ConfigStore.ExtraParameters, ConfigStore.MarkAsManualConnect);
            if (result.ResponseCode == HotspotAuthenticationResponseCode.LoginSucceeded)
            {
                rootPage.NotifyUser("Issuing credentials succeeded", NotifyType.StatusMessage);
                Uri logoffUrl = result.LogoffUrl;
                if (logoffUrl != null)
                {
                    rootPage.NotifyUser("The logoff URL is: " + logoffUrl.OriginalString, NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Issuing credentials failed", NotifyType.ErrorMessage);
            }
            AuthenticateButton.IsEnabled = true;
        }

        private void SkipButton_Click(object sender, RoutedEventArgs e)
        {
            // Here you can implement custom authentication.
            authenticationContext.SkipAuthentication();
            rootPage.NotifyUser("Authentication skipped", NotifyType.StatusMessage);
            ClearAuthenticationToken();
        }

        private void AbortButton_Click(object sender, RoutedEventArgs e)
        {
            authenticationContext.AbortAuthentication(ConfigStore.MarkAsManualConnect);
            rootPage.NotifyUser("Authentication aborted", NotifyType.StatusMessage);
            ClearAuthenticationToken();
        }

        /// <summary>
        /// Query authentication token from application storage and update the UI.
        /// The token gets passed from the background task handler.
        /// </summary>
        private void InitializeForegroundAppAuthentication()
        {
            string token = ConfigStore.AuthenticationToken;
            if (String.IsNullOrEmpty(token))
            {
                return; // no token found
            }
            if (!HotspotAuthenticationContext.TryGetAuthenticationContext(token, out authenticationContext))
            {
                rootPage.NotifyUser("TryGetAuthenticationContext failed", NotifyType.ErrorMessage);
                return;
            }

            AuthenticateButton.IsEnabled = true;
            SkipButton.IsEnabled = true;
            AbortButton.IsEnabled = true;
        }


        /// <summary>
        /// Clear the authentication token in the application storage and update the UI.
        /// </summary>
        private void ClearAuthenticationToken()
        {
            ConfigStore.AuthenticationToken = "";
            AuthenticateButton.IsEnabled = false;
            SkipButton.IsEnabled = false;
            AbortButton.IsEnabled = false;
        }
    }
}

