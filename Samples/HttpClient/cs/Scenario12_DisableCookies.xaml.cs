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
using System.Threading;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;
using Windows.Web.Http.Filters;

namespace SDKTemplate
{
    public sealed partial class Scenario12_DisableCookies : Page
    {
        MainPage rootPage = MainPage.Current;

        private HttpBaseProtocolFilter filter;
        private HttpClient httpClient;
        private CancellationTokenSource cts;

        public Scenario12_DisableCookies()
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
            cts.Cancel();
            cts.Dispose();
            httpClient.Dispose();
            filter.Dispose();
        }

        private async void SendInitialGetButton_Click(object sender, RoutedEventArgs e)
        {
            // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
            // valid, absolute URI, we'll notify the user about the incorrect input.
            Uri resourceUri = Helpers.TryParseHttpUri(AddressField.Text);
            if (resourceUri == null)
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            Helpers.ScenarioStarted(SendInitialGetButton, CancelButton, OutputField);
            rootPage.NotifyUser("In progress", NotifyType.StatusMessage);

            try
            {
                HttpRequestResult result = await httpClient.TryGetAsync(resourceUri).AsTask(cts.Token);

                if (result.Succeeded)
                {
                    HttpCookieCollection cookieCollection = filter.CookieManager.GetCookies(resourceUri);
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
                else
                {
                    Helpers.DisplayWebError(rootPage, result.ExtendedError);
                }
            }
            catch (TaskCanceledException)
            {
                rootPage.NotifyUser("Request canceled.", NotifyType.ErrorMessage);
            }

            Helpers.ScenarioCompleted(SendInitialGetButton, CancelButton);
        }

        private async void SendNextRequestButton_Click(object sender, RoutedEventArgs e)
        {
            Uri resourceUri = Helpers.TryParseHttpUri(AddressField.Text);

            // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
            // valid, absolute URI, we'll notify the user about the incorrect input.
            if (resourceUri == null)
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

            // This sample uses a "try" in order to support TaskCanceledException.
            // If you don't need to support cancellation, then the "try" is not needed.
            try
            {
                HttpRequestResult result = await httpClient.TryGetAsync(resourceUri).AsTask(cts.Token);

                if (result.Succeeded)
                {
                    await Helpers.DisplayTextResultAsync(result.ResponseMessage, OutputField, cts.Token);

                    rootPage.NotifyUser("Completed. Response came from " + result.ResponseMessage.Source.ToString() + ".", NotifyType.StatusMessage);
                }
                else
                {
                    Helpers.DisplayWebError(rootPage, result.ExtendedError);
                }
            }
            catch (TaskCanceledException)
            {
                rootPage.NotifyUser("Request canceled.", NotifyType.ErrorMessage);
            }

            Helpers.ScenarioCompleted(SendNextRequestButton, CancelButton);
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
        }
    }
}