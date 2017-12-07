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

using SDKTemplate.ViewModels;
using System;
using Windows.ApplicationModel.Core;
using Windows.Media.Protection;
using Windows.Media.Protection.PlayReady;
using Windows.UI.Core;

namespace SDKTemplate.Shared
{
    // This class provides helper methods for common PlayReady operations. 
    // Both Reactive and Proactive service call helpers are defined. 
    // Proactive helpers will configure a request and use the Reative method for the call.
    class PlayReadyHelpers
    {
        // This property shows if the application has been individualized.
        // The PlayReadyCertificateSecurityLevel will be 0 if it has not.
        static public bool IsIndividualized
        {
            get { return PlayReadyStatics.PlayReadyCertificateSecurityLevel != 0; }
        }

        static public MediaProtectionManager InitializeProtectionManager(ServiceRequestedEventHandler serviceRequestHandler)
        {

            var protectionManager = new MediaProtectionManager();

            //A setting to tell MF that we are using PlayReady.
            var props = new Windows.Foundation.Collections.PropertySet();
            props.Add("{F4637010-03C3-42CD-B932-B48ADF3A6A54}", "Windows.Media.Protection.PlayReady.PlayReadyWinRTTrustedInput");
            protectionManager.Properties.Add("Windows.Media.Protection.MediaProtectionSystemIdMapping", props);
            protectionManager.Properties.Add("Windows.Media.Protection.MediaProtectionSystemId", "{F4637010-03C3-42CD-B932-B48ADF3A6A54}");
            //Maps the conatiner guid from the manifest or media segment
            protectionManager.Properties.Add("Windows.Media.Protection.MediaProtectionContainerGuid", "{9A04F079-9840-4286-AB92-E65BE0885F95}");

            protectionManager.ServiceRequested += new ServiceRequestedEventHandler(serviceRequestHandler);
            protectionManager.ComponentLoadFailed += new ComponentLoadFailedEventHandler(ComponentLoadFailed);

            // Windows 10 provides built in support for Dash and does not require additional configuration. 
            // This would be be good place to configure a MediaExtensionManager to support another stream source 
            // such as the Smooth Streaming SDK

            return protectionManager;

        }

        static public PlayReadyLicenseSession createLicenseSession()
        {
            //A setting to tell MF that we are using PlayReady.
            var propSetMF = new Windows.Foundation.Collections.PropertySet();
            propSetMF["Windows.Media.Protection.MediaProtectionSystemId"] = "{F4637010-03C3-42CD-B932-B48ADF3A6A54}";
            var cpsystems = new Windows.Foundation.Collections.PropertySet();
            cpsystems["{F4637010-03C3-42CD-B932-B48ADF3A6A54}"] = "Windows.Media.Protection.PlayReady.PlayReadyWinRTTrustedInput"; //Playready TrustedInput Class Name
            propSetMF["Windows.Media.Protection.MediaProtectionSystemIdMapping"] = cpsystems;

            //Create the MF media session that the license will be tied to
            var pmpServer = new Windows.Media.Protection.MediaProtectionPMPServer(propSetMF);

            var propSetPMPServer = new Windows.Foundation.Collections.PropertySet();
            //Set the property for the LicenseSession. This tells PlayReady to tie the license to that particular media session
            propSetPMPServer["Windows.Media.Protection.MediaProtectionPMPServer"] = pmpServer;

            var licenseSession = new Windows.Media.Protection.PlayReady.PlayReadyLicenseSession(propSetPMPServer);
            return licenseSession;
        }


        /// <summary>
        /// A ComponentLoadFailed event will occur when binary data fails to load.
        /// This an infrequent event that may be triggered by a pending OS update in which DRM compoments 
        /// are being renewed and may require a restart.
        /// </summary>
        static void ComponentLoadFailed(MediaProtectionManager sender, ComponentLoadFailedEventArgs e)
        {

            for (int i = 0; i < e.Information.Items.Count; i++)
            {
                ViewModelBase.Log(e.Information.Items[i].Name + "\nReasons=0x" + e.Information.Items[i].Reasons + "\n"
                                                    + "Renewal Id=" + e.Information.Items[i].RenewalId);
            }
            e.Completion.Complete(false);
        }

        /// <summary>
        /// Reactive individualization is triggered by PlayReady when the DRM is first utilized in a new app.
        /// If individualization was triggered as part a license request then that request will be queued until
        /// the indiv process completes. The serviceCompletionNotifier is utilized to notify the ProtectionManager 
        /// that the next service request can begin.
        /// </summary>
        static public async void ReactiveIndividualization(PlayReadyIndividualizationServiceRequest indivRequest,
                                                                MediaProtectionServiceCompletion serviceCompletionNotifier,
                                                                Action callback = null)
        {
            Exception exception = null;
            try
            {
                // The actual service call. 
                // The Indiv ServiceRequest call cannot be customized like most other ServiceRequests. 
                await indivRequest.BeginServiceRequest();
            }
            catch (Exception ex)
            {
                exception = ex;
            }
            finally
            {
                if (serviceCompletionNotifier != null)
                {
                    // The Complete call will notify listeners that the service call is complete 
                    // and queued service requests can begin.
                    serviceCompletionNotifier.Complete(exception == null);
                }
                ViewModelBase.Log("ReactiveIndividualization::Complete");
            }

            // The callback is only used in the sample to show PlayReadyStatics within the UI on the 
            // completion of the ServiceRequest.
            if (callback != null)
            {
                var dispatcher = CoreApplication.MainView.CoreWindow.Dispatcher;
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    callback.Invoke();
                });
            }
        }

        /// <summary>
        /// Reactive license acquisition is triggered by PlayReady when the DRM does not have a license for the 
        /// content(KeyID) as part of an active playback attempt.
        /// Service requests can be queued which is useful in scenarios such as root/leaf license and PlayReady Domains 
        /// where one license depends on another. The serviceCompletionNotifier is utilized to notify the ProtectionManager 
        /// that the next service request can begin.
        /// </summary>
        static async public void ReactiveLicenseAcquisition(IPlayReadyLicenseAcquisitionServiceRequest licenseRequest, 
                                                                   MediaProtectionServiceCompletion serviceCompletionNotifier,
                                                                   Action callback = null)
        {
            
            Exception exception = null;
            bool success = false;
            try
            {
                await licenseRequest.BeginServiceRequest();
                success = true;
            }
            catch (Exception ex)
            {
                exception = ex;
            }
            finally
            {
                if (serviceCompletionNotifier != null)
                {
                    // The Complete call will notify listeners that the service call is complete
                    // and queued service requests can begin.
                    serviceCompletionNotifier.Complete(success);
                    serviceCompletionNotifier = null;
                }
                ViewModelBase.Log("ReactiveLicenseAcquisition::Complete");
            }

            // optional call back to update UI.
            if (callback != null)
            {
                var dispatcher = CoreApplication.MainView.CoreWindow.Dispatcher;
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    callback.Invoke();
                });

            }
        }

        /// <summary>
        /// The individual process can kick off proactively and not need to wait for a reactionary service
        /// request from PlayReady. This enables the app to manage when activation occures.
        /// </summary>
        static public void ProactiveIndividualization(Action callback)
        {
            //Creating a new Indiv request and using the previous reactive method to complete the request 
            var indivRequest = new PlayReadyIndividualizationServiceRequest();
            ReactiveIndividualization(indivRequest, null, callback);
        }

        /// <summary>
        /// The application can proativly request a license by creating a license request and 
        /// assigning a header which provides the specifics such as the KeyId of the license needed.
        /// </summary>
        static public void ProactiveLicenseAcquisition(PlayReadyContentHeader contentHeader, Action callback = null)
        {
            var laRequest = new PlayReadyLicenseAcquisitionServiceRequest();
            laRequest.ContentHeader = contentHeader;
            ReactiveLicenseAcquisition(laRequest, null, callback);
        }




    }
}

