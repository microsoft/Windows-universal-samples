//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            document.getElementById("oAuthFlickrLaunch").addEventListener("click", launchFlickrWebAuth, false);

            // Continuation handlers are specific to Windows Phone.
            if (options && options.activationKind === Windows.ApplicationModel.Activation.ActivationKind.webAuthenticationBrokerContinuation) {
                continueWebAuthentication(options.activatedEventArgs);
            }
        }
    });

    var authzInProgress = false;

    function sendRequest(url) {
        try {
            var request = new XMLHttpRequest();
            request.open("GET", url, false);
            request.send(null);
            return request.responseText;
        } catch (err) {
            WinJS.log("Error sending request: " + err, "Web Authentication SDK Sample", "error");
        }
    }

    function sendPostRequest(url, authzheader) {
        try {
            var request = new XMLHttpRequest();
            request.open("POST", url, false);
            request.setRequestHeader("Authorization", authzheader);
            request.send(null);
            return request.responseText;
        } catch (err) {
            WinJS.log("Error sending request: " + err, "Web Authentication SDK Sample", "error");
        }
    }

    function isValidUriString(uriString) {
        var uri = null;
        try {
            uri = new Windows.Foundation.Uri(uriString);
        }
        catch (err) {
        }
        return uri !== null;
    }

    function launchFlickrWebAuth() {
        var flickrURL = "https://secure.flickr.com/services/oauth/request_token";

        // Get all the parameters from the user
        var clientID = document.getElementById("FlickrClientID").value;
        if (clientID === null || clientID === "") {
            WinJS.log("Please enter a ClientID for Flickr", "Web Authentication SDK Sample", "error");
            return;
        }

        var clientSecret = document.getElementById("FlickrSecret").value;
        if (clientSecret === null || clientSecret === "") {
            WinJS.log("Please enter a Secret for Flickr App", "Web Authentication SDK Sample", "error");
            return;
        }

        var callbackURL = document.getElementById("FlickrCallbackURL").value;
        if (!isValidUriString(callbackURL)) {
            WinJS.log("Enter a valid Callback URL for Flickr", "Web Authentication SDK Sample", "error");
            return;
        }

        if (authzInProgress) {
            WinJS.log("Authorization already in Progress ...", "Web Authentication SDK Sample", "status");
            return;
        }

        // Acquiring a request token
        var timestamp = Math.round(new Date().getTime() / 1000.0);
        var nonce = Math.random();
        nonce = Math.floor(nonce * 1000000000);

        // Compute base signature string and sign it.
        // This is a common operation that is required for all requests even after the token is obtained.
        // Parameters need to be sorted in alphabetical order
        // Keys and values should be URL Encoded.
        var sigBaseStringParams = "oauth_callback=" + encodeURIComponent(callbackURL);
        sigBaseStringParams += "&" + "oauth_consumer_key=" + clientID;
        sigBaseStringParams += "&" + "oauth_nonce=" + nonce;
        sigBaseStringParams += "&" + "oauth_signature_method=HMAC-SHA1";
        sigBaseStringParams += "&" + "oauth_timestamp=" + timestamp;
        sigBaseStringParams += "&" + "oauth_version=1.0";

        var sigBaseString = "GET&";
        sigBaseString += encodeURIComponent(flickrURL) + "&" + encodeURIComponent(sigBaseStringParams);

        var keyText = clientSecret + "&";
        var keyMaterial = Windows.Security.Cryptography.CryptographicBuffer.convertStringToBinary(keyText, Windows.Security.Cryptography.BinaryStringEncoding.Utf8);
        var macAlgorithmProvider = Windows.Security.Cryptography.Core.MacAlgorithmProvider.openAlgorithm("HMAC_SHA1");
        var key = macAlgorithmProvider.createKey(keyMaterial);
        var tbs = Windows.Security.Cryptography.CryptographicBuffer.convertStringToBinary(sigBaseString, Windows.Security.Cryptography.BinaryStringEncoding.Utf8);
        var signatureBuffer = Windows.Security.Cryptography.Core.CryptographicEngine.sign(key, tbs);
        var signature = Windows.Security.Cryptography.CryptographicBuffer.encodeToBase64String(signatureBuffer);

        flickrURL += "?" + sigBaseStringParams + "&oauth_signature=" + encodeURIComponent(signature);
        var response = sendRequest(flickrURL);

        var oauth_token;
        var oauth_token_secret;
        var keyValPairs = response.split("&");

        for (var i = 0; i < keyValPairs.length; i++) {
            var splits = keyValPairs[i].split("=");
            switch (splits[0]) {
                case "oauth_token":
                    oauth_token = splits[1];
                    break;
                case "oauth_token_secret":
                    oauth_token_secret = splits[1];
                    break;
            }
        }

        WinJS.log("OAuth Token = " + oauth_token + "\r\nOAuth Token Secret = " + oauth_token_secret, "Web Authentication SDK Sample", "status");

        // Send the user to authorization
        flickrURL = "https://secure.flickr.com/services/oauth/authorize?oauth_token=" + oauth_token + "&perms=read";
        WinJS.log("Navigating to: " + flickrURL, "Web Authentication SDK Sample", "error");
        var startURI = new Windows.Foundation.Uri(flickrURL);
        var endURI = new Windows.Foundation.Uri(callbackURL);

        authzInProgress = true;

        if (Windows.Security.Authentication.Web.WebAuthenticationBroker.authenticateAndContinue) {
            Windows.Security.Authentication.Web.WebAuthenticationBroker.authenticateAndContinue(startURI, endURI, null, Windows.Security.Authentication.Web.WebAuthenticationOptions.none);
        }
        else {
            Windows.Security.Authentication.Web.WebAuthenticationBroker.authenticateAsync(
                Windows.Security.Authentication.Web.WebAuthenticationOptions.none, startURI, endURI)
                .done(function (result) {
                    document.getElementById("FlickrReturnedToken").value = result.responseData;
                    WinJS.log("Status returned by WebAuth broker: " + result.responseStatus, "Web Authentication SDK Sample", "error");
                    if (result.responseStatus === Windows.Security.Authentication.Web.WebAuthenticationStatus.errorHttp) {
                        WinJS.log("Error returned: " + result.responseErrorDetail, "Web Authentication SDK Sample", "error");
                    }
                    authzInProgress = false;
                }, function (err) {
                    WinJS.log("Error returned by WebAuth broker: " + err, "Web Authentication SDK Sample", "error");
                    WinJS.log(" Error Message: " + err.message, "Web Authentication SDK Sample", "error");
                    authzInProgress = false;
                });
        }
    }
    function continueWebAuthentication(args) {
        var result = args[0].webAuthenticationResult;

        if (result.responseStatus === Windows.Security.Authentication.Web.WebAuthenticationStatus.success) {
            document.getElementById("FlickrReturnedToken").value = result.responseData;
        }
        else if (result.responseStatus === Windows.Security.Authentication.Web.WebAuthenticationStatus.errorHttp) {
            WinJS.log("Error returned: " + result.responseErrorDetail, "Web Authentication SDK Sample", "error");
        }
        else {
            WinJS.log("Status returned by WebAuth broker: " + result.responseStatus, "Web Authentication SDK Sample", "error");
        }
        authzInProgress = false;

    }
})();
