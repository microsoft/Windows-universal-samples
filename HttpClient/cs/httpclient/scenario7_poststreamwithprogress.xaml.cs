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
using Windows.Web.Http;
using SDKTemplate;
using System;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using System.IO;
using System.Globalization;

namespace SDKSample.HttpClientSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario7 : Page, IDisposable
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private HttpClient httpClient;
        private CancellationTokenSource cts;

        public Scenario7()
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
            httpClient = new HttpClient();
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
            // Tell the server we want a transfer-encoding chunked response.
            string queryString = "";
            if (ChunkedResponseToggle.IsOn)
            {
                queryString = "?chunkedResponse=1";
            }

            Helpers.ReplaceQueryString(AddressField, queryString);
        }

        private void ChunkedResponseToggle_Toggled(object sender, RoutedEventArgs e)
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

            Helpers.ScenarioStarted(StartButton, CancelButton, null);
            ResetFields();
            rootPage.NotifyUser("In progress", NotifyType.StatusMessage);

            try
            {
                const uint streamLength = 100000;
                HttpStreamContent streamContent = new HttpStreamContent(new SlowInputStream(streamLength));

                // If stream length is unknown, the request is chunked transfer encoded.
                if (!ChunkedRequestToggle.IsOn)
                {
                    streamContent.Headers.ContentLength = streamLength;
                }

                IProgress<HttpProgress> progress = new Progress<HttpProgress>(ProgressHandler);
                HttpResponseMessage response = await httpClient.PostAsync(resourceAddress, streamContent).AsTask(cts.Token, progress);

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
                RequestProgressBar.Value = 100;
                Helpers.ScenarioCompleted(StartButton, CancelButton);
            }
        }

        private void ResetFields()
        {
            StageField.Text = "";
            RetriesField.Text = "0";
            BytesSentField.Text = "0";
            TotalBytesToSendField.Text = "0";
            BytesReceivedField.Text = "0";
            TotalBytesToReceiveField.Text = "0";
            RequestProgressBar.Value = 0;
        }

        private void ProgressHandler(HttpProgress progress)
        {
            StageField.Text = progress.Stage.ToString();
            RetriesField.Text = progress.Retries.ToString(CultureInfo.InvariantCulture);
            BytesSentField.Text = progress.BytesSent.ToString(CultureInfo.InvariantCulture);
            BytesReceivedField.Text = progress.BytesReceived.ToString(CultureInfo.InvariantCulture);

            ulong totalBytesToSend = 0;
            if (progress.TotalBytesToSend.HasValue)
            {
                totalBytesToSend = progress.TotalBytesToSend.Value;
                TotalBytesToSendField.Text = totalBytesToSend.ToString(CultureInfo.InvariantCulture);
            }
            else
            {
                TotalBytesToSendField.Text = "unknown";
            }

            ulong totalBytesToReceive = 0;
            if (progress.TotalBytesToReceive.HasValue)
            {
                totalBytesToReceive = progress.TotalBytesToReceive.Value;
                TotalBytesToReceiveField.Text = totalBytesToReceive.ToString(CultureInfo.InvariantCulture);
            }
            else
            {
                TotalBytesToReceiveField.Text = "unknown";
            }

            double requestProgress = 0;
            if (progress.Stage == HttpProgressStage.SendingContent && totalBytesToSend > 0)
            {
                    requestProgress = progress.BytesSent * 50 / totalBytesToSend;
            }
            else if (progress.Stage == HttpProgressStage.ReceivingContent)
            {
                // Start with 50 percent, request content was already sent.
                requestProgress += 50;

                if (totalBytesToReceive > 0)
                {
                    requestProgress += progress.BytesReceived * 50 / totalBytesToReceive;
                }
            }
            else
            {
                return;
            }

            RequestProgressBar.Value = requestProgress;
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
