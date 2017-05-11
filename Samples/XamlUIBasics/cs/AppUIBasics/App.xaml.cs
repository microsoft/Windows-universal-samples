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
using AppUIBasics.Data;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text.RegularExpressions;
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
            this.Resuming += App_Resuming;
            this.RequiresPointerMode = ApplicationRequiresPointerMode.WhenRequested;
            string selectedTheme = ApplicationData.Current.LocalSettings.Values["SelectedAppTheme"]?.ToString();
            if (selectedTheme != null)
            {
                Enum.TryParse(selectedTheme, out ApplicationTheme appTheme);
                RequestedTheme = appTheme;
            }
        }

        private void App_Resuming(object sender, object e)
        {
            switch (NavigationRootPage.RootFrame?.Content)
            {
                case ItemPage page:
                    page.SetInitialVisuals();
                    break;
                case NewControlsPage page:
                case MainPage mainPage:
                    NavigationRootPage.Current.NavigationView.AlwaysShowHeader = false;
                    break;
            }
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
            Windows.UI.ViewManagement.ApplicationView.GetForCurrentView().SetDesiredBoundsMode(Windows.UI.ViewManagement.ApplicationViewBoundsMode.UseCoreWindow);
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

        protected async override void OnActivated(IActivatedEventArgs args)
        {
            if (args.Kind == ActivationKind.Protocol)
            {
                Match match;
                Type targetPageType = typeof(MainPage);
                string targetId = string.Empty;
                Frame rootFrame = GetRootFrame();

                switch (((ProtocolActivatedEventArgs)args).Uri?.AbsolutePath)
                {
                    case string s when IsMatching(s, "/category/(.*)"):
                        targetId = match.Groups[1]?.ToString();
                        var groups = NavigationRootPage.Current.Groups ?? (await ControlInfoDataSource.GetGroupsAsync()).ToList();
                        if (groups.Any(g => g.UniqueId == targetId))
                        {
                            targetPageType = typeof(SectionPage);
                        }
                        break;

                    case string s when IsMatching(s, "/item/(.*)"):
                        targetId = match.Groups[1]?.ToString();
                        groups = NavigationRootPage.Current.Groups ?? (await ControlInfoDataSource.GetGroupsAsync()).ToList();
                        if (groups.Any(g => g.Items.Any(i => i.UniqueId == targetId)))
                        {
                            targetPageType = typeof(ItemPage);
                        }
                        break;
                }
                rootFrame.Navigate(targetPageType, targetId);

                bool IsMatching(string parent, string expression)
                {
                    match = Regex.Match(parent, expression);
                    return match.Success;
                }
            }
            base.OnActivated(args);
        }

        private Frame GetRootFrame()
        {
            Frame rootFrame;
            NavigationRootPage rootPage = Window.Current.Content as NavigationRootPage;
            if (rootPage == null)
            {
                rootPage = new NavigationRootPage();
                rootFrame = (Frame)rootPage.FindName("rootFrame");
                rootFrame.Language = Windows.Globalization.ApplicationLanguages.Languages[0];
                rootFrame.NavigationFailed += OnNavigationFailed;
                Window.Current.Content = rootPage;
                Window.Current.Activate();
            }
            else
            {
                rootFrame = (Frame)rootPage.FindName("rootFrame");
            }
            return rootFrame;
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
    }
}
