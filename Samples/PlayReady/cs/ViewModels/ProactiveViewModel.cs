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
    public class ProactiveViewModel : ViewModelBase
    {
        // A KeyId can be used to directly request a media license prior to accessing the 
        // associated proteced content
        private string keyId = "{6F651AE1-DBE4-4434-BCB4-690D1564C41C}";
        // The content assocated with the above KeyId
        private string moviePath = MainPage.SampleMovieURL;
        // The following test license service will return a persistent license with a MaxResDecode policy set
        private string licenseUrl = "http://playready.directtaps.net/pr/svc/rightsmanager.asmx?PlayRight=1&SecurityLevel=2000&MaxResDecode=1920x1080";

        public RelayCommand CmdGetLicense { get; private set; }
        public RelayCommand CmdPlayMovie { get; private set; }
        public RelayCommand CmdStopMovie { get; private set; }

        public PlayReadyInfoViewModel PlayReadyInfo { get; private set; }
        /// <summary>
        /// A view model class for the IndivReactive Scenario.
        /// </summary>
        public ProactiveViewModel(MediaElement mediaElement)
        {
            /// The ProtectionManager provides communication between the player and PlayReady DRM. 
            /// The helper class will configure the protection manager for PlayReady and assign an
            /// event handler for Service requests.
            this.ProtectionManager = PlayReadyHelpers.InitializeProtectionManager(ServiceRequested);
            mediaElement.ProtectionManager = this.ProtectionManager;
            /// The PlayReadyInfoViewModel is used in this sample app to show PlayReadyStatistics such as 
            /// SecurityLevel and hardware support within the UI.
            PlayReadyInfo = new PlayReadyInfoViewModel();
            PlayReadyInfo.RefreshStatics();            

            mediaElement.CurrentStateChanged += (s, a) => {
                ViewModelBase.Log("Media State::" + mediaElement.CurrentState);
            };

            mediaElement.MediaFailed += (s, a) => {
                ViewModelBase.Log("Media Failed::" + a.ErrorMessage);
            };

            /// Proactive license acqusition will ensure a license is available
            /// prior to playback
            CmdGetLicense = new RelayCommand(() => { GetLicense(new Guid(this.KeyId)); }, () => { return PlayReadyHelpers.IsIndividualized; });

            /// Play is enabled once a license is available
            CmdPlayMovie = new RelayCommand(() => { mediaElement.Source = new Uri(moviePath); SetPlaybackEnabled(true); }, 
                                            () => IsLicenseAvailable(new Guid(KeyId)));
            CmdStopMovie = new RelayCommand(() => { mediaElement.Stop(); SetPlaybackEnabled(false); }, 
                                            () => IsLicenseAvailable(new Guid(KeyId))); 

            /// Proactive individualization will ensure PlayReady have been configured
            /// to begin making license requests
            IndividualizeIfNeeded();
        }

        /// A check is made to see of the device has been initialize and a indiv
        /// service request is sent if not. The callback will notify the app license
        /// calls can now be made.
        void IndividualizeIfNeeded()
        {
            if (!PlayReadyHelpers.IsIndividualized)
            {
                PlayReadyHelpers.ProactiveIndividualization(() => {
                    CmdGetLicense.RaiseCanExecuteChanged();
                    PlayReadyInfo.RefreshStatics();
                });
            }
        }

        /// Tests to see if a license is available for playback.
        bool IsLicenseAvailable(Guid kid)
        {
            // return false is app is not initialized 
            // creating a PlayReadyContentHeader in an un-initialized app
            // will cause an exception
            if (!PlayReadyHelpers.IsIndividualized)
            {
                return false;
            }
            var contentHeader = new PlayReadyContentHeader(kid, "", PlayReadyEncryptionAlgorithm.Aes128Ctr, new Uri(licenseUrl), new Uri(licenseUrl), "", Guid.Empty);
            var licenses = new PlayReadyLicenseIterable(contentHeader, false);
            foreach (var license in licenses)
            {
                if (license.UsableForPlay)
                {
                    return true;
                }
            }

            return false;
        }

        /// A content header is required to make a proactive license request.
        /// The header needs at minimum a KeyId and a License URL.
        /// Additionally you set a custom data property which will be passed in the signed
        /// section of the license request for use by the service.
        void GetLicense(Guid kid)
        {
            var laURL = new Uri(licenseUrl);
            var customData = "token:12345";
            var contentHeader =  new PlayReadyContentHeader(kid, "", PlayReadyEncryptionAlgorithm.Aes128Ctr, laURL, laURL, customData, Guid.Empty);
            PlayReadyHelpers.ProactiveLicenseAcquisition(contentHeader, () => {
                CmdPlayMovie.RaiseCanExecuteChanged();
                CmdStopMovie.RaiseCanExecuteChanged();
            });
        }

        // The ServiceRequested and ProcessServiceRequest methods are not use in this 
        // sample because service calls are made proactively. The app could be re-configured 
        // to call Individualization proactively and reactively handle license 
        // requests which would used these methods.
        MediaProtectionServiceCompletion serviceCompletionNotifier = null;
        void ServiceRequested(MediaProtectionManager sender, ServiceRequestedEventArgs srEvent)
        {
            serviceCompletionNotifier = srEvent.Completion;
            IPlayReadyServiceRequest serviceRequest = (IPlayReadyServiceRequest)srEvent.Request;
            ViewModelBase.Log(serviceRequest.GetType().Name);
            ProcessServiceRequest(serviceRequest);
        }

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
                PlayReadyHelpers.ReactiveLicenseAcquisition(serviceRequest as PlayReadyLicenseAcquisitionServiceRequest,  serviceCompletionNotifier);
            }
        }

        public string KeyId
        {
            get
            {
                return keyId;
            }
            set
            {
                if (keyId != value)
                {
                    keyId = value;
                    RaisePropertyChanged();
                }
            }
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
    }
}
