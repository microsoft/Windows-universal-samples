//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using AppUIBasics.Common;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.Resources;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.UI.ApplicationSettings;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Hub App template is documented at http://go.microsoft.com/fwlink/?LinkId=286574

namespace AppUIBasics
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    sealed partial class App : Application
    {
        /// <summary>
        /// Initializes the singleton Application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            this.InitializeComponent();
            this.Suspending += OnSuspending;
            this.RequiresPointerMode = ApplicationRequiresPointerMode.WhenRequested;
        }

        /// <summary>
        /// Invoked when the application is launched normally by the end user.  Other entry points
        /// will be used such as when the application is launched to open a specific file.
        /// </summary>
        /// <param name="e">Details about the launch request and process.</param>
        protected override async void OnLaunched(LaunchActivatedEventArgs e)
        {
#if DEBUG
            //if (System.Diagnostics.Debugger.IsAttached)
            //{
            //    this.DebugSettings.EnableFrameRateCounter = true;
            //}
#endif
            await ShowWindow(e);
            this.ShowDisclaimer();

        }

        private async Task ShowWindow(LaunchActivatedEventArgs e)
        {
            NavigationRootPage rootPage = Window.Current.Content as NavigationRootPage;

            // Do not repeat app initialization when the Window already has content,
            // just ensure that the window is active
            if (rootPage == null)
            {
                rootPage = new NavigationRootPage();

                // Retrieve the root Frame to act as the navigation context and navigate to the first page
                Frame rootFrame = (Frame)rootPage.FindName("rootFrame");

                if (rootFrame == null)
                {
                    throw new Exception("Root frame not found");
                }

                // Associate the frame with a SuspensionManager key.
                SuspensionManager.RegisterFrame(rootFrame, "AppFrame");

                // Set the default language
                rootFrame.Language = Windows.Globalization.ApplicationLanguages.Languages[0];
                
                rootFrame.NavigationFailed += OnNavigationFailed;

                if (e.PreviousExecutionState == ApplicationExecutionState.Terminated)
                {
                    // Restore the saved session state only when appropriate
                    try
                    {
                        await SuspensionManager.RestoreAsync();
                    }
                    catch (SuspensionManagerException)
                    {
                        //Something went wrong restoring state.
                        //Assume there is no state and continue
                    }
                }

                if (rootFrame.Content == null)
                {
                    // When the navigation stack isn't restored navigate to the first page.
                    rootFrame.Navigate(typeof(MainPage), e.Arguments);
                }

                // Place the main page in the current Window.
                Window.Current.Content = rootPage;
            }

            // Ensure the current window is active
            Window.Current.Activate();
        }

        /// <summary>
        /// Invoked when Navigation to a certain page fails
        /// </summary>
        /// <param name="sender">The Frame which failed navigation</param>
        /// <param name="e">Details about the navigation failure</param>
        void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            throw new Exception("Failed to load Page " + e.SourcePageType.FullName);
        }

        /// <summary>
        /// Invoked when application execution is being suspended.  Application state is saved
        /// without knowing whether the application will be terminated or resumed with the contents
        /// of memory still intact.
        /// </summary>
        /// <param name="sender">The source of the suspend request.</param>
        /// <param name="e">Details about the suspend request.</param>
        private async void OnSuspending(object sender, SuspendingEventArgs e)
        {
            //SettingsPane.GetForCurrentView().CommandsRequested -= App_CommandsRequested;
            var deferral = e.SuspendingOperation.GetDeferral();
            await SuspensionManager.SaveAsync();
            deferral.Complete();
        }
        
        private async void ShowDisclaimer()
        {
            ResourceLoader resourceLoader = ResourceLoader.GetForCurrentView();
            var roamingSettings = ApplicationData.Current.RoamingSettings;

            // Show disclaimer regardless of how the app has been activated
            // unless disclaimer already accepted.
            var disclaimer = (bool)(roamingSettings.Values["disclaimer"] ?? false);

            // If no disclaimer response, show disclaimer.
            if (!disclaimer)
            {
                // Get disclaimer resources.
                // See Globalizing your app at http://go.microsoft.com/fwlink/?LinkId=258266.
                var resDisclaimer = resourceLoader.GetString("disclaimer").Replace(@"\n", Environment.NewLine);
                if (String.IsNullOrEmpty(resDisclaimer)) resDisclaimer = "Disclaimer.";
                var resDisclaimerTitle = resourceLoader.GetString("disclaimerTitle");
                if (String.IsNullOrEmpty(resDisclaimerTitle)) resDisclaimerTitle = "Disclaimer";
                var resDisclaimerButton = resourceLoader.GetString("disclaimerButton");
                if (String.IsNullOrEmpty(resDisclaimerButton)) resDisclaimerButton = "Accept";

                // Create a disclaimer message dialog and set its content.
                // A message dialog can support up to three commands. 
                // If no commands are specified, a close command is provided by default.
                // Note: Message dialogs should be used sparingly, and only for messages or 
                // questions critical to the continued use of your app.
                // See Adding message dialogs at http://go.microsoft.com/fwlink/?LinkID=275116.
                var msg = new Windows.UI.Popups.MessageDialog(resDisclaimer, resDisclaimerTitle);

                // Handler for disclaimer.
                msg.Commands.Add(new Windows.UI.Popups.UICommand(resDisclaimerButton,
                    _ => roamingSettings.Values["disclaimer"] = true));

                // If specifying your own commmands, set the command that will be invoked by default.
                // For example, msg.defaultCommandIndex = 1;

                // Show the message dialog
                await msg.ShowAsync();
            }
        }
    }
}
