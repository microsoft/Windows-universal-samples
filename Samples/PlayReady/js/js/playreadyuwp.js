//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    var PlayReady = Windows.Media.Protection.PlayReady;

    function createMediaProtectionManager(onServiceRequested) {
        var mediaProtectionManager = new Windows.Media.Protection.MediaProtectionManager();

        var props = new Windows.Foundation.Collections.PropertySet();
        props["{F4637010-03C3-42CD-B932-B48ADF3A6A54}"] =  "Windows.Media.Protection.PlayReady.PlayReadyWinRTTrustedInput";
        mediaProtectionManager.properties["Windows.Media.Protection.MediaProtectionSystemIdMapping"] = props;
        mediaProtectionManager.properties["Windows.Media.Protection.MediaProtectionSystemId"] = "{F4637010-03C3-42CD-B932-B48ADF3A6A54}";
        mediaProtectionManager.properties["Windows.Media.Protection.MediaProtectionContainerGuid"] = "{9A04F079-9840-4286-AB92-E65BE0885F95}";

        //Setting up event handler for PlayReady related events
        mediaProtectionManager.addEventListener("servicerequested", onServiceRequested, false);
        mediaProtectionManager.addEventListener("componentloadfailed", onComponentLoadFailed, false);

        return mediaProtectionManager;
    };

    function isIndividualized() {
        return PlayReady.PlayReadyStatics.playReadyCertificateSecurityLevel != 0;
    }

    function reactiveIndividualization(indivRequest, completionNotifier, complete) {
        var currentServiceRequest = indivRequest;

        function onServiceRequestSuccess() {
            completionNotifier && completionNotifier.complete(true);
            completionNotifier = null;

            complete && complete();

            WinJS.log("Individualization Service Request Complete", "", "status");
        }

        var MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED = 0x8004b895 | 0;

        function onServiceRequestError(e) {
            switch (e.number) {
                case MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED:
                    WinJS.log("PlayReady service request error: MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED", "", "error");
                    return;
                    break;
                default:
                    WinJS.log("PlayReady service request error: " + e.number, "", "error");
            }
            
            var nextServiceRequest = currentServiceRequest.nextServiceRequest();
            if (nextServiceRequest != null) {
                nextServiceRequest.beginServiceRequest().then(onServiceRequestSuccess, onServiceRequestError);
            }
        }
        indivRequest.beginServiceRequest().then(onServiceRequestSuccess, onServiceRequestError);
    }

    function proactiveIndividualization(callback) {
        //Creating a new Indiv request and use the previously reactive fucntion to complete the request 
        var indivRequest = new PlayReady.PlayReadyIndividualizationServiceRequest();
        reactiveIndividualization(indivRequest, null, callback);
    }

    function reactiveLicenseRequest(licenseRequest, completionNotifier, complete) {
        var currentServiceRequest = licenseRequest;

        function onServiceRequestSuccess() {
            completionNotifier && completionNotifier.complete(true);
            completionNotifier = null;

            complete && complete();

            WinJS.log("License Service Request Complete", "", "status");
        }

        function onServiceRequestError(e) {
            if (e.number != -2147174251) { // MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED (0x8004b895)
                // 
            } else {
                var nextServiceRequest = currentServiceRequest.nextServiceRequest();
                if (nextServiceRequest != null) {
                    nextServiceRequest.beginServiceRequest().then(onServiceRequestSuccess, onServiceRequestError);
                }
            }
        }
        licenseRequest.beginServiceRequest().then(onServiceRequestSuccess, onServiceRequestError);
    }

    function proactiveLicenseAcquisition(contentHeader, complete) {
        var laRequest = new PlayReady.PlayReadyLicenseAcquisitionServiceRequest();
        laRequest.contentHeader = contentHeader;

        // The uri property can be used to override the license service.
        // laRequest.uri = ...;
        reactiveLicenseRequest(laRequest, null, complete);
    }

    function onComponentLoadFailed(e) {

        //  List the failing components
        for (var i = 0; i < e.information.items.size; i++) {
            //logMsg(e.information.items[i].name + "\nReasons=0x" + e.information.items[i].reasons + '\n'
            //                                    + "Renewal Id=" + e.information.items[i].renewalId);
        }
        e.completion.complete(false);
        //logMsg("Resumed source (false)");
    }

    WinJS.Namespace.define("playreadyHelpers", {
        createMediaProtectionManager: createMediaProtectionManager,
        isIndividualized: isIndividualized,
        reactiveIndividualization: reactiveIndividualization,
        proactiveIndividualization: proactiveIndividualization,
        reactiveLicenseRequest: reactiveLicenseRequest,
        proactiveLicenseAcquisition: proactiveLicenseAcquisition
    });
})();