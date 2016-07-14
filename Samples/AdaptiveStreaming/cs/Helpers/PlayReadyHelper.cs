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
using System.Net.Http;
using System.Net.Http.Headers;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation.Collections;
using Windows.Media.Playback;
using Windows.Media.Protection;
using Windows.Media.Protection.PlayReady;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate.Helpers
{

    /// <summary>
    /// Encapsulates all the required calls to setup a MediaProtectionManger within a MediaPlayer
    /// for typical PlayReady scenarios.
    /// 
    /// It is left to the user of these samples to explore other PlayReady rights management scenarios.
    /// 
    /// </summary>
    public class PlayReadyHelper
    {
        public PlayReadyHelper(Logger logger = null)
        {
            this.logger = logger;
        }
        private Logger logger;


        private const int MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED = -2147174251;
        private KeyValuePair<string, object> PlayReadyWinRTTrustedInput = new KeyValuePair<string, object>("{F4637010-03C3-42CD-B932-B48ADF3A6A54}", "Windows.Media.Protection.PlayReady.PlayReadyWinRTTrustedInput" );
        private KeyValuePair<string, object> MediaProtectionSystemId = new KeyValuePair<string, object>("Windows.Media.Protection.MediaProtectionSystemId", "{F4637010-03C3-42CD-B932-B48ADF3A6A54}");
        private KeyValuePair<string, object> MediaProtectionContainerGuid = new KeyValuePair<string, object>("Windows.Media.Protection.MediaProtectionContainerGuid", "{9A04F079-9840-4286-AB92-E65BE0885F95}");
        private const string MediaProtectionSystemIdMapping = "Windows.Media.Protection.MediaProtectionSystemIdMapping";

        // NOTE: For custom scenarios, you may need these:
        public string playReadyLicenseUrl = "";
        public string playReadyChallengeCustomData = "";


        private void Log(string message)
        {
            if (logger != null)
            {
                logger.Log(message);
            }
            else
            {
                string logEntry = $"{DateTime.Now:HH:mm:ss.fff} - {message}";
                System.Diagnostics.Debug.WriteLine(logEntry);
            }
        }

        public void SetUpProtectionManager(MediaPlayer mediaPlayer)
        {
            Log("Enter SetUpProtectionManager");

            if(mediaPlayer == null)
                throw new ArgumentException("SetUpProtectionManager was passed a null MediaPlayer");
            
            Log("Creating protection system mappings...");
            var protectionManager = new MediaProtectionManager();

            protectionManager.ComponentLoadFailed += new ComponentLoadFailedEventHandler(ProtectionManager_ComponentLoadFailed);
            protectionManager.ServiceRequested += new ServiceRequestedEventHandler(ProtectionManager_ServiceRequested);

            // The code here is mandatory and should be just copied directly over to the app
            // Ref: https://msdn.microsoft.com/en-us/library/windows/desktop/aa376846%28v=vs.85%29.aspx

            // Setup PlayReady as the ProtectionSystem to use by mediaFoundation: 
            var contentProtectionSystems = new PropertySet();
            contentProtectionSystems.Add(PlayReadyWinRTTrustedInput);
            protectionManager.Properties.Add(MediaProtectionSystemIdMapping, contentProtectionSystems);
            protectionManager.Properties.Add(MediaProtectionSystemId);
            protectionManager.Properties.Add(MediaProtectionContainerGuid);

            mediaPlayer.ProtectionManager = protectionManager;

            Log("Leave SetUpProtectionManager");
        }

        private async void ProtectionManager_ServiceRequested(MediaProtectionManager sender, ServiceRequestedEventArgs e)
        {
            Log("Enter ProtectionManager_ServiceRequested");

            if (e.Request is PlayReadyIndividualizationServiceRequest)
            {
                var individualizationRequest = e.Request as PlayReadyIndividualizationServiceRequest;
                bool bResultIndiv = await ReactiveIndividualizationRequestAsync(individualizationRequest, e.Completion);
            }
            else if (e.Request is PlayReadyLicenseAcquisitionServiceRequest)
            {
                var licenseRequest = e.Request as PlayReadyLicenseAcquisitionServiceRequest;
                LicenseAcquisitionRequest(licenseRequest, e.Completion, playReadyLicenseUrl, playReadyChallengeCustomData);
            }

            Log("Leave ProtectionManager_ServiceRequested");
        }

        private void ProtectionManager_ComponentLoadFailed(MediaProtectionManager sender, ComponentLoadFailedEventArgs e)
        {
            Log("Enter ProtectionManager_ComponentLoadFailed");
            Log(e.Information.ToString());

            // List the failing components - RevocationAndRenewalInformation
            foreach (var item in e.Information.Items)
            {
                Log(item.Name + 
                    "\nReasons=0x" + item.Reasons + 
                    "\nRenewal Id=" + item.RenewalId);
            }
            e.Completion.Complete(false);

            Log("Leave ProtectionManager_ComponentLoadFailed");
        }

        /// <summary>
        /// Invoked to acquire the PlayReady License
        /// </summary>
        async void LicenseAcquisitionRequest(PlayReadyLicenseAcquisitionServiceRequest licenseRequest, MediaProtectionServiceCompletion completionNotifier, string url, string challengeCustomData)
        {
            bool bResult = false;
            string ExceptionMessage = string.Empty;

            try
            {
                if (!string.IsNullOrEmpty(url))
                {
                    Log("ProtectionManager PlayReady Manual License Acquisition Service Request in progress - URL: " + url);

                    if (!string.IsNullOrEmpty(challengeCustomData))
                    {
                        var encoding = new UTF8Encoding();
                        byte[] b = encoding.GetBytes(challengeCustomData);
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
                    var licenseAcquision = new CommonLicenseRequest();
                    HttpContent responseHttpContent = await licenseAcquision.AcquireLicenseAsync(new Uri(url), httpContent);
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
                    Log("ProtectionManager PlayReady License Acquisition Service Request in progress - URL: " + licenseRequest.Uri.ToString());
                    await licenseRequest.BeginServiceRequest();
                    bResult = true;
                }
            }
            catch (Exception e)
            {
                ExceptionMessage = e.Message;
            }

            if (bResult == true)
                Log(!string.IsNullOrEmpty(url) ? "ProtectionManager Manual PlayReady License Acquisition Service Request successful" :
                    "ProtectionManager PlayReady License Acquisition Service Request successful");
            else
                Log(!string.IsNullOrEmpty(url) ? "ProtectionManager Manual PlayReady License Acquisition Service Request failed: " + ExceptionMessage :
                    "ProtectionManager PlayReady License Acquisition Service Request failed: " + ExceptionMessage);
            completionNotifier.Complete(bResult);
        }
        /// <summary>
        /// Proactive Individualization Request 
        /// </summary>
        async void ProactiveIndividualizationRequest()
        {
            var individualizationRequest = new PlayReadyIndividualizationServiceRequest();
            Log("ProtectionManager PlayReady ProActive Individualization Service Request in progress...");
            bool bResultIndiv = await ReactiveIndividualizationRequestAsync(individualizationRequest, null);
            if (bResultIndiv == true)
                Log("ProtectionManager PlayReady ProActive Individualization Service Request successful");
            else
                Log("ProtectionManager PlayReady ProActive Individualization Service Request failed");

        }
        /// <summary>
        /// Invoked to send the Individualization Request 
        /// </summary>
        async Task<bool> ReactiveIndividualizationRequestAsync(PlayReadyIndividualizationServiceRequest individualizationRequest, MediaProtectionServiceCompletion completionNotifier)
        {
            bool bResult = false;
            Log("ProtectionManager PlayReady Individualization Service Request in progress...");
            try
            {
                await individualizationRequest.BeginServiceRequest();
                bResult = true;
            }
            catch (COMException comException) when (comException.HResult == MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED)
            {
                individualizationRequest.NextServiceRequest();
            }
            catch (Exception)
            {
            }

            if (bResult == true)
                Log("ProtectionManager PlayReady Individualization Service Request successful");
            else
                Log("ProtectionManager PlayReady Individualization Service Request failed");
            if (completionNotifier != null) completionNotifier.Complete(bResult);
            return bResult;

        }
        
    }
}
