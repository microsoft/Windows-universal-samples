//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("oAuthTwitterLaunch").addEventListener("click", launchTwitterWebAuth, false);

            // Continuation handlers are specific to Windows Phone.
            if (options && options.activationKind === Windows.ApplicationModel.Activation.ActivationKind.webAuthenticationBrokerContinuation) {
                continueWebAuthentication(options.activatedEventArgs);
            }
        }
    });

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

    var authzInProgress = false;

    function launchTwitterWebAuth() {
        var twitterURL = "https://api.twitter.com/oauth/request_token";

        // Get all the parameters from the user
        var clientID = document.getElementById("TwitterClientID").value;
        if (clientID === null || clientID === "") {
            WinJS.log("Please enter a ClientID for Twitter App", "Web Authentication SDK Sample", "error");
            return;
        }

        var clientSecret = document.getElementById("TwitterSecret").value;
        if (clientSecret === null || clientSecret === "") {
            WinJS.log("Please enter a Secret for Twitter App", "Web Authentication SDK Sample", "error");
            return;
        }

        var callbackURL = document.getElementById("TwitterCallbackURL").value;
        if (!isValidUriString(callbackURL)) {
            WinJS.log("Please enter a Callback URL for Twitter", "Web Authentication SDK Sample", "error");
            return;
        }

        if (authzInProgress) {
            WinJS.log("Authorization already in Progress...", "Web Authentication SDK Sample", "status");
            return;
        }

        // Acquiring a request token
        var oauth_token = getTwitterRequestToken(callbackURL, clientID, clientSecret);

        // Send the user to authorization
        twitterURL = "https://api.twitter.com/oauth/authorize?oauth_token=" + oauth_token;

        WinJS.log("Navigating to: " + twitterURL, "Web Authentication SDK Sample", "status")
        var startURI = new Windows.Foundation.Uri(twitterURL);
        var endURI = new Windows.Foundation.Uri(callbackURL);

        authzInProgress = true;

        if (Windows.Security.Authentication.Web.WebAuthenticationBroker.authenticateAndContinue) {
            Windows.Security.Authentication.Web.WebAuthenticationBroker.authenticateAndContinue(startURI, endURI, null, Windows.Security.Authentication.Web.WebAuthenticationOptions.none);
        }
        else {
            Windows.Security.Authentication.Web.WebAuthenticationBroker.authenticateAsync(
                Windows.Security.Authentication.Web.WebAuthenticationOptions.none, startURI, endURI)
                .done(function (result) {
                    document.getElementById("TwitterReturnedToken").value = result.responseData;
                    WinJS.log("Status returned by WebAuth broker: " + result.responseStatus, "Web authentication SDK Sample", "status");
                    getTwitterUserName(result.responseData, clientID, clientSecret);
                    if (result.responseStatus === Windows.Security.Authentication.Web.WebAuthenticationStatus.errorHttp) {
                        WinJS.log("Error returned: " + result.responseErrorDetail, "Web Autentication SDK Sample", "error");
                    }
                    authzInProgress = false;
                }, function (err) {
                    WinJS.log("Error returned by WebAuth broker: " + err, "Web Authentication SDK Sample", "error");
                    authzInProgress = false;
                });
        }
    }

    //
    //This function is Continuation handler for Windows Phone App
    //
    function continueWebAuthentication(args) {
        var result = args[0].webAuthenticationResult;

        if (result.responseStatus === Windows.Security.Authentication.Web.WebAuthenticationStatus.success) {
            document.getElementById("TwitterReturnedToken").value = result.responseData;
        }
        else if (result.responseStatus === Windows.Security.Authentication.Web.WebAuthenticationStatus.errorHttp) {
            WinJS.log("Error returned: " + result.responseErrorDetail, "Web Authentication SDK Sample", "error");
        }
        else {
            WinJS.log("Status returned by WebAuth broker: " + result.responseStatus, "Web Authentication SDK Sample", "error");
        }
        authzInProgress = false;

    }

    /// <summary>
    /// This function extracts oauth_token and oauth_verifier from the response returned from web authentication broker
    /// and uses that token to get Twitter access token. 
    /// </summary>
    function getTwitterUserName(webAuthResultResponseData, clientId, clientSecret) {

        //
        // Acquiring a access_token first.
        //
        var responseData = webAuthResultResponseData.substring(webAuthResultResponseData.indexOf("oauth_token"));
        var request_token;
        var oauth_verifier;
        var keyValPairs = responseData.split("&");

        for (var i = 0; i < keyValPairs.length; i++) {
            var splits = keyValPairs[i].split("=");
            switch (splits[0]) {
                case "oauth_token":
                    request_token = splits[1];
                    break;
                case "oauth_verifier":
                    oauth_verifier = splits[1];
                    break;
            }
        }

        var twitterURL = "https://api.twitter.com/oauth/access_token";
        var timeStamp = getTimeStamp();
        var nonce = getNonce();

        var sigBaseStringParams = "oauth_consumer_key=" + clientId;
        sigBaseStringParams += "&" + "oauth_nonce=" + nonce;
        sigBaseStringParams += "&" + "oauth_signature_method=HMAC-SHA1";
        sigBaseStringParams += "&" + "oauth_timestamp=" + timeStamp;
        sigBaseStringParams += "&" + "oauth_token=" + request_token;
        sigBaseStringParams += "&" + "oauth_version=1.0";
        var sigBaseString = "POST&";
        sigBaseString += encodeURIComponent(twitterURL) + "&" + encodeURIComponent(sigBaseStringParams);

        var keyText = clientSecret + "&";
        var signature = getSignature(sigBaseString, keyText);
        var authorizationHeaderParams = "OAuth oauth_consumer_key=\"" + clientId + "\", oauth_nonce=\"" + nonce + "\", oauth_signature_method=\"HMAC-SHA1\", oauth_signature=\"" + encodeURIComponent(signature) + "\", oauth_timestamp=\"" + timeStamp + "\", oauth_token=\"" + encodeURIComponent(request_token) + "\", oauth_version=\"1.0\"";

        var request = new XMLHttpRequest();
        request.open("POST", twitterURL, false);
        request.setRequestHeader("Authorization", authorizationHeaderParams);
        request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        request.send("oauth_verifier=" + oauth_verifier);
        var response = request.responseText;

        var access_token;
        var oauth_token_secret;
        var screen_name;
        keyValPairs = response.split("&");

        for (var j = 0; j < keyValPairs.length; j++) {
            var tokens = keyValPairs[j].split("=");
            switch (tokens[0]) {
                case "oauth_token":
                    access_token = tokens[1];
                    break;
                case "oauth_token_secret":
                    oauth_token_secret = tokens[1];
                    break;
                case "screen_name":
                    screen_name = tokens[1];
                    break;
            }
        }

        //You can store access_token and oauth_token_secretfor further use. see "Account Management" scenario for usage.
        WinJS.log("access_token = " + access_token + "\r\n" + "oauth_token_secret = " + oauth_token_secret + "\r\n" + screen_name + " is connected!!", "Web Authentication SDK Sample", "status");
    }


    function getTwitterRequestToken(callbackURL, clientID, clientSecret) {

        var twitterURL = "https://api.twitter.com/oauth/request_token";
        var timestamp = getTimeStamp();
        var nonce = getNonce();

        // Compute base signature string and sign it.
        //    This is a common operation that is required for all requests even after the token is obtained.
        //    Parameters need to be sorted in alphabetical order
        //    Keys and values should be URL Encoded.
        var sigBaseStringParams = "oauth_callback=" + encodeURIComponent(callbackURL);
        sigBaseStringParams += "&" + "oauth_consumer_key=" + clientID;
        sigBaseStringParams += "&" + "oauth_nonce=" + nonce;
        sigBaseStringParams += "&" + "oauth_signature_method=HMAC-SHA1";
        sigBaseStringParams += "&" + "oauth_timestamp=" + timestamp;
        sigBaseStringParams += "&" + "oauth_version=1.0";
        var sigBaseString = "POST&";
        sigBaseString += encodeURIComponent(twitterURL) + "&" + encodeURIComponent(sigBaseStringParams);

        var keyText = clientSecret + "&";
        var signature = getSignature(sigBaseString, keyText);
        var dataToPost = "OAuth oauth_callback=\"" + encodeURIComponent(callbackURL) + "\", oauth_consumer_key=\"" + clientID + "\", oauth_nonce=\"" + nonce + "\", oauth_signature_method=\"HMAC-SHA1\", oauth_timestamp=\"" + timestamp + "\", oauth_version=\"1.0\", oauth_signature=\"" + encodeURIComponent(signature) + "\"";
        var response = sendPostRequest(twitterURL, dataToPost);
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
        WinJS.log("oauth_token = " + oauth_token + "\r\noauth_token = " + oauth_token_secret, "Web Authentication SDK Sample", "status");

        return oauth_token;
    }

    function getNonce() {
        var nonce = Math.random();
        nonce = Math.floor(nonce * 1000000000);
        return nonce;
    }

    function getTimeStamp() {
        var timestamp = Math.round(new Date().getTime() / 1000.0);
        return timestamp;
    }

    function getSignature(sigBaseString, keyText) {
        var keyMaterial = Windows.Security.Cryptography.CryptographicBuffer.convertStringToBinary(keyText, Windows.Security.Cryptography.BinaryStringEncoding.Utf8);
        var macAlgorithmProvider = Windows.Security.Cryptography.Core.MacAlgorithmProvider.openAlgorithm("HMAC_SHA1");
        var key = macAlgorithmProvider.createKey(keyMaterial);
        var tbs = Windows.Security.Cryptography.CryptographicBuffer.convertStringToBinary(sigBaseString, Windows.Security.Cryptography.BinaryStringEncoding.Utf8);
        var signatureBuffer = Windows.Security.Cryptography.Core.CryptographicEngine.sign(key, tbs);
        var signature = Windows.Security.Cryptography.CryptographicBuffer.encodeToBase64String(signatureBuffer);

        return signature;
    }
})();
