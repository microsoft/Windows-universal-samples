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
using System.Threading;
using System.Threading.Tasks;
using Windows.Web.Http;
using Windows.Web.Http.Filters;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKSample.HttpClientSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario12 : Page, IDisposable
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private HttpBaseProtocolFilter filter;
        private HttpClient httpClient;
        private CancellationTokenSource cts;

        public Scenario12()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            filter = new HttpBaseProtocolFilter();
            httpClient = new HttpClient(filter);
            cts = new CancellationTokenSource();
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
        private async void SendInitialGetButton_Click(object sender, RoutedEventArgs e)
        {
            Uri resourceAddress;

            // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
            // valid, absolute URI, we'll notify the user about the incorrect input.
            if (!Helpers.TryGetUri(AddressField.Text, out resourceAddress))
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            Helpers.ScenarioStarted(SendInitialGetButton, CancelButton, OutputField);
            rootPage.NotifyUser("In progress", NotifyType.StatusMessage);

            try
            {
                HttpResponseMessage response = await httpClient.GetAsync(resourceAddress).AsTask(cts.Token);
                HttpCookieCollection cookieCollection = filter.CookieManager.GetCookies(resourceAddress);
                OutputField.Text = cookieCollection.Count + " cookies found.\r\n";
                foreach (HttpCookie cookie in cookieCollection)
                {
                    OutputField.Text += "--------------------\r\n";
                    OutputField.Text += "Name: " + cookie.Name + "\r\n";
                    OutputField.Text += "Domain: " + cookie.Domain + "\r\n";
                    OutputField.Text += "Path: " + cookie.Path + "\r\n";
                    OutputField.Text += "Value: " + cookie.Value + "\r\n";
                    OutputField.Text += "Expires: " + cookie.Expires + "\r\n";
                    OutputField.Text += "Secure: " + cookie.Secure + "\r\n";
                    OutputField.Text += "HttpOnly: " + cookie.HttpOnly + "\r\n";
                }

                rootPage.NotifyUser("Completed", NotifyType.StatusMessage);
                SendNextRequestButton.IsEnabled = true;
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
                Helpers.ScenarioCompleted(SendInitialGetButton, CancelButton);
            }
        }

        private async void SendNextRequestButton_Click(object sender, RoutedEventArgs e)
        {
            Uri resourceAddress;

            // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
            // valid, absolute URI, we'll notify the user about the incorrect input.
            if (!Helpers.TryGetUri(AddressField.Text, out resourceAddress))
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            Helpers.ScenarioStarted(SendNextRequestButton, CancelButton, OutputField);
            rootPage.NotifyUser("In progress", NotifyType.StatusMessage);
            if (CookieUsageBehaviorToggle.IsOn)
            {
                filter.CookieUsageBehavior = HttpCookieUsageBehavior.Default;
            }
            else
            {
                // Disable cookie handling - this will do two things: no cookies are attached to outgoing request, and
                // cookies contained in incoming server responses are not added to the CookieManager of the HttpBaseProtocolFilter.
                filter.CookieUsageBehavior = HttpCookieUsageBehavior.NoCookies;
            }

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
                Helpers.ScenarioCompleted(SendNextRequestButton, CancelButton);
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