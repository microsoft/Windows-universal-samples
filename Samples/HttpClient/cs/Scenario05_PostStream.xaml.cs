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
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;

namespace SDKTemplate
{
    public sealed partial class Scenario05_PostStream : Page
    {
        MainPage rootPage = MainPage.Current;

        private HttpClient httpClient;
        private CancellationTokenSource cts;

        public Scenario05_PostStream()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            httpClient = Helpers.CreateHttpClientWithPlugIn();
            cts = new CancellationTokenSource();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            cts.Cancel();
            cts.Dispose();
            httpClient.Dispose();
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

            const int contentLength = 1000;
            Stream stream = GenerateSampleStream(contentLength);
            HttpStreamContent streamContent = new HttpStreamContent(stream.AsInputStream());

            HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Post, resourceUri);
            request.Content = streamContent;

            // This sample uses a "try" in order to support TaskCanceledException.
            // If you don't need to support cancellation, then the "try" is not needed.
            try
            {
                // Do an asynchronous POST.
                HttpRequestResult result = await httpClient.TrySendRequestAsync(request).AsTask(cts.Token);

                if (result.Succeeded)
                {
                    await Helpers.DisplayTextResultAsync(result.ResponseMessage, OutputField, cts.Token);

                    rootPage.NotifyUser("Completed", NotifyType.StatusMessage);
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

        private static MemoryStream GenerateSampleStream(int size)
        {
            // Generate sample data.
            byte[] subData = new byte[size];
            for (int i = 0; i < subData.Length; i++)
            {
                subData[i] = 64; // '@'
            }

            return new MemoryStream(subData);
        }
    }
}
