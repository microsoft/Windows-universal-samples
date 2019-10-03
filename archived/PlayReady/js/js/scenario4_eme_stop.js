//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario4_eme_stop.html", {
        ready: function (element, options) {

            var drmManager;

            var video = document.getElementById("video");
            var btnPlay = document.getElementById("btnPlay");
            var btnStop = document.getElementById("btnStop");
            btnPlay.disabled = 'disabled';
            btnStop.disabled = 'disabled';

            var data = {
                laURL: "http://playready.directtaps.net/pr/svc/rightsmanager.asmx?SecureStop=1&UseSimpleNonPersistentLicense=1&RealTimeExpiration=1&FirstPlayExpiration=300"
                , pubCertURL: "http://playready.directtaps.net/pr/svc/rightsmanager.asmx"
                , publisherID: "xw6TjIuUUmvdCcl00t4RBA=="
                , contentURL: SdkSample.sampleContentURL
                , currentSource: ""
                , playHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    bindSource.currentSource = bindSource.contentURL;
                    video.autoplay = "autoplay";
                    video.src = bindSource.currentSource;
                    video.controls = "controls";
                })
                , stopHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    bindSource.currentSource = null;
                    drmManager.createSecureStopCDMSession();
                    video.pause();
                    video.removeAttribute("src");
                    video.removeAttribute("controls");
                })
                ,contentUrlHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    bindSource.contentURL = e.target.value;
                })
                , laUrlHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    bindSource.laURL = e.target.value;
                    drmManager.licenseUrl = bindSource.laURL;
                })
                , videoErrorHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    //handle video error. check video.error.msExtendedCode also
                    WinJS.log("video error code:" + video.error.code, "", "error");
                })
                , getPublisherCertHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    drmManager.getPublisherCert(bindSource.pubCertURL, bindSource.publisherID, function (result) {
                            //the cert should be cached within the app storage for subsequent uses 
                            btnPlay.removeAttribute('disabled');
                            btnStop.removeAttribute('disabled');
                        }
                    );
                })
                , sendStopDataHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    drmManager.createSecureStopCDMSession();
                })

            }
            
            this.bindSource = WinJS.Binding.as(data);
            var bindSource = this.bindSource;

            drmManager = new playready.drmManager(video, data.laURL);
            
            // send Secure Stop data when playback ends
            video.addEventListener('ended', function() {
                drmManager.createSecureStopCDMSession();
            }, false);

            WinJS.Binding.processAll(null, bindSource);
        }
    });
})();

