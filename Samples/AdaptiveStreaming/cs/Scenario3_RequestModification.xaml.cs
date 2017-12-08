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
using SDKTemplate.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Media.Protection;
using Windows.Media.Streaming.Adaptive;
using Windows.UI.Core;
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

        private AzureKeyAcquisitionMethod tokenMethod;
        private CancellationTokenSource ctsForAppHttpClientForKeys = new CancellationTokenSource();

        public Scenario3_RequestModification()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // Cancel any HTTP requests the app was making:
            ctsForAppHttpClientForKeys.Cancel();

            // Release handles on various media objects to ensure a quick clean-up
            ContentSelectorControl.MediaPlaybackItem = null;
            var mediaPlayer = mediaPlayerElement.MediaPlayer;
            if (mediaPlayer != null)
            {
                mediaPlayerLogger?.Dispose();
                mediaPlayerLogger = null;

                UnregisterHandlers(mediaPlayer);

                mediaPlayer.DisposeSource();
                mediaPlayerElement.SetMediaPlayer(null);
                mediaPlayer.Dispose();
            }

            // We clean up HDCP explicitly. Otherwise, the OutputProtectionManager
            // will continue to impose our policy until it is finalized by the
            // garbage collector.
            hdcpSession?.Dispose();
            hdcpSession = null;
        }

        private void UnregisterHandlers(MediaPlayer mediaPlayer)
        {
            AdaptiveMediaSource adaptiveMediaSource = null;
            MediaPlaybackItem mpItem = mediaPlayer.Source as MediaPlaybackItem;
            if (mpItem != null)
            {
                adaptiveMediaSource = mpItem.Source.AdaptiveMediaSource;
            }
            MediaSource source = mediaPlayer.Source as MediaSource;
            if (source != null)
            {
                adaptiveMediaSource = source.AdaptiveMediaSource;
            }

            mediaPlaybackItemLogger?.Dispose();
            mediaPlaybackItemLogger = null;

            mediaSourceLogger?.Dispose();
            mediaSourceLogger = null;

            adaptiveMediaSourceLogger?.Dispose();
            adaptiveMediaSourceLogger = null;

            UnregisterForAdaptiveMediaSourceEvents(adaptiveMediaSource);
        }

        AddAuthorizationHeaderFilter httpClientFilter;
        HdcpSession hdcpSession;
        AdaptiveMediaSource adaptiveMediaSource;

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
                SetMaxBitrateForProtectionLevel(protection, adaptiveMediaSource);
            };

            // Explicitly create the instance of MediaPlayer if you need to register for its events
            // (like MediaOpened / MediaFailed) prior to setting an IMediaPlaybackSource.
            var mediaPlayer = new MediaPlayer();

            // We use a helper class that logs all the events for the MediaPlayer:
            mediaPlayerLogger = new MediaPlayerLogger(LoggerControl, mediaPlayer);

            mediaPlayerElement.SetMediaPlayer(mediaPlayer);

            ContentSelectorControl.Initialize(
                mediaPlayer,
                MainPage.ContentManagementSystemStub.Where(m =>!m.PlayReady),
                null,
                LoggerControl,
                LoadSourceFromUriAsync);

            ContentSelectorControl.HideLoadUri(); // Avoid free text URIs for this scenario.

            // Initialize tokenMethod based on the default selected radio button.
            var defaultRadioButton = AzureAuthorizationMethodPanel.Children.OfType<RadioButton>().First(button => button.IsChecked.Value);
            Enum.TryParse((string)defaultRadioButton.Tag, out tokenMethod);

            Log("Content Id 13 and 14 require that you choose an authorization method.");
            ContentSelectorControl.SetSelectedModel(MainPage.ContentManagementSystemStub.Where(m => m.Id == 13).FirstOrDefault());
        }


        #region Content Loading

        private async Task<MediaPlaybackItem> LoadSourceFromUriAsync(Uri uri, HttpClient httpClient = null)
        {
            UnregisterHandlers(mediaPlayerElement.MediaPlayer);
            mediaPlayerElement.MediaPlayer?.DisposeSource();

            if (tokenMethod == AzureKeyAcquisitionMethod.AuthorizationHeader)
            {
                if (ContentSelectorControl.SelectedModel == null)
                {
                    return null;
                }
                // Use an IHttpFilter to identify key request URIs and insert an Authorization header with the Bearer token.
                var baseProtocolFilter = new HttpBaseProtocolFilter();
                baseProtocolFilter.CacheControl.WriteBehavior = HttpCacheWriteBehavior.NoCache;  // Always set WriteBehavior = NoCache
                httpClientFilter = new AddAuthorizationHeaderFilter(baseProtocolFilter);
                httpClientFilter.AuthorizationHeader = new HttpCredentialsHeaderValue("Bearer", ContentSelectorControl.SelectedModel.AesToken);
                httpClient = new HttpClient(httpClientFilter);

                // Here is where you can add any required custom CDN headers.
                httpClient.DefaultRequestHeaders.Append("X-HeaderKey", "HeaderValue");
                // NOTE: It is not recommended to set Authorization headers needed for key request on the
                // default headers of the HttpClient, as these will also be used on non-HTTPS calls
                // for media segments and manifests -- and thus will be easily visible.
            }

            AdaptiveMediaSourceCreationResult result = null;
            if (httpClient != null)
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri, httpClient);
            }
            else
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri);
            }

            MediaSource source;
            if (result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                adaptiveMediaSource = result.MediaSource;

                // We use a helper class that logs all the events for the AdaptiveMediaSource:
                adaptiveMediaSourceLogger = new AdaptiveMediaSourceLogger(LoggerControl, adaptiveMediaSource);

                // In addition to logging, we use the callbacks to update some UI elements in this scenario:
                RegisterForAdaptiveMediaSourceEvents(adaptiveMediaSource);

                // At this point, we have read the manifest of the media source, and all bitrates are known.
                // Now that we have bitrates, attempt to cap them based on HdcpProtection.
                HdcpProtection? protection = hdcpSession.GetEffectiveProtection();
                SetMaxBitrateForProtectionLevel(protection, adaptiveMediaSource);

                source = MediaSource.CreateFromAdaptiveMediaSource(adaptiveMediaSource);

            }
            else
            {
                Log($"Error creating the AdaptiveMediaSource. Status: {result.Status}, ExtendedError.Message: {result.ExtendedError.Message}, ExtendedError.HResult: {result.ExtendedError.HResult.ToString("X8")}");
                return null;
            }

            // We use a helper class that logs all the events for the MediaSource:
            mediaSourceLogger = new MediaSourceLogger(LoggerControl, source);

            // Save the original Uri.
            source.CustomProperties["uri"] = uri.ToString();

            // You're likely to put a content tracking id into the CustomProperties.
            source.CustomProperties["contentId"] = Guid.NewGuid().ToString();

            var mpItem = new MediaPlaybackItem(source);

            // We use a helper class that logs all the events for the MediaPlaybackItem:
            mediaPlaybackItemLogger = new MediaPlaybackItemLogger(LoggerControl, mpItem);

            HideDescriptionOnSmallScreen();

            return mpItem;
        }

        private async void HideDescriptionOnSmallScreen()
        {
            // On small screens, hide the description text to make room for the video.
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                DescriptionText.Visibility = (ActualHeight < 500) ? Visibility.Collapsed : Visibility.Visible;
            });
        }
        #endregion


        #region AdaptiveMediaSource Event Handlers

        private void RegisterForAdaptiveMediaSourceEvents(AdaptiveMediaSource adaptiveMediaSource)
        {
            adaptiveMediaSource.DownloadRequested += DownloadRequested;
            adaptiveMediaSource.DownloadFailed += DownloadFailed;
        }

        private void UnregisterForAdaptiveMediaSourceEvents(AdaptiveMediaSource adaptiveMediaSource)
        {
            if (adaptiveMediaSource != null)
            {
                adaptiveMediaSource.DownloadRequested -= DownloadRequested;
                adaptiveMediaSource.DownloadFailed -= DownloadFailed;
            }
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
            if (ContentSelectorControl.SelectedModel == null)
            {
                return;
            }

            // This pattern can be used to modify Uris, for example:
            //   To a redirect traffic to a secondary endpoint
            //   To change an segment request into a byte-range Uri into another resource

            // Add the Bearer token to the Uri and modify the args.Result.ResourceUri
            string armoredAuthToken = System.Net.WebUtility.UrlEncode("Bearer=" + ContentSelectorControl.SelectedModel.AesToken);
            string uriWithTokenParameter = $"{args.ResourceUri.AbsoluteUri}&token={armoredAuthToken}";
            args.Result.ResourceUri = new Uri(uriWithTokenParameter);
        }

        private async Task AppDownloadedKeyRequest(AdaptiveMediaSourceDownloadRequestedEventArgs args)
        {
            if (ContentSelectorControl.SelectedModel == null)
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
                appHttpClientForKeys.DefaultRequestHeaders.Authorization = new HttpCredentialsHeaderValue("Bearer", ContentSelectorControl.SelectedModel.AesToken);

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
        private async void SetMaxBitrateForProtectionLevel(HdcpProtection? protection, AdaptiveMediaSource ams)
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
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
            });
        }
        #endregion


        #region Utilities
        private void Log(string message)
        {
            LoggerControl.Log(message);
        }
        MediaPlayerLogger mediaPlayerLogger;
        MediaSourceLogger mediaSourceLogger;
        MediaPlaybackItemLogger mediaPlaybackItemLogger;
        AdaptiveMediaSourceLogger adaptiveMediaSourceLogger;
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
