//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1_reactive.html", {
        ready: function (element, options) {
            
            var video = document.getElementById("video");

            var that = this;

            var data = {
                laURL: SdkSample.sampleSimpleLicenseURL
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
            
            // binding source for the UI elements
            this.bindSource = WinJS.Binding.as(data);
            var bindSource = this.bindSource;

            function onServiceRequested(serviceRequest) {
                try {
                    processServiceRequest(serviceRequest, null);
                } catch (e) {
                    WinJS.log("Exception(onServiceRequested): " + e, "", "status");
                }
            }
            
            function processServiceRequest(serviceRequest) {
                var request = serviceRequest.request;

                var statics = Windows.Media.Protection.PlayReady.PlayReadyStatics;
                switch (request.type)
                {
                    case statics.individualizationServiceRequestType:
                        WinJS.log("Individualization Service Request Started", "", "status");
                        playreadyHelpers.reactiveIndividualization(request, serviceRequest.completion, null);
                        break;
                    case statics.licenseAcquirerServiceRequestType:
                        WinJS.log("License Service Request Started", "", "status");
                        request.uri = new Windows.Foundation.Uri(data.laURL);
                        playreadyHelpers.reactiveLicenseRequest(request, serviceRequest.completion, null);
                        break;
                }
            }

            this.mediaProtectionManager = playreadyHelpers.createMediaProtectionManager(onServiceRequested);

            video.msSetMediaProtectionManager(this.mediaProtectionManager);

            WinJS.Binding.processAll(null, bindSource);
        }
    });
})();

