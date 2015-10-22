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
using Windows.Foundation;
using Windows.System;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_LaunchUri : Page
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage such as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario2_LaunchUri()
        {
            this.InitializeComponent();
            ViewPreference.ItemsSource = MainPage.ViewSizePreferences;
            ViewPreference.SelectedIndex = 0;
        }

        /// <summary>
        // Launch a URI.
        /// </summary>
        private async void LaunchUriDefault()
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Launch the URI.
            bool success = await Launcher.LaunchUriAsync(uri);
            if (success)
            {
                rootPage.NotifyUser("URI launched: " + uri.AbsoluteUri, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        // Launch a URI. Show a warning prompt.
        /// </summary>
        private async void LaunchUriWithWarning()
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Configure the warning prompt.
            var options = new LauncherOptions() { TreatAsUntrusted = true };

            // Launch the URI.
            bool success = await Launcher.LaunchUriAsync(uri, options);
            if (success)
            {
                rootPage.NotifyUser("URI launched: " + uri.AbsoluteUri, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        // Launch a URI. Show an Open With dialog that lets the user chose the handler to use.
        /// </summary>
        private async void LaunchUriOpenWith(object sender, RoutedEventArgs e)
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Calulcate the position for the Open With dialog.
            // An alternative to using the point is to set the rect of the UI element that triggered the launch.
            Point openWithPosition = MainPage.GetElementLocation(sender);

            // Next, configure the Open With dialog.
            var options = new LauncherOptions();
            options.DisplayApplicationPicker = true;
            options.UI.InvocationPoint = openWithPosition;
            options.UI.PreferredPlacement = Windows.UI.Popups.Placement.Below;

            // Launch the URI.
            bool success = await Launcher.LaunchUriAsync(uri, options);
            if (success)
            {
                rootPage.NotifyUser("URI launched: " + uri.AbsoluteUri, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        // Launch a URI. Request to share the screen with the launched app.
        /// </summary>
        private async void LaunchUriSplitScreen()
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Configure the request for split screen launch.
            var options = new LauncherOptions() { DesiredRemainingView = (ViewSizePreference)ViewPreference.SelectedValue };

            // Launch the URI.
            bool success = await Launcher.LaunchUriAsync(uri, options);
            if (success)
            {
                rootPage.NotifyUser("URI launched: " + uri.AbsoluteUri, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage);
            }
        }
    }
}