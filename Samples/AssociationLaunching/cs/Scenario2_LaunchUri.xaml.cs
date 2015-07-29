//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Foundation;
using SDKTemplate;
using System;

namespace AssociationLaunching
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class LaunchUri : Page
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage such as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public LaunchUri()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
            {
                // Disable scenarios that are not supported in Phone.
                LaunchUriOpenWithPanel.Visibility = Visibility.Collapsed;
                LaunchUriWithWarningPanel.Visibility = Visibility.Collapsed;
                LaunchUriSplitScreenPanel.Visibility = Visibility.Collapsed;
            }
        }

        /// <summary>
        // Launch a URI.
        /// </summary>
        private async void LaunchUriButton_Click(object sender, RoutedEventArgs e)
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Launch the URI.
            bool success = await Windows.System.Launcher.LaunchUriAsync(uri);
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
        private async void LaunchUriWithWarningButton_Click(object sender, RoutedEventArgs e)
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Configure the warning prompt.
            var options = new Windows.System.LauncherOptions();
            options.TreatAsUntrusted = true;

            // Launch the URI.
            bool success = await Windows.System.Launcher.LaunchUriAsync(uri, options);
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
        private async void LaunchUriOpenWithButton_Click(object sender, RoutedEventArgs e)
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Calulcate the position for the Open With dialog.
            // An alternative to using the point is to set the rect of the UI element that triggered the launch.
            Point openWithPosition = GetOpenWithPosition(LaunchUriOpenWithButton);

            // Next, configure the Open With dialog.
            var options = new Windows.System.LauncherOptions();
            options.DisplayApplicationPicker = true;
            options.UI.InvocationPoint = openWithPosition;
            options.UI.PreferredPlacement = Windows.UI.Popups.Placement.Below;

            // Launch the URI.
            bool success = await Windows.System.Launcher.LaunchUriAsync(uri, options);
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
        private async void LaunchUriSplitScreenButton_Click(object sender, RoutedEventArgs e)
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Configure the request for split screen launch.
            var options = new Windows.System.LauncherOptions();

            if (!(Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons")))
            {
                if (Default.IsSelected == true)
                {
                    options.DesiredRemainingView = Windows.UI.ViewManagement.ViewSizePreference.Default;
                }
                else if (UseLess.IsSelected == true)
                {
                    options.DesiredRemainingView = Windows.UI.ViewManagement.ViewSizePreference.UseLess;
                }
                else if (UseHalf.IsSelected == true)
                {
                    options.DesiredRemainingView = Windows.UI.ViewManagement.ViewSizePreference.UseHalf;
                }
                else if (UseMore.IsSelected == true)
                {
                    options.DesiredRemainingView = Windows.UI.ViewManagement.ViewSizePreference.UseMore;
                }
                else if (UseMinimum.IsSelected == true)
                {
                    options.DesiredRemainingView = Windows.UI.ViewManagement.ViewSizePreference.UseMinimum;
                }
                else if (UseNone.IsSelected == true)
                {
                    options.DesiredRemainingView = Windows.UI.ViewManagement.ViewSizePreference.UseNone;
                }
            }

            // Launch the URI.
            bool success = await Windows.System.Launcher.LaunchUriAsync(uri, options);
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
        // The Open With dialog should be displayed just under the element that triggered it.
        /// </summary>
        private Windows.Foundation.Point GetOpenWithPosition(FrameworkElement element)
        {
            Windows.UI.Xaml.Media.GeneralTransform buttonTransform = element.TransformToVisual(null);

            Point desiredLocation = buttonTransform.TransformPoint(new Point());
            desiredLocation.Y = desiredLocation.Y + element.ActualHeight;

            return desiredLocation;
        }

        private void OnPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
            {
                // Adjust UI elements layout for small screen on Phone
                LaunchUriPanel.Orientation = Orientation.Vertical;
                LaunchUriDescription.Width = this.ActualWidth;
                LaunchUriDescription.TextWrapping = TextWrapping.Wrap;
                LaunchUriDescription.HorizontalAlignment = HorizontalAlignment.Left;

                UriToLaunchPanel.Orientation = Orientation.Vertical;
                UriToLaunchDescription.Width = this.ActualWidth;
                UriToLaunchDescription.TextWrapping = TextWrapping.Wrap;
                UriToLaunchDescription.HorizontalAlignment = HorizontalAlignment.Left;
            }
        }
    }
}