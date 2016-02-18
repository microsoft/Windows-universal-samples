//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var CurrentApp = Windows.ApplicationModel.Store.CurrentApp;
    var HttpClient = Windows.Web.Http.HttpClient;
    var HttpFormUrlEncodedContent = Windows.Web.Http.HttpFormUrlEncodedContent;
    var HttpMediaTypeHeaderValue = Windows.Web.Http.Headers.HttpMediaTypeHeaderValue;
    var Uri = Windows.Foundation.Uri;

    // Note: The methods on CurrentApp require your application to be published in the store and you must have a
    // valid license.  The best way to accomlish this is to publish your application once privately.  Then,
    // download it from the store once, and deploy over the top of it with this application after you have associated
    // it with your store application.
    var IAP_E_UNEXPECTED = -2143330041;
    var IAP_E_UNEXPECTED_MESSAGE = "This scenario will not run correctly if you haven't associated it with an existing published application.";

    // Note: 'tenant', 'clientId', "clientSecret" and "appIdUri" will need to be updated based on your particular
    // Azure Active Directory configuration. See the README for more information.
    // https://msdn.microsoft.com/en-us/library/azure/dn645542.aspx
    var tenantId = "00000000-0000-0000-0000-000000000000";
    var clientId = "00000000-0000-0000-0000-000000000000";
    var clientSecret = "00000000000000000000000000000000000000000000";
    var appIdUri = "https://contoso.onmicrosoft.com";

    var page = WinJS.UI.Pages.define("/html/scenario9-b2b.html", {
        ready: function (element, options) {
            document.getElementById("getCustomerCollectionsId").addEventListener("click", getCustomerCollectionsId);
            document.getElementById("getCustomerPurchaseId").addEventListener("click", getCustomerPurchaseId);
        }
    });

    function getCustomerCollectionsId() {
        var token = getTokenFromAzureOAuthAsync().then(function (aadToken) {
            if (aadToken !== undefined) {
                return CurrentApp.getCustomerCollectionsIdAsync(aadToken, "kim@example.com");
            }
        }).done(function (result) {
            output.innerText = result;
        }, function (err) {
            if (err.number == IAP_E_UNEXPECTED) {
                WinJS.log && WinJS.log("GetCustomerPurchaseIdAsync failed: " + IAP_E_UNEXPECTED_MESSAGE, "sample", "error");
            }
            else {
                WinJS.log && WinJS.log("GetCustomerPurchaseIdAsync failed: " + err, "sample", "error");
            }
        });
    }

    function getCustomerPurchaseId() {
        var token = getTokenFromAzureOAuthAsync().then(function (aadToken) {
            if (aadToken !== undefined) {
                return CurrentApp.getCustomerPurchaseIdAsync(aadToken, "kim@example.com");
            }
        }).done(function (result) {
            output.innerText = result;
        }, function (err) {
            if (err.number == IAP_E_UNEXPECTED) {
                WinJS.log && WinJS.log("GetCustomerPurchaseIdAsync failed: " + IAP_E_UNEXPECTED_MESSAGE, "sample", "error");
            }
            else {
                WinJS.log && WinJS.log("GetCustomerPurchaseIdAsync failed: " + err, "sample", "error");
            }
        });
    }

    function getTokenFromAzureOAuthAsync() {
        var map = Windows.Foundation.Collections.StringMap();
        map["grant_type"] = "client_credentials";
        map["client_id"] = clientId;
        map["client_secret"] = clientSecret;
        map["resource"] = appIdUri;

        var client = new HttpClient();
        var content = new HttpFormUrlEncodedContent(map);

        content.headers.contentType = new HttpMediaTypeHeaderValue("application/x-www-form-urlencoded");

        var uri = new Uri("https://login.microsoftonline.com/" + tenantId + "/oauth2/token");
        return client.postAsync(uri, content)
            .then(function (result) { return result.content.readAsStringAsync(); })
            .then(function (responseString) {
                var response = JSON.parse(responseString);
                if (response.access_token === undefined) {
                    WinJS.log && WinJS.log("Failed to load Azure OAuth Token.", "sample", "error");
                }

                return response.access_token;
            }, function (err) {
                WinJS.log && WinJS.log("Failed to load Azure OAuth Token: " + err, "sample", "error");
            });
    }
})();
