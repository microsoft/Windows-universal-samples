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
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;
using Windows.Web.Http.Filters;
using HttpFilters;

namespace SDKSample.HttpClientSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario14 : Page, IDisposable
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private HttpMeteredConnectionFilter meteredConnectionFilter;
        private HttpClient httpClient;
        private CancellationTokenSource cts;

        public Scenario14()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            HttpBaseProtocolFilter baseProtocolFilter = new HttpBaseProtocolFilter();
            meteredConnectionFilter = new HttpMeteredConnectionFilter(baseProtocolFilter);
            httpClient = new HttpClient(meteredConnectionFilter);
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

            base.OnNavigatedFrom(e);
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

            Helpers.ScenarioStarted(StartButton, CancelButton, OutputField);
            rootPage.NotifyUser("In progress", NotifyType.StatusMessage);

            try
            {
                HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, resourceAddress);

                MeteredConnectionPriority priority = MeteredConnectionPriority.Low;
                if (MediumRadio.IsChecked.Value)
                {
                    priority = MeteredConnectionPriority.Medium;
                }
                else if (HighRadio.IsChecked.Value)
                {
                    priority = MeteredConnectionPriority.High;
                }
                request.Properties["meteredConnectionPriority"] = priority;

                HttpResponseMessage response = await httpClient.SendRequestAsync(request).AsTask(cts.Token);

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
            if (meteredConnectionFilter != null)
            {
                meteredConnectionFilter.Dispose();
                meteredConnectionFilter = null;
            }
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

        private void OptInSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            meteredConnectionFilter.OptIn = OptInSwitch.IsOn;
        }
    }
}
