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
using System.Threading.Tasks;
using Windows.Storage;
using Windows.System;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Foundation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_LaunchFile : Page
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage such as NotifyUser().
        MainPage rootPage = MainPage.Current;

        string fileToLaunch = @"Assets\microsoft-sdk.png";

        public Scenario1_LaunchFile()
        {
            this.InitializeComponent();
            ViewPreference.ItemsSource = MainPage.ViewSizePreferences;
            ViewPreference.SelectedIndex = 0;
        }

        private async Task<StorageFile> GetFileToLaunch()
        {
            // First, get the image file from the package's image directory.
            var file = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync(fileToLaunch);

            //Can't launch files directly from install folder so copy it over 
            //to temporary folder first
            file = await file.CopyAsync(ApplicationData.Current.TemporaryFolder, "filetolaunch.png", NameCollisionOption.ReplaceExisting);

            return file;
        }

        /// <summary>
        // Launch a .png file that came with the package.
        /// </summary>
        private async void LaunchFileDefault()
        {
            // First, get the image file from the package's image directory.
            var file = await GetFileToLaunch();

            // Next, launch the file.
            bool success = await Launcher.LaunchFileAsync(file);
            if (success)
            {
                rootPage.NotifyUser("File launched: " + file.Name, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        // Launch a .png file that came with the package. Show a warning prompt.
        /// </summary>
        private async void LaunchFileWithWarning()
        {
            // First, get the image file from the package's image directory.
            var file = await GetFileToLaunch();

            // Next, configure the warning prompt.
            var options = new LauncherOptions() { TreatAsUntrusted = true };

            // Finally, launch the file.
            bool success = await Launcher.LaunchFileAsync(file, options);
            if (success)
            {
                rootPage.NotifyUser("File launched: " + file.Name, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        // Launch a .png file that came with the package. Show an Open With dialog that lets the user chose the handler to use.
        /// </summary>
        private async void LaunchFileOpenWith(object sender, RoutedEventArgs e)
        {
            // First, get the image file from the package's image directory.
            var file = await GetFileToLaunch();

            // Calculate the position for the Open With dialog.
            // An alternative to using the point is to set the rect of the UI element that triggered the launch.
            Point openWithPosition = MainPage.GetElementLocation(sender);

            // Next, configure the Open With dialog.
            var options = new LauncherOptions();
            options.DisplayApplicationPicker = true;
            options.UI.InvocationPoint = openWithPosition;
            options.UI.PreferredPlacement = Windows.UI.Popups.Placement.Below;

            // Finally, launch the file.
            bool success = await Launcher.LaunchFileAsync(file, options);
            if (success)
            {
                rootPage.NotifyUser("File launched: " + file.Name, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        // Launch a .png file that came with the package. Request to share the screen with the launched app.
        /// </summary>
        private async void LaunchFileSplitScreen()
        {
            // First, get a file via the picker.
            var openPicker = new Windows.Storage.Pickers.FileOpenPicker();
            openPicker.FileTypeFilter.Add("*");

            Windows.Storage.StorageFile file = await openPicker.PickSingleFileAsync();
            if (file != null)
            {
                // Configure the request for split screen launch.
                var options = new LauncherOptions() { DesiredRemainingView = (ViewSizePreference)ViewPreference.SelectedValue };

                // Next, launch the file.
                bool success = await Launcher.LaunchFileAsync(file, options);
                if (success)
                {
                    rootPage.NotifyUser("File launched: " + file.Name, NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("No file was picked.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        // Have the user pick a file, then launch it.
        /// </summary>
        private async void PickAndLaunchFile()
        {
            // First, get a file via the picker.
            var openPicker = new Windows.Storage.Pickers.FileOpenPicker();
            openPicker.FileTypeFilter.Add("*");
            Windows.Storage.StorageFile file = await openPicker.PickSingleFileAsync();
            if (file != null)
            {
                // Next, launch the file.
                bool success = await Launcher.LaunchFileAsync(file);
                if (success)
                {
                    rootPage.NotifyUser("File launched: " + file.Name, NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("No file was picked.", NotifyType.ErrorMessage);
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
    }
}
