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


//Simplifies call to the MediaProtectionManager
using Windows.Media.Protection;
using Windows.Media.Protection.PlayReady;
using System.Net.Http;
using Windows.Foundation.Collections;
using System.Net.Http.Headers;
using LicenseRequest;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace AdaptiveStreaming
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4 : Page
    {
        private MainPage rootPage;
        AdaptiveMediaSource ams = null; //ams represents the AdaptiveMedaSource used throughout this scenario

        private MediaProtectionManager protectionManager = null;
        MediaProtectionServiceCompletion serviceCompletionNotifier = null;
        private string playReadyLicenseUrl = "";
        private string playReadyChallengeCustomData = "";


        private const int MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED = -2147174251;

        public Scenario4()
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
           
            AdaptiveMediaSourceCreationResult result = await AdaptiveMediaSource.CreateFromUriAsync(uri);
            if (result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                ams = result.MediaSource;
                SetUpProtectionManager(ref m);
                m.SetMediaStreamSource(ams);
            }
            else
            {
                rootPage.NotifyUser("Error creating the AdaptiveMediaSource\n\t" + result.Status, NotifyType.ErrorMessage);
            }
        }

        private void SetUpProtectionManager(ref MediaElement m)
        {
            log("Enter SetUpProtectionManager");

            log("Creating protection system mappings...");
            protectionManager = new MediaProtectionManager();

            protectionManager.ComponentLoadFailed += new ComponentLoadFailedEventHandler(ProtectionManager_ComponentLoadFailed);
            protectionManager.ServiceRequested += new ServiceRequestedEventHandler(ProtectionManager_ServiceRequested);

            //Setup PlayReady as the ProtectionSystem to use by MF. 
            //The code here is mandatory and should be just copied directly over to the app
            Windows.Foundation.Collections.PropertySet cpSystems = new Windows.Foundation.Collections.PropertySet();

            cpSystems.Add("{F4637010-03C3-42CD-B932-B48ADF3A6A54}", "Windows.Media.Protection.PlayReady.PlayReadyWinRTTrustedInput");
            protectionManager.Properties.Add("Windows.Media.Protection.MediaProtectionSystemIdMapping", cpSystems);

            //Use by the media stream source about how to create ITA InitData.
            //See here for more detai: https://msdn.microsoft.com/en-us/library/windows/desktop/aa376846%28v=vs.85%29.aspx
            protectionManager.Properties.Add("Windows.Media.Protection.MediaProtectionSystemId", "{F4637010-03C3-42CD-B932-B48ADF3A6A54}");

            // Setup the container GUID that's in the PPSH box
            protectionManager.Properties.Add("Windows.Media.Protection.MediaProtectionContainerGuid", "{9A04F079-9840-4286-AB92-E65BE0885F95}");

            m.ProtectionManager = protectionManager;

            log("Leave SetUpProtectionManager");
        }

        private async void ProtectionManager_ServiceRequested(MediaProtectionManager sender, ServiceRequestedEventArgs e)
        {
            log("Enter ProtectionManager_ServiceRequested");

            if (e.Request is PlayReadyIndividualizationServiceRequest)
            {
                PlayReadyIndividualizationServiceRequest IndivRequest = e.Request as PlayReadyIndividualizationServiceRequest;
                bool bResultIndiv = await ReactiveIndivRequest(IndivRequest, e.Completion);
            }
            else if (e.Request is PlayReadyLicenseAcquisitionServiceRequest)
            {
                PlayReadyLicenseAcquisitionServiceRequest licenseRequest = e.Request as PlayReadyLicenseAcquisitionServiceRequest;
                LicenseAcquisitionRequest(licenseRequest, e.Completion, playReadyLicenseUrl, playReadyChallengeCustomData);
            }

            log("Leave ProtectionManager_ServiceRequested");
        }

        private void ProtectionManager_ComponentLoadFailed(MediaProtectionManager sender, ComponentLoadFailedEventArgs e)
        {
            log("Enter ProtectionManager_ComponentLoadFailed");
            log(e.Information.ToString());

            //  List the failing components - RevocationAndRenewalInformation
            for (int i = 0; i < e.Information.Items.Count; i++)
            {
                log(e.Information.Items[i].Name + "\nReasons=0x" + e.Information.Items[i].Reasons + "\n"
                                                    + "Renewal Id=" + e.Information.Items[i].RenewalId);

            }
            e.Completion.Complete(false);

            log("Leave ProtectionManager_ComponentLoadFailed");
        }

        /// <summary>
        /// Invoked to acquire the PlayReady License
        /// </summary>
        async void LicenseAcquisitionRequest(PlayReadyLicenseAcquisitionServiceRequest licenseRequest, MediaProtectionServiceCompletion CompletionNotifier, string Url, string ChallengeCustomData)
        {
            bool bResult = false;
            string ExceptionMessage = string.Empty;

            try
            {
                if (!string.IsNullOrEmpty(Url))
                {
                    log("ProtectionManager PlayReady Manual License Acquisition Service Request in progress - URL: " + Url);

                    if (!string.IsNullOrEmpty(ChallengeCustomData))
                    {
                        System.Text.UTF8Encoding encoding = new System.Text.UTF8Encoding();
                        byte[] b = encoding.GetBytes(ChallengeCustomData);
                        licenseRequest.ChallengeCustomData = Convert.ToBase64String(b, 0, b.Length);
                    }

                    PlayReadySoapMessage soapMessage = licenseRequest.GenerateManualEnablingChallenge();

                    byte[] messageBytes = soapMessage.GetMessageBody();
                    HttpContent httpContent = new ByteArrayContent(messageBytes);

                    IPropertySet propertySetHeaders = soapMessage.MessageHeaders;
                    foreach (string strHeaderName in propertySetHeaders.Keys)
                    {
                        string strHeaderValue = propertySetHeaders[strHeaderName].ToString();

                        // The Add method throws an ArgumentException try to set protected headers like "Content-Type"
                        // so set it via "ContentType" property
                        if (strHeaderName.Equals("Content-Type", StringComparison.OrdinalIgnoreCase))
                            httpContent.Headers.ContentType = MediaTypeHeaderValue.Parse(strHeaderValue);
                        else
                            httpContent.Headers.Add(strHeaderName.ToString(), strHeaderValue);
                    }
                    CommonLicenseRequest licenseAcquision = new CommonLicenseRequest();
                    HttpContent responseHttpContent = await licenseAcquision.AcquireLicense(new Uri(Url), httpContent);
                    if (responseHttpContent != null)
                    {
                        Exception exResult = licenseRequest.ProcessManualEnablingResponse(await responseHttpContent.ReadAsByteArrayAsync());
                        if (exResult != null)
                        {
                            throw exResult;
                        }
                        bResult = true;
                    }
                    else
                        ExceptionMessage = licenseAcquision.GetLastErrorMessage();
                }
                else
                {
                    log ("ProtectionManager PlayReady License Acquisition Service Request in progress - URL: " + licenseRequest.Uri.ToString());
                    await licenseRequest.BeginServiceRequest();
                    bResult = true;
                }
            }
            catch (Exception e)
            {
                ExceptionMessage = e.Message;
            }

            if (bResult == true)
                log(!string.IsNullOrEmpty(Url) ? "ProtectionManager Manual PlayReady License Acquisition Service Request successful" :
                    "ProtectionManager PlayReady License Acquisition Service Request successful");
            else
                log(!string.IsNullOrEmpty(Url) ? "ProtectionManager Manual PlayReady License Acquisition Service Request failed: " + ExceptionMessage :
                    "ProtectionManager PlayReady License Acquisition Service Request failed: " + ExceptionMessage);
            CompletionNotifier.Complete(bResult);
        }
        /// <summary>
        /// Proactive Individualization Request 
        /// </summary>
        async void ProActiveIndivRequest()
        {
            PlayReadyIndividualizationServiceRequest indivRequest = new PlayReadyIndividualizationServiceRequest();
            log("ProtectionManager PlayReady ProActive Individualization Service Request in progress...");
            bool bResultIndiv = await ReactiveIndivRequest(indivRequest, null);
            if (bResultIndiv == true)
                log("ProtectionManager PlayReady ProActive Individualization Service Request successful");
            else
                log("ProtectionManager PlayReady ProActive Individualization Service Request failed");

        }
        /// <summary>
        /// Invoked to send the Individualization Request 
        /// </summary>
        async Task<bool> ReactiveIndivRequest(PlayReadyIndividualizationServiceRequest IndivRequest, MediaProtectionServiceCompletion CompletionNotifier)
        {
            bool bResult = false;
            Exception exception = null;
            log("ProtectionManager PlayReady Individualization Service Request in progress...");
            try
            {
                await IndivRequest.BeginServiceRequest();
            }
            catch (Exception ex)
            {
                exception = ex;
            }
            finally
            {
                if (exception == null)
                {
                    bResult = true;
                }
                else
                {
                    COMException comException = exception as COMException;
                    if (comException != null && comException.HResult == MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED)
                    {
                        IndivRequest.NextServiceRequest();
                    }
                }
            }
            if (bResult == true)
                log("ProtectionManager PlayReady Individualization Service Request successful");
            else
                log("ProtectionManager PlayReady Individualization Service Request failed");
            if (CompletionNotifier != null) CompletionNotifier.Complete(bResult);
            return bResult;

        }
    }
}
