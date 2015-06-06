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
using Windows.Media.Streaming.Adaptive;
using Windows.UI.Core;

namespace AdaptiveStreaming
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        private MainPage rootPage;
        AdaptiveMediaSource ams = null; //ams represents the AdaptiveMedaSource used throughout this sample
        Windows.Web.Http.HttpClient httpClient = null; //used to customize download requests

        public Scenario3()
        {
            this.InitializeComponent();
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }
        async private void log(string s)
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
            {
                TextBlock text = new TextBlock();
                text.Text = s;
                text.TextWrapping = TextWrapping.WrapWholeWords;
                stkOutput.Children.Add(text);
            }));
        }

        private void btnPlay_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(txtInputURL.Text))
            {
                rootPage.NotifyUser("Specify a URI to play", NotifyType.ErrorMessage);
                return;
            }

            InitializeAdaptiveMediaSource(new System.Uri(txtInputURL.Text), mePlayer);
        }

        async private void InitializeAdaptiveMediaSource(System.Uri uri, MediaElement m)
        {
            httpClient = new Windows.Web.Http.HttpClient();
            httpClient.DefaultRequestHeaders.TryAppendWithoutValidation("X-CustomHeader", "This is a custom header");

            AdaptiveMediaSourceCreationResult result = await AdaptiveMediaSource.CreateFromUriAsync(uri, httpClient);
            if (result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                ams = result.MediaSource;
                m.SetMediaStreamSource(ams);

                //Register for download requested event
                ams.DownloadRequested += DownloadRequested;

                //Register for download success and failure events
                ams.DownloadCompleted += DownloadCompleted;
                ams.DownloadFailed += DownloadFailed;
            }
            else
            {
                rootPage.NotifyUser("Error creating the AdaptiveMediaSource\n\t" + result.Status, NotifyType.ErrorMessage);
            }
        }

        private void DownloadRequested(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadRequestedEventArgs args)
        {
            // rewrite key URIs to replace http:// with https://
            if (args.ResourceType == AdaptiveMediaSourceResourceType.Key)
            {
                string originalUri = args.ResourceUri.ToString();
                string secureUri = originalUri.Replace("http:", "https:");

                // override the URI by setting property on the result sub object
                args.Result.ResourceUri = new Uri(secureUri);
            }
        }

    private void DownloadCompleted(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadCompletedEventArgs args)
        {
            log("Download Completed. Resource Type: " + args.ResourceType + " Resource URI: " + args.ResourceUri);
        }
        private void DownloadFailed(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadFailedEventArgs args)
        {
            log("Download Failured. Resource Type: " + args.ResourceType + " Resource URI: " + args.ResourceUri + "\nHTTP Response:\n" + args.HttpResponseMessage);
        }
    }
}
