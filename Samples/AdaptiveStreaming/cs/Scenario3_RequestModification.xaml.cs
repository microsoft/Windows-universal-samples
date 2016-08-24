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

using SDKTemplate.Helpers;
using SDKTemplate.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Media.Core;
using Windows.Media.Protection;
using Windows.Media.Streaming.Adaptive;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;
using Windows.Web.Http.Filters;
using Windows.Web.Http.Headers;

namespace SDKTemplate
{
    /// See the README.md for discussion of this scenario.
    ///
    /// Note: We register but do not unregister event handlers in this scenario, see the EventHandler
    ///       scenario for patterns that can be used to clean up.
    public sealed partial class Scenario3_RequestModification : Page
    {
        public Scenario3_RequestModification()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ctsForAppHttpClientForKeys.Cancel(); // Cancel any HTTP requests the app was making.
            adaptiveMS = null;
            var mp = mediaPlayerElement.MediaPlayer;
            if (mp != null)
            {
                mp.DisposeSource();
                mediaPlayerElement.SetMediaPlayer(null);
                mp.Dispose();
            }

            // We will clean up HDCP explicitly, this ensures the OutputProtectionManager
            // does not have to continue to impose our policy until the Garbage Collector
            // cleans the object.
            hdcpSession?.Dispose();
            hdcpSession = null;
        }

        AdaptiveContentModel adaptiveContentModel;
        AddAuthorizationHeaderFilter httpClientFilter;
        HdcpSession hdcpSession;
        AdaptiveMediaSource adaptiveMS;


        private void Page_OnLoaded(object sender, RoutedEventArgs e)
        {
            // We enforce HDCP in this scenario that uses encryption but not PlayReady.
            hdcpSession = new HdcpSession();

            // The protection level may change if screens are plugged or unplugged from the system or
            // if explicitly set in the radio buttons in the UI.
            hdcpSession.ProtectionChanged += (HdcpSession session, object args) =>
            {
                // After a change, impose a maximum bitrate based on the actual protection level.
                HdcpProtection? protection = session.GetEffectiveProtection();
                SetMaxBitrateForProtectionLevel(protection, adaptiveMS);
            };

            // Choose a default content, and tell the user that some content IDs
            // require an authorization mode. Filter out the PlayReady content
            // because this scenario does not support PlayReady.
            SelectedContent.ItemsSource = MainPage.ContentManagementSystemStub.Where(model => !model.PlayReady);
            SelectedContent.SelectedItem = MainPage.FindContentById(13);

            // Initialize tokenMethod based on the default selected radio button.
            var defaultRadioButton = AzureAuthorizationMethodPanel.Children.OfType<RadioButton>().First(button => button.IsChecked.Value);
            Enum.TryParse((string)defaultRadioButton.Tag, out tokenMethod);

            Log("Content Id 13 and 14 require that you choose an authorization method.");
        }

        private async void Load_Click(object sender, RoutedEventArgs e)
        {
            adaptiveContentModel = (AdaptiveContentModel)SelectedContent.SelectedItem;

            if (tokenMethod == AzureKeyAcquisitionMethod.AuthorizationHeader)
            {
                // Use an IHttpFilter to identify key request URIs and insert an Authorization header with the Bearer token.
                var baseProtocolFilter = new HttpBaseProtocolFilter();
                httpClientFilter = new AddAuthorizationHeaderFilter(baseProtocolFilter);
                httpClientFilter.AuthorizationHeader = new HttpCredentialsHeaderValue("Bearer", adaptiveContentModel.AesToken);
                var httpClient = new HttpClient(httpClientFilter);

                // Here is where you can add any required custom CDN headers.
                httpClient.DefaultRequestHeaders.Append("X-HeaderKey", "HeaderValue");
                // NOTE: It is not recommended to set Authorization headers needed for key request on the
                // default headers of the HttpClient, as these will also be used on non-HTTPS calls
                // for media segments and manifests -- and thus will be easily visible.

                await LoadSourceFromUriAsync(adaptiveContentModel.ManifestUri, httpClient);
            }
            else
            {
                await LoadSourceFromUriAsync(adaptiveContentModel.ManifestUri);
            }

            // On small screens, hide the description text to make room for the video.
            DescriptionText.Visibility = (ActualHeight < 650) ? Visibility.Collapsed : Visibility.Visible;
        }

        private async Task LoadSourceFromUriAsync(Uri uri, HttpClient httpClient = null)
        {
            mediaPlayerElement.MediaPlayer?.DisposeSource();

            AdaptiveMediaSourceCreationResult result = null;
            if (httpClient != null)
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri, httpClient);
            }
            else
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri);
            }

            if (result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                adaptiveMS = result.MediaSource;

                // Register for events before setting the IMediaPlaybackSource
                RegisterForAdaptiveMediaSourceEvents(adaptiveMS);

                // Now that we have bitrates, attempt to cap them based on HdcpProtection.
                HdcpProtection? protection = hdcpSession.GetEffectiveProtection();
                SetMaxBitrateForProtectionLevel(protection, adaptiveMS);

                MediaSource source = MediaSource.CreateFromAdaptiveMediaSource(adaptiveMS);

                // Note that in this sample with very few event handlers, we are creating neither
                // a MediaPlayer nor a MediaPlaybackItem. The MediaPlayer will be created implicitly
                // by the mpElement, which will also manage its lifetime.

                mediaPlayerElement.Source = source;
                // You can now access mpElement.MediaPlayer, but it is too late to register
                // to handle its MediaOpened event.
            }
            else
            {
                Log($"Error creating the AdaptiveMediaSource: {result.Status}");
            }
        }

        #region AdaptiveMediaSource Event Handlers

        private void RegisterForAdaptiveMediaSourceEvents(AdaptiveMediaSource aMS)
        {
            aMS.DownloadRequested += DownloadRequested;
            aMS.DownloadFailed += DownloadFailed;
        }

        private async void DownloadRequested(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadRequestedEventArgs args)
        {
            if (args.ResourceType == AdaptiveMediaSourceResourceType.Key)
            {
                switch (tokenMethod)
                {
                    case AzureKeyAcquisitionMethod.None:
                        break;
                    case AzureKeyAcquisitionMethod.AuthorizationHeader:
                        // By updating the IHttpFilter KeyHost property here, we ensure it's up to date
                        // before the network request is made.  It is the IHttpFilter that will insert
                        // the Authorization header.
                        if (httpClientFilter != null)
                        {
                            httpClientFilter.KeyHost = args.ResourceUri.Host;
                        }
                        break;
                    case AzureKeyAcquisitionMethod.UrlQueryParameter:
                        ModifyKeyRequestUri(args);
                        break;
                    case AzureKeyAcquisitionMethod.ApplicationDownloaded:
                        await AppDownloadedKeyRequest(args);
                        break;
                    default:
                        break;
                }
            }
        }

        private void ModifyKeyRequestUri(AdaptiveMediaSourceDownloadRequestedEventArgs args)
        {
            if (adaptiveContentModel == null)
            {
                return;
            }

            // This pattern can be used to modify Uris, for example:
            //   To a redirect traffic to a secondary endpoint
            //   To change an segment request into a byte-range Uri into another resource

            // Add the Bearer token to the Uri and modify the args.Result.ResourceUri
            string armoredAuthToken = System.Net.WebUtility.UrlEncode("Bearer=" + adaptiveContentModel.AesToken);
            string uriWithTokenParameter = $"{args.ResourceUri.AbsoluteUri}&token={armoredAuthToken}";
            args.Result.ResourceUri = new Uri(uriWithTokenParameter);
        }

        private async Task AppDownloadedKeyRequest(AdaptiveMediaSourceDownloadRequestedEventArgs args)
        {
            if (adaptiveContentModel == null)
            {
                return;
            }

            // For AzureKeyAcquisitionMethod.ApplicationDownloaded we do the following:
            //   Call .GetDeferral() to allow asynchronous work to be performed
            //   Get the requested network resource using app code
            //   Set .Result.InputStream or .Result.Buffer with the response data
            //   Complete the deferral to indicate that we are done.
            // With this pattern, the app has complete control over any downloaded content.

            // Obtain a deferral so we can perform asynchronous operations.
            var deferral = args.GetDeferral();
            try
            {
                var appHttpClientForKeys = new HttpClient();
                appHttpClientForKeys.DefaultRequestHeaders.Authorization = new HttpCredentialsHeaderValue("Bearer", adaptiveContentModel.AesToken);

                HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, args.ResourceUri);

                HttpResponseMessage response = await appHttpClientForKeys.SendRequestAsync(
                    request, HttpCompletionOption.ResponseHeadersRead).AsTask(ctsForAppHttpClientForKeys.Token);

                if (response.IsSuccessStatusCode)
                {
                    args.Result.InputStream = await response.Content.ReadAsInputStreamAsync();
                    // Alternatively, we could use:
                    // args.Result.Buffer = await response.Content.ReadAsBufferAsync();
                }
                else
                {
                    // The app code failed. Report this by setting the args.Result.ExtendedStatus.
                    // This will ensure that the AdaptiveMediaSource does not attempt to download the resource
                    // itself, and instead treats the download as having failed.
                    switch (response.StatusCode)
                    {
                        case HttpStatusCode.Unauthorized:
                            // HTTP_E_STATUS_DENIED
                            args.Result.ExtendedStatus = 0x80190191;
                            break;
                        case HttpStatusCode.NotFound:
                            // HTTP_E_STATUS_NOT_FOUND
                            args.Result.ExtendedStatus = 0x80190194;
                            break;
                        default:
                            // HTTP_E_STATUS_UNEXPECTED
                            args.Result.ExtendedStatus = 0x80190001;
                            break;
                    }
                    Log($"Key Download Failed: {response.StatusCode} {args.ResourceUri}");
                }
            }
            catch (TaskCanceledException)
            {
                Log($"Request canceled: {args.ResourceUri}");

            }
            catch (Exception e)
            {
                Log($"Key Download Failed: {e.Message} {args.ResourceUri}");
            }
            finally
            {
                // Complete the deferral when done.
                deferral.Complete();
            }
        }

        private void DownloadFailed(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadFailedEventArgs args)
        {
            Log($"DownloadFailed: {args.HttpResponseMessage}, {args.ResourceType}, {args.ResourceUri}");
        }
        #endregion

        #region AzureKeyAcquisitionMethod
        enum AzureKeyAcquisitionMethod
        {
            None,
            AuthorizationHeader,
            UrlQueryParameter,
            ApplicationDownloaded,
        }
        private AzureKeyAcquisitionMethod tokenMethod;
        private CancellationTokenSource ctsForAppHttpClientForKeys = new CancellationTokenSource();

        private void AzureMethodSelected_Click(object sender, RoutedEventArgs e)
        {
            AzureKeyAcquisitionMethod selectedMethod;
            string selected = (sender as RadioButton).Tag.ToString();
            if (Enum.TryParse(selected, out selectedMethod))
            {
                tokenMethod = selectedMethod;
            }
        }
        #endregion

        #region HdcpDesiredMinimumProtection

        /// <summary>
        /// Handles the radio button selections from the UI and imposes that minimum desired protection
        /// </summary>
        private async void HdcpDesiredMinimumProtection_Click(object sender, RoutedEventArgs e)
        {
            HdcpProtection desiredMinimumProtection = HdcpProtection.Off;
            string selected = (sender as RadioButton).Tag.ToString();
            if (Enum.TryParse(selected, out desiredMinimumProtection))
            {
                var result = await hdcpSession.SetDesiredMinProtectionAsync(desiredMinimumProtection);
                HdcpProtection? actualProtection = hdcpSession.GetEffectiveProtection();
                if (result != HdcpSetProtectionResult.Success)
                {
                    Log($"ERROR: Unable to set HdcpProtection.{desiredMinimumProtection}, Error: {result}, Actual HDCP: {actualProtection}");
                }
                else
                {
                    Log($"HDCP Requested Minimum: {desiredMinimumProtection}, Actual: {actualProtection}");
                }
            }
        }

        /// <summary>
        /// Enforces a fictitious content publisher's rules for bandwidth maximums based on HDCP protection levels.
        /// </summary>
        /// <param name="protection">Protection level to use when imposing bandwidth restriction.</param>
        /// <param name="ams">AdaptiveMediaSource on which to impose restrictions</param>
        private void SetMaxBitrateForProtectionLevel(HdcpProtection? protection, AdaptiveMediaSource ams)
        {
            EffectiveHdcpProtectionText.Text = protection.ToString();

            if (ams != null && ams.AvailableBitrates.Count > 1)
            {
                // Get a sorted list of available bitrates.
                var bitrates = new List<uint>(ams.AvailableBitrates);
                bitrates.Sort();

                // Apply maximum bitrate policy based on a fictitious content publisher's rules.
                switch (protection)
                {
                    case HdcpProtection.OnWithTypeEnforcement:
                        // Allow full bitrate.
                        ams.DesiredMaxBitrate = bitrates[bitrates.Count - 1];
                        DesiredMaxBitrateText.Text = "full bitrate allowed";
                        break;
                    case HdcpProtection.On:
                        // When there is no HDCP Type 1, make the highest bitrate unavailable.
                        ams.DesiredMaxBitrate = bitrates[bitrates.Count - 2];
                        DesiredMaxBitrateText.Text = "highest bitrate is unavailable";
                        break;
                    case HdcpProtection.Off:
                    case null:
                    default:
                        // When there is no HDCP at all (Off), or the system is still trying to determine what
                        // HDCP protection level to apply (null), then make only the lowest bitrate available.
                        ams.DesiredMaxBitrate = bitrates[0];
                        DesiredMaxBitrateText.Text = "lowest bitrate only";
                        break;
                }
                Log($"Imposed DesiredMaxBitrate={ams.DesiredMaxBitrate} for HdcpProtection.{protection}");
            }
        }
        #endregion

        #region Utilities
        private void Log(string message)
        {
            LoggerControl.Log(message);
        }
        #endregion
    }

    #region IHttpFilter to add an authorization header

    public class AddAuthorizationHeaderFilter : IHttpFilter
    {
        public AddAuthorizationHeaderFilter(IHttpFilter innerFilter)
        {
            if (innerFilter == null)
            {
                throw new ArgumentException("innerFilter cannot be null.");
            }
            this.innerFilter = innerFilter;
        }

        internal HttpCredentialsHeaderValue AuthorizationHeader;
        private IHttpFilter innerFilter;

        // NOTE: In production, an app might need logic for several failover key-delivery hosts.
        public string KeyHost { get; set; }

        public IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> SendRequestAsync(HttpRequestMessage request)
        {
            bool isKeyRequest = String.Equals(request.RequestUri.Host, KeyHost, StringComparison.OrdinalIgnoreCase);
            if (isKeyRequest && AuthorizationHeader != null)
            {
                request.Headers.Authorization = AuthorizationHeader;
            }

            return innerFilter.SendRequestAsync(request);
        }

        public void Dispose()
        {
        }
    }
    #endregion
}
