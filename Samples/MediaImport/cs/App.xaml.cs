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
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Media.Import;
using System.Collections.Generic;
using System.Diagnostics;

// The Blank Application template is documented at http://go.microsoft.com/fwlink/?LinkId=402347&clcid=0x409

namespace SDKTemplate
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    sealed partial class App : Application
    {
        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            this.InitializeComponent();
            this.Suspending += OnSuspending;
        }

        /// <summary>
        /// Invoked when the application is launched normally by the end user.  Other entry points
        /// will be used such as when the application is launched to open a specific file.
        /// </summary>
        /// <param name="e">Details about the launch request and process.</param>
        protected override async void OnLaunched(LaunchActivatedEventArgs e)
        {

#if DEBUG
            if (System.Diagnostics.Debugger.IsAttached)
            {
                this.DebugSettings.EnableFrameRateCounter = false;
            }
#endif

            Frame rootFrame = Window.Current.Content as Frame;

            // Do not repeat app initialization when the Window already has content,
            // just ensure that the window is active

            if (rootFrame == null)
            {
                // Create a Frame to act as the navigation context and navigate to the first page
                rootFrame = new Frame();
                // Set the default language
                rootFrame.Language = Windows.Globalization.ApplicationLanguages.Languages[0];

                rootFrame.NavigationFailed += OnNavigationFailed;
                rootFrame.Navigate(typeof(MainPage));

                // if our app was previous terminated or wasn't running, check if there were pending progress
                if (e.PreviousExecutionState == ApplicationExecutionState.Terminated || e.PreviousExecutionState == ApplicationExecutionState.NotRunning)
                {
                    try
                    {
                        bool isImportSupported = await PhotoImportManager.IsSupportedAsync();
                        if (isImportSupported)
                        {
                            // Determine if there was an in progress session
                            IReadOnlyList<PhotoImportOperation> ops = PhotoImportManager.GetPendingOperations();

                            // If there is at least one session and state isn't idle, restore session state

                            if (ops != null && ops.Count != 0)
                            {
                                var p = rootFrame.Content as MainPage;

                                if (ops.Count > 1)
                                {
                                    // If there is more than one operation, since this app handles single
                                    // download sessions only, the previous session(s) were not properly
                                    // disposed of, do this now.

                                    for (int x = 0; x < ops.Count - 1; x++)
                                    {
                                        ops[x].Session.Dispose();
                                    }
                                }

                                // Take the last operation from the list, and if some task was started, navigate to reconnect to the proper stage.

                                p.ImportOperation = ops[ops.Count - 1];

                                if (p.ImportOperation.Stage != PhotoImportStage.NotStarted)
                                {
                                    p.NavigateToReconnect();
                                }
                                else
                                {
                                    p.ImportOperation.Session.Dispose();
                                }
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Debug.WriteLine("Exception: " + ex.Message);
                    }
                }

                // Place the frame in the current Window

                Window.Current.Content = rootFrame;
            }

            if (rootFrame.Content == null)
            {
                // When the navigation stack isn't restored navigate to the first page,
                // configuring the new page by passing required information as a navigation
                // parameter
                rootFrame.Navigate(typeof(MainPage), e.Arguments);
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
        /// Invoked when application is being suspended.  Application state is saved
        /// without knowing whether the application will be terminated or resumed with the contents
        /// of memory still intact.
        /// </summary>
        /// <param name="sender">The source of the suspend request.</param>
        /// <param name="e">Details about the suspend request.</param>
        private void OnSuspending(object sender, SuspendingEventArgs e)
        {
            var deferral = e.SuspendingOperation.GetDeferral();
            //TODO: Save application state and stop any background activity
            deferral.Complete();
        }

        protected override void OnActivated(IActivatedEventArgs args)
        {
            // AutoPlay scenario: This app is registered for as a hanlder for WPD\ImageSourceAutoPlay event in the Package.appxmanifest
            if (args.Kind == ActivationKind.Device)
            {
                Frame rootFrame = null;
                // Ensure that the current page exists and is activated
                if (Window.Current.Content == null)
                {
                    rootFrame = new Frame();
                    rootFrame.Navigate(typeof(MainPage));
                    Window.Current.Content = rootFrame;
                }
                else
                {
                    rootFrame = Window.Current.Content as Frame;
                }

                // Reference the current page as type MainPage
                 var page = rootFrame.Content as MainPage;

                // Cast the activated event args as DeviceActivatedEventArgs and navigate to scenario1 to preselect the device
                var deviceArgs = args as DeviceActivatedEventArgs;
                page.DeviceEvent = deviceArgs;
                page.NavigateToAutoPlay();

                Window.Current.Activate();
            }

            base.OnActivated(args);
        }
    }
}
