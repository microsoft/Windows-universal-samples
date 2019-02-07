//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var WebAuthenticationBroker = Windows.Security.Authentication.Web.WebAuthenticationBroker;
    var WebAuthenticationOptions = Windows.Security.Authentication.Web.WebAuthenticationOptions;
    var WebAuthenticationStatus = Windows.Security.Authentication.Web.WebAuthenticationStatus;

    var facebookUserName;
    var facebookClientID;
    var facebookCallbackURL;
    var facebookReturnedToken;

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            facebookUserName = document.getElementById("facebookUserName");
            facebookClientID = document.getElementById("facebookClientID");
            facebookCallbackURL = document.getElementById("facebookCallbackURL");
            facebookReturnedToken = document.getElementById("facebookReturnedToken");

            document.getElementById("oAuthFacebookLaunch").addEventListener("click", launchFacebookWebAuth, false);

            // Use these SIDs to register the app with Facebook.
            document.getElementById("windowsStoreSidText").textValue = WebAuthenticationBroker.getCurrentApplicationCallbackUri().host;
            document.getElementById("windowsPhoneStoreSidText").textValue = "5119c4a4-bfe3-404f-981b-9ad93ac49802"; // copied from Package.appxmanifest
        }
    });

    function tryParseUri(uriString) {
        try {
            return new Windows.Foundation.Uri(uriString);
        } catch (err) {
            return null;
        }
    }

    var authzInProgress = false;

    function launchFacebookWebAuth() {
        if (authzInProgress) {
            return;
        }

        facebookUserName.textContent = "";
        facebookReturnedToken.textContent = "";

        var clientID = facebookClientID.value;
        if (clientID === null || clientID === "") {
            WinJS.log("Enter a ClientID", "Web Authentication SDK Sample", "error");
            return;
        }

        var callbackUri = tryParseUri(facebookCallbackURL.value);
        if (!callbackUri) {
            WinJS.log("Enter a valid Callback URL for Facebook", "Web Authentication SDK Sample", "error");
            return;
        }

        var facebookStartUri = new Windows.Foundation.Uri(`https://www.facebook.com/dialog/oauth?` +
            `client_id=${encodeURIComponent(clientID)}&redirect_uri=${encodeURIComponent(callbackUri)}` +
            `&scope=read_stream&display=popup&response_type=token`);

        WinJS.log(`Navigating to ${facebookStartUri}`, "Web Authentication SDK Sample", "status");

        authzInProgress = true;

        WebAuthenticationBroker.authenticateAsync(WebAuthenticationOptions.none, facebookStartUri, callbackUri)
            .then(function (result) {
                facebookReturnedToken.textValue = result.responseData;
                if (result.responseStatus === WebAuthenticationStatus.success) {
                    return getFacebookUserNameAsync(result.responseData);
                } else if (result.responseStatus === WebAuthenticationStatus.errorHttp) {
                    WinJS.log(`HTTP error: ${result.responseErrorDetail}`, "Web Authentication SDK Sample", "error");
                } else {
                    WinJS.log(`Error: ${result.responseStatus}`, "Web Authentication SDK Sample", "status");
                }
            }, function (err) {
                WinJS.log("Error returned by WebAuth broker: " + err, "Web Authentication SDK Sample", "error");
            }).done(function () {
                authzInProgress = false;
            });
    }

    /// <summary>
    /// This function extracts access_token from the response returned from web authentication broker
    /// and uses that token to get user information using facebook graph api. 
    /// </summary>
    function getFacebookUserNameAsync(responseData) {
        var decoder = new Windows.Foundation.WwwFormUrlDecoder(responseData);
        var error = SdkSample.tryGetFormValue(decoder, "error");
        if (error) {
            facebookUserName.textContent = `Error: ${error}`;
            return;
        }

        // You can store access token locally for further use.
        var access_token = decoder.getFirstValueByName("access_token");
        var expires_in = SdkSample.tryGetFormValue(decoder, "expires_in"); // expires_in is optional

        var client = new Windows.Web.Http.HttpClient();
        var uri = new Windows.Foundation.Uri(`https://graph.facebook.com/me?access_token=${encodeURIComponent(access_token)}`);
        return client.tryGetStringAsync(uri).then(function (result) {
            if (result.succeeded) {
                var userInfo = JSON.parse(result.value);
                facebookUserName.textContent = userInfo.name;
            } else {
                facebookUserName.textContent = "Error contacting Facebook";
            }
        });
    }
})();
