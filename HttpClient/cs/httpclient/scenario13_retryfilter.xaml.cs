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

using HttpFilters;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using Windows.Web.Http;
using Windows.Web.Http.Filters;

namespace SDKSample.HttpClientSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario13 : Page, IDisposable
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private HttpClient httpClient;
        private CancellationTokenSource cts;

        public Scenario13()
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
            HttpBaseProtocolFilter baseProtocolFilter = new HttpBaseProtocolFilter();
            HttpRetryFilter retryFilter = new HttpRetryFilter(baseProtocolFilter);
            httpClient = new HttpClient(retryFilter);
            cts = new CancellationTokenSource();
            UpdateAddressField();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // If the navigation is external to the app do not clean up.
            // This can occur on Phone when suspending the app.
            if (e.NavigationMode == NavigationMode.Forward && e.Uri == null)
            {
                return;
            }

            Dispose();
        }

        private void UpdateAddressField()
        {
            // Tell the server which type of Retry-After header we would like to receive.
            string queryString = "";
            if (RetryAfterSwitch.IsOn)
            {
                queryString += "?retryAfter=deltaSeconds";
            }
            else
            {
                queryString += "?retryAfter=httpDate";
            }

            Helpers.ReplaceQueryString(AddressField, queryString);
        }

        private void RetryAfterSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            UpdateAddressField();
        }

        private async void Start_Click(object sender, RoutedEventArgs e)
        {
            Uri resourceAddress;

            // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
            // valid, absolute URI, we'll notify the user about the incorrect input.
            if (!Helpers.TryGetUri(AddressField.Text, out resourceAddress))
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            Helpers.ScenarioStarted(StartButton, CancelButton, OutputField);
            rootPage.NotifyUser("In progress", NotifyType.StatusMessage);

            try
            {
                HttpResponseMessage response = await httpClient.GetAsync(resourceAddress).AsTask(cts.Token);

                await Helpers.DisplayTextResultAsync(response, OutputField, cts.Token);

                rootPage.NotifyUser("Completed", NotifyType.StatusMessage);
            }
            catch (TaskCanceledException)
            {
                rootPage.NotifyUser("Request canceled.", NotifyType.ErrorMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error: " + ex.Message, NotifyType.ErrorMessage);
            }
            finally
            {
                Helpers.ScenarioCompleted(StartButton, CancelButton);
            }
        }

        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            cts.Cancel();
            cts.Dispose();

            // Re-create the CancellationTokenSource.
            cts = new CancellationTokenSource();
        }

        public void Dispose()
        {
            if (httpClient != null)
            {
                httpClient.Dispose();
                httpClient = null;
            }

            if (cts != null)
            {
                cts.Dispose();
                cts = null;
            }
        }
    }
}
