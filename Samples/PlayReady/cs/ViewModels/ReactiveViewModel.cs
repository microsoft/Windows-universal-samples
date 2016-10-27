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

using SDKTemplate.Shared;
using System;
using Windows.Media.Protection;
using Windows.Media.Protection.PlayReady;
using Windows.UI.Xaml.Controls;


namespace SDKTemplate.ViewModels
{
    public class ReactiveViewModel : ViewModelBase
    {
        // The sample is a protected H264 Dash (OnDemand) asset.  
        private string moviePath = MainPage.SampleMovieURL;
        private string licenseURL = "http://playready.directtaps.net/pr/svc/rightsmanager.asmx?UseSimpleNonPersistentLicense=1";

        public RelayCommand CmdPlayMovie { get; private set; }
        public RelayCommand CmdStopMovie { get; private set; }

        // PlayReadyInfo is just used to show PlayReadyStatistics and debug info in the UI 
        public PlayReadyInfoViewModel PlayReadyInfo { get; private set; }

        /// <summary>
        /// A view model class for the ReactiveRequest Scenario.
        /// The ViewModel takes a UI MediaElement in the contructor to wire up commands and events to simplify the sample.
        /// Decoupling the MediaElement from the ViewModel would require addtional MVVM infrastucture.
        /// </summary>
        public ReactiveViewModel(MediaElement mediaElement)
        {

            /// The ProtectionManager provides communication between the player and PlayReady DRM. 
            /// The helper class will configure the protection manager for PlayReady and assign an
            /// event handler for Service requests.
            this.ProtectionManager = PlayReadyHelpers.InitializeProtectionManager(ServiceRequested);
            mediaElement.ProtectionManager = this.ProtectionManager;
            /// The PlayReadyInfoViewModel is used in this sample app to show PlayReadyStatistics such as 
            /// SecurityLevel and hardware support in the UI.
            PlayReadyInfo = new PlayReadyInfoViewModel();
            PlayReadyInfo.RefreshStatics();

            /// Reactive license acqusition will happen automatically when setting the source of 
            /// a MediaElement to protected content. 
            CmdPlayMovie = new RelayCommand( () => { mediaElement.Source = new Uri(moviePath); SetPlaybackEnabled(true); });
            /// The licenseUrl in the sample is set to return a non-peristent license. When there is a 
            /// hard Stop() on the playback, a new license will be requested on Play(). 
            CmdStopMovie = new RelayCommand(() => { mediaElement.Stop(); SetPlaybackEnabled(false); });

            mediaElement.CurrentStateChanged += (s, a) => {
                ViewModelBase.Log("Media State::" + mediaElement.CurrentState);
            };

            mediaElement.MediaFailed += (s, a) => {
                ViewModelBase.Log("Err::" + a.ErrorMessage);
            };
        }


        /// <summary>
        /// The ProtectionManager defers the service call to the ServiceReqested handler.
        /// This handler will enable the application to customize the communication (custom data, http headers, manual request)
        /// The ServiceCompletion instance will notify the ProtectionManager in the case of queued requests.
        /// </summary>
        MediaProtectionServiceCompletion serviceCompletionNotifier = null;
        void ServiceRequested(MediaProtectionManager sender, ServiceRequestedEventArgs srEvent)
        {
            serviceCompletionNotifier = srEvent.Completion;
            IPlayReadyServiceRequest serviceRequest = (IPlayReadyServiceRequest)srEvent.Request;
            ViewModelBase.Log(serviceRequest.GetType().Name);
            ProcessServiceRequest(serviceRequest);
        }

        /// <summary>
        /// The helper class will determine the exact type of ServiceRequest in order to customize and send
        /// the service request. ServiceRequests (except for Individualization and Revocation) also support the
        /// GenerateManualEnablingChallenge method. This can be used to read and customize the SOAP challenge
        /// and manually send the challenge.
        /// </summary>
        void ProcessServiceRequest(IMediaProtectionServiceRequest serviceRequest)
        {
            //Alternatively the serviceRequest can be determined by the Guid serviceRequest.Type
            if (serviceRequest is PlayReadyIndividualizationServiceRequest)
            {
               PlayReadyHelpers.ReactiveIndividualization(serviceRequest as PlayReadyIndividualizationServiceRequest, serviceCompletionNotifier, () => PlayReadyInfo.RefreshStatics());
               PlayReadyInfo.RefreshStatics();
            }
            else if (serviceRequest is PlayReadyLicenseAcquisitionServiceRequest)
            {
                var licenseRequest = serviceRequest as PlayReadyLicenseAcquisitionServiceRequest;
                // The inital service request url was taken from the playready header from the dash manifest.
                // This can overridden to a different license service prior to sending the request (staging, production,...). 
                licenseRequest.Uri = new Uri(licenseURL);

                PlayReadyHelpers.ReactiveLicenseAcquisition(licenseRequest,  serviceCompletionNotifier);
                SetPlaybackEnabled(true);
            }

            // Here are the additional service calls available to support other scenarios
            //PlayReadySecureStopServiceRequest
            //PlayReadyDomainJoinServiceRequest
            //PlayReadyDomainLeaveServiceRequest
            //PlayReadyMeteringReportServiceRequest

            //PlayReadyRevocationServiceRequest

        }


        public string MoviePath
        {
            get
            {
                return moviePath;
            }
            set
            {
                if (moviePath != value)
                {
                    moviePath = value;
                    RaisePropertyChanged();
                }
            }
        }

        // Method not used in this sample. 
        // This shows the minimal configuration for basic reactive playback.
        public void MinConfig(MediaElement mediaElement)
        {
            var manager = new MediaProtectionManager();
            var props = new Windows.Foundation.Collections.PropertySet();
            props.Add("{F4637010-03C3-42CD-B932-B48ADF3A6A54}", "Windows.Media.Protection.PlayReady.PlayReadyWinRTTrustedInput");
            manager.Properties.Add("Windows.Media.Protection.MediaProtectionSystemIdMapping", props);
            manager.Properties.Add("Windows.Media.Protection.MediaProtectionSystemId", "{F4637010-03C3-42CD-B932-B48ADF3A6A54}");
            manager.Properties.Add("Windows.Media.Protection.MediaProtectionContainerGuid", "{9A04F079-9840-4286-AB92-E65BE0885F95}");

            MediaProtectionServiceCompletion completionNotifer = null;
            manager.ServiceRequested += async (sender, srEvent) =>
            {
                completionNotifer = srEvent.Completion;
                var serviceRequest = (IPlayReadyServiceRequest)srEvent.Request;
               
                ProcessServiceRequest(serviceRequest);
                if (serviceRequest is PlayReadyIndividualizationServiceRequest)
                {
                    var indivRequest = serviceRequest as PlayReadyIndividualizationServiceRequest;
                    await indivRequest.BeginServiceRequest();
                    serviceCompletionNotifier.Complete(true);
                }
                else if (serviceRequest is PlayReadyLicenseAcquisitionServiceRequest)
                {
                    var licenseRequest = serviceRequest as PlayReadyLicenseAcquisitionServiceRequest;
                    //licenseRequest.
                    await licenseRequest.BeginServiceRequest();
                    serviceCompletionNotifier.Complete(true);
                    serviceCompletionNotifier = null;
                }

            };
            mediaElement.ProtectionManager = manager;
        }
    }
}
