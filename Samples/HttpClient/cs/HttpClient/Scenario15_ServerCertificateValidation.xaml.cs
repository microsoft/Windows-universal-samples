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

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Security.Cryptography.Certificates;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;
using Windows.Web.Http.Filters;


namespace SDKSample.HttpClientSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario15 : Page, IDisposable
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private HttpBaseProtocolFilter filter;
        private HttpClient httpClient;
        private CancellationTokenSource cts;

        public Scenario15()
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
            // In this scenario we just create an HttpClient instance with default settings. I.e. no custom filters. 
            // For examples on how to use custom filters see other scenarios.
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

            Helpers.ScenarioStarted(StartButton, CancelButton, null);
            rootPage.NotifyUser("In progress", NotifyType.StatusMessage);

            try
            {
                if (DefaultOSValidation.IsChecked.Value)
                {
                    // Do nothing.
                }
                else if (DefaultAndCustomValidation.IsChecked.Value)
                {
                    // Add event handler to listen to the ServerCustomValidationRequested event. By default, OS validation
                    // will be performed before the event is raised.
                    filter.ServerCustomValidationRequested += MyCustomServerCertificateValidator;
                }
                else if (IgnoreErrorsAndCustomValidation.IsChecked.Value)
                {
                    // ---------------------------------------------------------------------------
                    // WARNING: Only test applications should ignore SSL errors.
                    // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
                    // attacks (while the connection is secure, the server is not authenticated).
                    // Note that not all certificate validation errors can be ignored.
                    // ---------------------------------------------------------------------------
                    filter.IgnorableServerCertificateErrors.Add(ChainValidationResult.Untrusted);
                    filter.IgnorableServerCertificateErrors.Add(ChainValidationResult.InvalidName);

                    // Add event handler to listen to the ServerCustomValidationRequested event.
                    // This event handler must implement the desired custom certificate validation logic.
                    filter.ServerCustomValidationRequested += MyCustomServerCertificateValidator;
                }

                // Here, we turn off writing to and reading from the cache to ensure that each request actually 
                // hits the network and tries to establish an SSL/TLS connection with the server.
                filter.CacheControl.WriteBehavior = HttpCacheWriteBehavior.NoCache;
                filter.CacheControl.ReadBehavior = HttpCacheReadBehavior.NoCache;

                HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, resourceAddress);
                HttpResponseMessage response = await httpClient.SendRequestAsync(
                    request,
                    HttpCompletionOption.ResponseHeadersRead).AsTask(cts.Token);

                rootPage.NotifyUser("Success - response received from server. Server certificate was valid.", NotifyType.StatusMessage);
            }
            catch (TaskCanceledException)
            {
                rootPage.NotifyUser("Request canceled.", NotifyType.ErrorMessage);
            }
            catch (Exception ex)
            {
                // Network errors are reported as exceptions.
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
            httpClient?.Dispose();
            httpClient = null;

            cts?.Dispose();
            cts = null;
        }

        // This event handler for server certificate validation executes synchronously as part of the SSL/TLS handshake. 
        // An app should not perform lengthy operations inside this handler. Otherwise, the remote server may terminate the connection abruptly.
        private async void MyCustomServerCertificateValidator(HttpBaseProtocolFilter sender, HttpServerCustomValidationRequestedEventArgs args)
        {
            // Get the server certificate and certificate chain from the args parameter.
            Certificate serverCert = args.ServerCertificate;
            IReadOnlyList<Certificate> serverCertChain = args.ServerIntermediateCertificates;

            // To illustrate the use of async APIs, we use the IsCertificateValidAsync method.
            // In order to call async APIs in this handler, you must first take a deferral and then
            // release it once you are done with the operation. The "using" statement
            // ensures that the deferral completes when control leaves the method.
            using (Deferral deferral = args.GetDeferral())
            {
                try
                {
                    bool isCertificateValid = await IsCertificateValidAsync(serverCert);
                    if (!isCertificateValid)
                    {
                        args.Reject();
                    }
                }
                catch
                {
                    // If we get an exception from IsCertificateValidAsync, we reject the certificate
                    // (secure by default).
                    args.Reject();
                }
            }
        }

        private async Task<bool> IsCertificateValidAsync(Certificate serverCert)
        {
            // This is a placeholder call to simulate long-running async calls. Note that this code runs synchronously as part of the SSL/TLS handshake. 
            // Avoid performing lengthy operations here - else, the remote server may terminate the connection abruptly. 
            await Task.Delay(100);

            // In this sample, we compare the hash code of the certificate to a specific integer - this is purely 
            // for illustration purposes and should not be considered as a recommendation for certificate validation.
            const int trustedHashCode = 6044116;
            if (serverCert.GetHashCode().Equals(trustedHashCode))
            {
                // If certificate satisfies the criteria, return true.
                return true;
            }
            else
            {
                // If certificate does not meet the required criteria,return false.
                return false;
            }
        }

        private void DefaultOSValidation_Checked(object sender, RoutedEventArgs e)
        {
            AddressField.Text = "https://www.microsoft.com";
        }

        private void DefaultAndCustomValidation_Checked(object sender, RoutedEventArgs e)
        {
            AddressField.Text = "https://www.microsoft.com";
        }

        private void IgnoreErrorsAndCustomValidation_Checked(object sender, RoutedEventArgs e)
        {
            AddressField.Text = "https://localhost/HttpClientSample/default.aspx";
        }
    }


}
