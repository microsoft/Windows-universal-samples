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
using System.Linq;
using System.Net.Http;
using System.Xml.Linq;
using Windows.Media.Protection;
using Windows.Media.Protection.PlayReady;
using Windows.Storage;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SDKTemplate.ViewModels
{
    public class SecureStopViewModel : ViewModelBase
    {
        private string moviePath = SDKTemplate.MainPage.SampleMovieURL;
        private string licenseUrl = "http://playready.directtaps.net/pr/svc/rightsmanager.asmx?SecureStop=1&UseSimpleNonPersistentLicense=1&RealTimeExpiration=1&FirstPlayExpiration=300";

        private string secureStopServiceUrl = "http://playready.directtaps.net/pr/svc/rightsmanager.asmx";

        // publisherID and cert used by the test license service
        private string publisherID = "xw6TjIuUUmvdCcl00t4RBA==";
        private byte[] publisherCert;

        public RelayCommand CmdPlayMovie { get; private set; }
        public RelayCommand CmdStopMovie { get; private set; }
        public RelayCommand CmdGetPublisherCert { get; private set; }
        public RelayCommand CmdRenewLicense { get; private set; }

        public PlayReadyInfoViewModel PlayReadyInfo { get; private set; }
        // The PlayReadyLicenseSession instance needs to be in scope until playback has completed.
        // To handle this the following instance is maintained.
        public PlayReadyLicenseSession licenseSession { get; private set; }
        // The ContentHeader for the actively playing asset is tracked to support
        // 
        public PlayReadyContentHeader activePlayReadyHeader { get; private set; }

        /// <summary>
        /// A view model class for the IndivReactive Scenario.
        /// </summary>
        public SecureStopViewModel(MediaElement mediaElement)
        {
            this.ProtectionManager = PlayReadyHelpers.InitializeProtectionManager(ServiceRequested);
            mediaElement.ProtectionManager = this.ProtectionManager;

            licenseSession = PlayReadyHelpers.createLicenseSession();
            licenseSession.ConfigureMediaProtectionManager(mediaElement.ProtectionManager);

            PlayReadyInfo = new PlayReadyInfoViewModel();
            PlayReadyInfo.RefreshStatics();

            CmdPlayMovie = new RelayCommand(
                                    () => { mediaElement.Source = new Uri(moviePath); SetPlaybackEnabled(true); },
                                    () => publisherCert != null);

            CmdStopMovie = new RelayCommand(() => { mediaElement.Stop();}, 
                                            () => publisherCert != null);

            CmdGetPublisherCert = new RelayCommand(() => 
                                            {
                                                GetPublisherCert(publisherID, (cert) =>
                                                {
                                                    CmdPlayMovie.RaiseCanExecuteChanged();
                                                    CmdStopMovie.RaiseCanExecuteChanged();
                                                    CmdRenewLicense.RaiseCanExecuteChanged();
                                                });
                                            });

            CmdRenewLicense = new RelayCommand(() => RenewActiveLicense(), 
                                               () => publisherCert != null);

            mediaElement.CurrentStateChanged += (s, a) =>
            {
                ViewModelBase.Log("Media State::" + mediaElement.CurrentState);

                switch (mediaElement.CurrentState)
                {
                    case MediaElementState.Closed:
                        SendSecureStopRecords();
                        activePlayReadyHeader = null;
                        SetPlaybackEnabled(false);
                        // renewing the licenseSession for subsequent Plays since the 
                        // session is stopped
                        licenseSession = PlayReadyHelpers.createLicenseSession();
                        licenseSession.ConfigureMediaProtectionManager(mediaElement.ProtectionManager);
                        break;
                    default:
                        break;
                }

            };

            mediaElement.MediaFailed += (s, a) =>
            {
                ViewModelBase.Log("Err::" + a.ErrorMessage);
            };

            var localSettings = ApplicationData.Current.LocalSettings;
            ApplicationDataContainer container;
            localSettings.Containers.TryGetValue("PublisherCerts", out container);
            if (container != null && container.Values.ContainsKey(publisherID))
            {
                publisherCert = (byte[])container.Values[publisherID];
            }

            try
            {
                var securityVersion = PlayReadyStatics.PlayReadySecurityVersion;
                SendSecureStopRecords();
            }
            catch {
                PlayReadyHelpers.ProactiveIndividualization(() =>
                {
                    PlayReadyInfo.RefreshStatics();
                });
            }
        }

        private async void SendSecureStopRecords()
        {
            if (publisherCert == null) { return; }

            //// The following code will send all secure stop sessions at once
            var secureStopRequest = new PlayReadySecureStopServiceRequest(publisherCert);
            // setting the uri is optional. the uri defaults to whats in the cert
            try
            {
                secureStopRequest.Uri = new Uri(secureStopServiceUrl);
                await secureStopRequest.BeginServiceRequest();
            }
            catch (Exception e)
            {

                ViewModelBase.Log(e.Message);
            }


            // The following code can be used to iterate the secure stop sessions 
            // which can be then be processed individually
            //PlayReadySecureStopIterable secureStopIterable = new PlayReadySecureStopIterable(publisherCert);
            //foreach (var secureStopReq in secureStopIterable)
            //{
            //    secureStopReq.Uri = new Uri(secureStopServiceUrl);
            //    await secureStopReq.BeginServiceRequest();
            //}
        }

        //...
        MediaProtectionServiceCompletion serviceCompletionNotifier = null;
        void ServiceRequested(MediaProtectionManager sender, ServiceRequestedEventArgs srEvent)
        {
            serviceCompletionNotifier = srEvent.Completion;
            IPlayReadyServiceRequest serviceRequest = (IPlayReadyServiceRequest)srEvent.Request;
            ViewModelBase.Log(serviceRequest.GetType().Name);
            ProcessServiceRequest(serviceRequest);
        }

        /// <summary>
        /// ...
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
                var licenseServiceRequestNotUsed = (serviceRequest as PlayReadyLicenseAcquisitionServiceRequest);
                activePlayReadyHeader = licenseServiceRequestNotUsed.ContentHeader;

                var licenseServiceRequest = licenseSession.CreateLAServiceRequest();
                licenseServiceRequest.ContentHeader = activePlayReadyHeader;
                licenseServiceRequest.Uri = new Uri(licenseUrl);
                PlayReadyHelpers.ReactiveLicenseAcquisition(licenseServiceRequest, serviceCompletionNotifier);
            }
        }

        void RenewActiveLicense()
        { 
            if (activePlayReadyHeader != null)
            {
                var licenseServiceRequest = licenseSession.CreateLAServiceRequest();
                licenseServiceRequest.ContentHeader = activePlayReadyHeader;
                licenseServiceRequest.Uri = new Uri(licenseUrl);
                PlayReadyHelpers.ReactiveLicenseAcquisition(licenseServiceRequest, null);
            }
        }

        private async void GetPublisherCert(string publisherID, Action<byte[]> callback)
        {
            var localSettings = ApplicationData.Current.LocalSettings;
            if (!localSettings.Containers.ContainsKey("PublisherCerts"))
            {
                localSettings.CreateContainer("PublisherCerts", Windows.Storage.ApplicationDataCreateDisposition.Always);
            }

            if (!localSettings.Containers["PublisherCerts"].Values.ContainsKey(publisherID))
            {
                var xDoc = XDocument.Load("Data/MeteringChallenge.xml");
                XNamespace ns = "http://schemas.microsoft.com/DRM/2007/03/protocols";
                xDoc.Descendants(ns + "MID").FirstOrDefault().Value = publisherID;

                var content = new StringContent(xDoc.ToString(), System.Text.Encoding.UTF8, "text/xml");

                var client = new HttpClient();
                client.DefaultRequestHeaders.Add("SOAPAction", "http://schemas.microsoft.com/DRM/2007/03/protocols/GetMeteringCertificate");
                var response = await client.PostAsync(secureStopServiceUrl, content);

                string soapXml = await response.Content.ReadAsStringAsync();
                var xMeteringDoc = XDocument.Parse(soapXml);

                var base64Cert = xMeteringDoc.Descendants(ns + "MeterCert").FirstOrDefault().Value;
                var cert = Convert.FromBase64String(base64Cert);
                localSettings.Containers["PublisherCerts"].Values[publisherID] = cert;
            }

            this.publisherCert = (byte [])localSettings.Containers["PublisherCerts"].Values[publisherID];
            callback(this.publisherCert);
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


