//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("oAuthFacebookLaunch").addEventListener("click", launchFacebookWebAuth, false);
            
            // Continuation handlers are specific to Windows Phone.
            if (options && options.activationKind === Windows.ApplicationModel.Activation.ActivationKind.webAuthenticationBrokerContinuation) {
                continueWebAuthentication(options.activatedEventArgs);
            }
        }
    });

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

    function launchFacebookWebAuth() {
        var facebookURL = "https://www.facebook.com/dialog/oauth?client_id=";

        var clientID = document.getElementById("FacebookClientID").value;
        if (clientID === null || clientID === "") {
            WinJS.log("Enter a ClientID", "Web Authentication SDK Sample", "error");
            return;
        }

        var callbackURL = document.getElementById("FacebookCallbackURL").value;
        if (!isValidUriString(callbackURL)) {
            WinJS.log("Enter a valid Callback URL for Facebook", "Web Authentication SDK Sample", "error");
            return;
        }

        facebookURL += clientID + "&redirect_uri=" + encodeURIComponent(callbackURL) + "&scope=read_stream&display=popup&response_type=token";

        if (authzInProgress) {
            WinJS.log("Authorization already in Progress...", "Web Authentication SDK Sample", "status");
            return;
        }

        var startURI = new Windows.Foundation.Uri(facebookURL);
        var endURI = new Windows.Foundation.Uri(callbackURL);

        WinJS.log("Navigating to: " + facebookURL, "Web Authentication SDK Sample", "status");

        authzInProgress = true;

        if (Windows.Security.Authentication.Web.WebAuthenticationBroker.authenticateAndContinue) {
            Windows.Security.Authentication.Web.WebAuthenticationBroker.authenticateAndContinue(startURI, endURI, null, Windows.Security.Authentication.Web.WebAuthenticationOptions.none);
        }
        else {
            Windows.Security.Authentication.Web.WebAuthenticationBroker.authenticateAsync(
                Windows.Security.Authentication.Web.WebAuthenticationOptions.none, startURI, endURI)
                .done(function (result) {
                    document.getElementById("FacebookReturnedToken").value = result.responseData;
                    WinJS.log("Status returned by WebAuth broker: " + result.responseStatus, "Web Authentication SDK Sample", "status");
                    getfacebookUserName(result.responseData);
                    if (result.responseStatus === Windows.Security.Authentication.Web.WebAuthenticationStatus.errorHttp) {
                        WinJS.log("Error returned: " + result.responseErrorDetail, "Web Authentication SDK Sample", "error");
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
            document.getElementById("FacebookReturnedToken").value = result.responseData;
        }
        else if (result.responseStatus === Windows.Security.Authentication.Web.WebAuthenticationStatus.errorHttp) {
            document.getElementById("FacebookDebugArea").value += "Error returned: " + result.responseErrorDetail + "\r\n";
        }
        else {
            document.getElementById("FacebookDebugArea").value += "Status returned by WebAuth broker: " + result.responseStatus + "\r\n";
        }
        authzInProgress = false;

    }

    /// <summary>
    /// This function extracts access_token from the response returned from web authentication broker
    /// and uses that token to get user information using facebook graph api. 
    /// </summary>
    function getfacebookUserName(webAuthResultResponseData) {

        var responseData = webAuthResultResponseData.substring(webAuthResultResponseData.indexOf("access_token"));
        var keyValPairs = responseData.split("&");
        var access_token;
        var expires_in;
        for (var i = 0; i < keyValPairs.length; i++) {
            var splits = keyValPairs[i].split("=");
            switch (splits[0]) {
                case "access_token":
                    access_token = splits[1]; //You can store access token locally for further use. See "Account Management" scenario for usage.
                    break;
                case "expires_in":
                    expires_in = splits[1];
                    break;
            }
        }

        document.getElementById("FacebookDebugArea").value += "\r\naccess_token = " + access_token + "\r\n";
        var client = new Windows.Web.Http.HttpClient();
        client.getStringAsync(new Windows.Foundation.Uri("https://graph.facebook.com/me?access_token=" + access_token)).done(function (result) {
            var userInfo = JSON.parse(result);
            document.getElementById("FacebookDebugArea").value += userInfo.name + " is connected!! \r\n";
        });
    }
})();
