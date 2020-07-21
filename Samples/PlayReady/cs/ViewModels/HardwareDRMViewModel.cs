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

    public class HardwareDRMViewModel : ViewModelBase
    {
        private string moviePath = MainPage.SampleMovieURL;
        private string licenseUrl = "http://playready.directtaps.net/pr/svc/rightsmanager.asmx?PlayRight=1&SecurityLevel=2000";

        public RelayCommand CmdUseHardware { get; private set; }
        public RelayCommand CmdUseSoftware { get; private set; }

        public RelayCommand CmdPlayMovie { get; private set; }
        public RelayCommand CmdStopMovie { get; private set; }

        public PlayReadyInfoViewModel PlayReadyInfo { get; private set; }

        // flag to determine if hardware or software have been selected.
        // The application should try to stay with one or the other mode 
        // and not frequently jump between the two.
        private bool modeSelected = false;
        /// <summary>
        /// A view model class for the IndivReactive Scenario.
        /// </summary>
        public HardwareDRMViewModel(MediaElement mediaElement)
        {
            this.ProtectionManager = PlayReadyHelpers.InitializeProtectionManager(ServiceRequested);
            mediaElement.ProtectionManager = this.ProtectionManager;
            PlayReadyInfo = new PlayReadyInfoViewModel();
            PlayReadyInfo.RefreshStatics();

            mediaElement.CurrentStateChanged += (s, a) => {
                ViewModelBase.Log("Media State::" + mediaElement.CurrentState);
            };

            mediaElement.MediaFailed += (s, a) => {
                ViewModelBase.Log("Media Failed::" + a.ErrorMessage);
            };

            CmdPlayMovie    = new RelayCommand(() => { mediaElement.Source = new Uri(moviePath); SetPlaybackEnabled(true); },
                                               () => { return modeSelected && PlayReadyHelpers.IsIndividualized; });
            CmdStopMovie    = new RelayCommand(() => { mediaElement.Stop(); SetPlaybackEnabled(false); }); 
            CmdUseHardware  = new RelayCommand(() => { ConfigureHardwareDRM(mediaElement); }, 
                                               () => { return !modeSelected;  });
            CmdUseSoftware  = new RelayCommand(() => { ConfigureSoftwareDRM(mediaElement); }, 
                                               () => { return !modeSelected; });

        }

        /// <summary>
        /// This method will configure the properties used by Media Foundation and PlayReady
        /// to specify Hardware DRM. The existing ProtectionManager assigned to the MediaElement
        /// is altered with updated/removed properties. Lastly, proactive individualiation is called.
        /// </summary>
        private void ConfigureHardwareDRM(MediaElement mediaElement)
        {
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            localSettings.CreateContainer("PlayReady", Windows.Storage.ApplicationDataCreateDisposition.Always);
            localSettings.Containers["PlayReady"].Values["SoftwareOverride"] = 0;
            localSettings.Containers["PlayReady"].Values["HardwareOverride"] = 1;
            mediaElement.ProtectionManager.Properties.Remove("Windows.Media.Protection.UseSoftwareProtectionLayer");
            mediaElement.ProtectionManager.Properties["Windows.Media.Protection.UseHardwareProtectionLayer"] = true;
            Individualize();
        }

        /// <summary>
        /// This method will configure the properties used by Media Foundation and PlayReady
        /// to specify Software DRM. The existing ProtectionManager assigned to the MediaElement
        /// is altered with updated/removed properties. Lastly, proactive individualiation is called.
        /// </summary>
        private void ConfigureSoftwareDRM(MediaElement mediaElement)
        {
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            localSettings.CreateContainer("PlayReady", Windows.Storage.ApplicationDataCreateDisposition.Always);
            localSettings.CreateContainer("PlayReady", Windows.Storage.ApplicationDataCreateDisposition.Always);
            localSettings.Containers["PlayReady"].Values["SoftwareOverride"] = 1;
            localSettings.Containers["PlayReady"].Values["HardwareOverride"] = 0;
            mediaElement.ProtectionManager.Properties.Remove("Windows.Media.Protection.UseHardwareProtectionLayer");
            mediaElement.ProtectionManager.Properties["Windows.Media.Protection.UseSoftwareProtectionLayer"] = true;
            Individualize();
        }

        // Proactive individualization is used to configure playready to the selected 
        // hardware/software mode
        void Individualize()
        {
            PlayReadyHelpers.ProactiveIndividualization(() => {
                modeSelected = true;
                CmdPlayMovie.RaiseCanExecuteChanged();
                CmdUseHardware.RaiseCanExecuteChanged();
                CmdUseSoftware.RaiseCanExecuteChanged();
            });
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
                // The initial service request url was taken from the playready header from the dash manifest.
                // This can overridden to a different license service prior to sending the request (staging, production,...). 
                licenseRequest.Uri = new Uri(licenseUrl);

                PlayReadyHelpers.ReactiveLicenseAcquisition(licenseRequest, serviceCompletionNotifier);
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
