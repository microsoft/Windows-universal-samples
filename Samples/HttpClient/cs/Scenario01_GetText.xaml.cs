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
using Windows.Security.Cryptography.Certificates;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;
using Windows.Web.Http.Filters;

namespace SDKTemplate
{
    public sealed partial class Scenario01_GetText : Page
    {
        MainPage rootPage = MainPage.Current;

        private HttpBaseProtocolFilter filter;
        private HttpClient httpClient;
        private CancellationTokenSource cts;
        private bool isFilterUsed;

        public Scenario01_GetText()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {    
            // In this scenario we just create an HttpClient instance with default settings. I.e. no custom filters. 
            // For examples on how to use custom filters see other scenarios.
            filter = new HttpBaseProtocolFilter();

            // Setting the starting state of the toggle based on the default HTTP version in the OS
            if (filter.MaxVersion == HttpVersion.Http20)
            {
                MaxHttpVersionToggle.IsOn = true;
            }
            else
            {
                MaxHttpVersionToggle.IsOn = false;
            }

            httpClient = new HttpClient(filter);
            cts = new CancellationTokenSource();
            isFilterUsed = false;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            cts.Cancel();
            cts.Dispose();
            httpClient.Dispose();
            filter.Dispose();
        }

        private async void Start_Click(object sender, RoutedEventArgs e)
        {
            // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
            // valid, absolute URI, we'll notify the user about the incorrect input.
            Uri resourceUri = Helpers.TryParseHttpUri(AddressField.Text);
            if (resourceUri == null)
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            Helpers.ScenarioStarted(StartButton, CancelButton, OutputField);
            rootPage.NotifyUser("In progress", NotifyType.StatusMessage);
            
            if (ReadDefaultRadio.IsChecked.Value)
            {
                filter.CacheControl.ReadBehavior = HttpCacheReadBehavior.Default;
            }
            else if (ReadMostRecentRadio.IsChecked.Value)
            {
                filter.CacheControl.ReadBehavior = HttpCacheReadBehavior.MostRecent;
            }
            else if (ReadOnlyFromCacheRadio.IsChecked.Value)
            {
                filter.CacheControl.ReadBehavior = HttpCacheReadBehavior.OnlyFromCache;
            }
            else if (ReadNoCacheRadio.IsChecked.Value)
            {
                filter.CacheControl.ReadBehavior = HttpCacheReadBehavior.NoCache;
            }

            if (WriteDefaultRadio.IsChecked.Value)
            {
                filter.CacheControl.WriteBehavior = HttpCacheWriteBehavior.Default;
            }
            else if (WriteNoCacheRadio.IsChecked.Value)
            {
                filter.CacheControl.WriteBehavior = HttpCacheWriteBehavior.NoCache;
            }

            // ---------------------------------------------------------------------------
            // WARNING: Only test applications should ignore SSL errors.
            // In real applications, ignoring server certificate errors can lead to MITM
            // attacks (while the connection is secure, the server is not authenticated).
            //
            // The SetupServer script included with this sample creates a server certificate that is self-signed
            // and issued to fabrikam.com, and hence we need to ignore these errors here. 
            // ---------------------------------------------------------------------------
            filter.IgnorableServerCertificateErrors.Add(ChainValidationResult.Untrusted);
            filter.IgnorableServerCertificateErrors.Add(ChainValidationResult.InvalidName);

            // This sample uses a "try" in order to support TaskCanceledException.
            // If you don't need to support cancellation, then the "try" is not needed.
            try
            {
                isFilterUsed = true;
                HttpRequestResult result = await httpClient.TryGetAsync(resourceUri).AsTask(cts.Token);

                if (result.Succeeded)
                {
                    await Helpers.DisplayTextResultAsync(result.ResponseMessage, OutputField, cts.Token);

                    rootPage.NotifyUser(
                        "Completed. Response came from " + result.ResponseMessage.Source + ". HTTP version used: " + result.ResponseMessage.Version.ToString() + ".",
                        NotifyType.StatusMessage);
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

            Helpers.ScenarioCompleted(StartButton, CancelButton);
        }

        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            cts.Cancel();
            cts.Dispose();

            // Re-create the CancellationTokenSource.
            cts = new CancellationTokenSource();
        }

        private void MaxHttpVersionToggle_Toggled(object sender, RoutedEventArgs e)
        {
            HttpVersion httpVersion = HttpVersion.Http11;
            if (MaxHttpVersionToggle.IsOn)
            {
                httpVersion = HttpVersion.Http20;
            }

            // In general, for best memory utilization, HttpBaseProtocolFilter and HttpClient objects should be reused for multiple requests.
            // However, if the user has already used the filter object to send at least one request, the HTTP version of the filter cannot be changed.
            // Hence, we need to create new client and filter objects.
            if (isFilterUsed)
            {
                filter.Dispose();
                httpClient.Dispose();
                filter = new HttpBaseProtocolFilter();
                httpClient = new HttpClient(filter);
                isFilterUsed = false;
            }
            filter.MaxVersion = httpVersion;
        }
    }
}
