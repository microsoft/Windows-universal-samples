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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.ApplicationModel.DataTransfer;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario7_Share : Page
    {
        private MainPage rootPage;

        public Scenario7_Share()
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
            rootPage = MainPage.Current;

            WebViewControl.Navigate(new Uri("http://www.microsoft.com"));

            // Register for the share event
            DataTransferManager.GetForCurrentView().DataRequested += DataTransferManager_DataRequested;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // Unregister for the share event
            DataTransferManager.GetForCurrentView().DataRequested -= DataTransferManager_DataRequested;
        }

        void WebViewControl_NavigationStarting(WebView sender, WebViewNavigationStartingEventArgs args)
        {
            ProgressControl.IsActive = true;
        }

        void WebViewControl_NavigationCompleted(WebView sender, WebViewNavigationCompletedEventArgs args)
        {
            ProgressControl.IsActive = false;
        }

        private void ShareContent_Click(object sender, RoutedEventArgs e)
        {
            // Show the share UI
            DataTransferManager.ShowShareUI();
        }

        /// <summary>
        /// Raised when the user initiates a Share operation.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        async void DataTransferManager_DataRequested(DataTransferManager sender, DataRequestedEventArgs args)
        {
            DataRequest request = args.Request;
            // We are going to use an async API to talk to the webview, so get a deferral to give
            // us time to generate the results.
            DataRequestDeferral deferral = request.GetDeferral();
            DataPackage dp = await WebViewControl.CaptureSelectedContentToDataPackageAsync();

            // Determine whether there was any selected content.
            bool hasSelection = false;
            try
            {
                hasSelection = (dp != null) && (dp.GetView().AvailableFormats.Count > 0);
            }
            catch (Exception ex)
            {
                switch (ex.HResult)
                {
                    case unchecked((int)0x80070490):
                        // Mobile does not generate a data package with AvailableFormats
                        // and results in an exception. Sorry. Handle the case by acting as
                        // if we had no selected content.
                        hasSelection = false;
                        break;

                    default:
                        // All other exceptions are unexpected. Let them propagate.
                        throw;
                }
            }

            if (hasSelection)
            {
                // Webview has a selection, so we'll share its data package.
                dp.Properties.Title = "WebView sample selected content";
            }
            else
            {
                // No selection, so we'll share the url of the webview.
                dp = new DataPackage();
                dp.SetWebLink(WebViewControl.Source);
                dp.Properties.Title = "WebView sample page";
            }
            dp.Properties.Description = WebViewControl.Source.ToString();
            request.Data = dp;

            deferral.Complete();
        }
    }
}
