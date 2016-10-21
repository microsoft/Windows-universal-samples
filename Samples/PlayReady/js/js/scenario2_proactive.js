//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var PlayReady = Windows.Media.Protection.PlayReady;

    var page = WinJS.UI.Pages.define("/html/scenario2_proactive.html", {
        ready: function (element, options) {
            
            var video = document.getElementById("video");
            var btnPlay = document.getElementById("btnPlay");
            var btnStop = document.getElementById("btnStop");
            btnPlay.disabled = "disabled";
            btnGetLicense.disabled = "disabled";
            btnStop.disabled = "disabled";

            var that = this;

            var data = {
                //The license URL will provide a persistent license that expires in 300 seconds after first play
                laURL: "http://playready.directtaps.net/pr/svc/rightsmanager.asmx?PlayRight=1&FirstPlayExpiration=300"
                , keyId : "{6F651AE1-DBE4-4434-BCB4-690D1564C41C}"
                , contentURL: SdkSample.sampleContentURL
                , currentSource: ""
                , playHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    bindSource.currentSource = bindSource.contentURL;
                    video.autoplay = "autoplay";
                    video.src = bindSource.currentSource;
                    video.controls = "controls";
                })
                ,stopHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    bindSource.currentSource = null;
                    video.pause();
                    video.removeAttribute("src");
                    video.removeAttribute("controls");
                })
                , getLicenseHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    getLicense(bindSource.keyId);
                })
                , keyIdHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    bindSource.keyId = e.target.value;
                })
                ,contentUrlHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    bindSource.contentURL = e.target.value;
                })
                , laUrlHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    bindSource.laURL = e.target.value;
                })
                , videoErrorHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    //handle video error. check video.error.msExtendedCode also
                    WinJS.log("video error code:" + video.error.code, "", "error");
                })
                
            }
            
            this.bindSource = WinJS.Binding.as(data);
            var bindSource = this.bindSource;

            function individualizeIfNeeded() {
                if (!playreadyHelpers.isIndividualized()) {
                    playreadyHelpers.proactiveIndividualization(function () {
                        //licenses can now be requested on successful individualization
                        btnGetLicense.removeAttribute('disabled');
                    });
                } else {
                    btnGetLicense.removeAttribute('disabled');
                }
            }

            function getLicense(kid) {
                var laURI = new Windows.Foundation.Uri(data.laURL);
                var customData = "token:12345";
                var serviceId = '{DEB47F00-8A3B-416D-9B1E-5D55FD023044}';
                var contentHeader = new PlayReady.PlayReadyContentHeader(kid, null, PlayReady.PlayReadyEncryptionAlgorithm.aes128Ctr, laURI, laURI, customData, serviceId);
                playreadyHelpers.proactiveLicenseAcquisition(contentHeader, onLicenseAcquisitionComplete);
            }

            function onLicenseAcquisitionComplete(success) {
                btnPlay.removeAttribute("disabled");
                btnStop.removeAttribute("disabled");
            }

            function onServiceRequested(serviceRequest) {
                try {
                    processServiceRequest(serviceRequest);
                }
                catch (e) {
                    // service request failed
                }
            }

            function processServiceRequest(serviceRequest) {
                var request = serviceRequest.request;

                var statics = Windows.Media.Protection.PlayReady.PlayReadyStatics;
                switch (request.type) {
                    case statics.individualizationServiceRequestType:
                        WinJS.log("Individualization Service Request Started", "", "status");
                        playreadyHelpers.reactiveIndividualization(request, serviceRequest.completion, null)
                        break;
                    case statics.licenseAcquirerServiceRequestType:
                        request.uri = new Windows.Foundation.Uri(data.laURL);
                        WinJS.log("License Service Request Started", "", "status");
                        playreadyHelpers.reactiveLicenseRequest(request, serviceRequest.completion, null)
                        break;
                }
            }

            this.mediaProtectionManager = playreadyHelpers.createMediaProtectionManager(onServiceRequested);

            video.msSetMediaProtectionManager(this.mediaProtectionManager);

            WinJS.Binding.processAll(null, bindSource);

            individualizeIfNeeded();
            
        }
    });
})();

