//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3_eme.html", {
        ready: function (element, options) {
            
            var drmManager;

            var video = document.getElementById("video");

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
                    drmManager.licenseUrl = bindSource.laURL;
                })
                , videoErrorHandler: WinJS.Utilities.markSupportedForProcessing(function (e) {
                    //handle video error. check video.error.msExtendedCode also
                    WinJS.log("video error code:" + video.error.code, "", "error");
                })

            }
            
            this.bindSource = WinJS.Binding.as(data);
            var bindSource = this.bindSource;

            drmManager = new playready.drmManager(video, data.laURL);
            
            WinJS.Binding.processAll(null, bindSource);
        }
    });
})();

