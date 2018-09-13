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
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// This page is used to handle authentication by background task scenario
    /// </summary>
    public sealed partial class Scenario2_BackgroundTask : Page
    {
        // A pointer back to the main page.  This is needed if you call methods in MainPage such as NotifyUser()
        MainPage rootPage = MainPage.Current;
        public Scenario2_BackgroundTask()
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
            // Configure background task handler to perform authentication
            ConfigStore.AuthenticateThroughBackgroundTask = true;

            // Setup completion handler
            ScenarioCommon.Instance.RegisteredCompletionHandlerForBackgroundTask();
        }
    }
}
