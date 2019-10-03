//// Copyright (c) Microsoft Corporation. All rights reserved

/// <summary>
/// A page for seventh scenario.  For more details on the business to business APIs, see:
/// https://msdn.microsoft.com/en-us/library/windows/apps/mt609002.aspx
/// </summary>
(function () {
    "use strict";

    var HttpClient = Windows.Web.Http.HttpClient;
    var HttpFormUrlEncodedContent = Windows.Web.Http.HttpFormUrlEncodedContent;
    var HttpMediaTypeHeaderValue = Windows.Web.Http.Headers.HttpMediaTypeHeaderValue;
    var Uri = Windows.Foundation.Uri;

    // Note: 'tenant', 'clientId', "clientSecret" and "appIdUri" will need to be updated based on your particular
    // Azure Active Directory configuration. See the README for more information.
    // https://msdn.microsoft.com/en-us/library/azure/dn645542.aspx
    var tenantId = "00000000-0000-0000-0000-000000000000";
    var clientId = "00000000-0000-0000-0000-000000000000";
    var clientSecret = "00000000000000000000000000000000000000000000";
    var appIdUri = "https://contoso.onmicrosoft.com";

    var storeContext = Windows.Services.Store.StoreContext.getDefault();

    var page = WinJS.UI.Pages.define("/html/scenario7-b2b.html", {
        ready: function (element, options) {
            document.getElementById("getCustomerCollectionsId").addEventListener("click", getCustomerCollectionsId);
            document.getElementById("getCustomerPurchaseId").addEventListener("click", getCustomerPurchaseId);
        }
    });

    function getCustomerCollectionsId() {
        var token = getTokenFromAzureOAuthAsync().done(function (aadToken) {
            if (aadToken) {
                storeContext.getCustomerCollectionsIdAsync(aadToken, "kim@example.com")
                    .done(function (result) {
                        output.innerText = result;
                        if (!result) {
                            WinJS.log && WinJS.log("getCustomerCollectionsIdAsync failed.", "sample", "error");
                        }
                    });
            }
        });
    }

    function getCustomerPurchaseId() {
        var token = getTokenFromAzureOAuthAsync().done(function (aadToken) {
            if (aadToken) {
                storeContext.getCustomerPurchaseIdAsync(aadToken, "kim@example.com")
                    .done(function (result) {
                        output.innerText = result;
                        if (!result) {
                            WinJS.log && WinJS.log("getCustomerPurchaseIdAsync failed.", "sample", "error");
                        }
                    });
            }
        });
    }

    function getTokenFromAzureOAuthAsync() {
        var formContent = Windows.Foundation.Collections.StringMap();
        formContent.grant_type = "client_credentials";
        formContent.client_id = clientId;
        formContent.client_secret = clientSecret;
        formContent.resource = appIdUri;

        var content = new HttpFormUrlEncodedContent(formContent);
        content.headers.contentType = new HttpMediaTypeHeaderValue("application/x-www-form-urlencoded");

        var client = new HttpClient();
        var uri = new Uri(`https://login.microsoftonline.com/${tenantId}/oauth2/token`);
        return client.postAsync(uri, content)
            .then(function (result) { return result.content.readAsStringAsync(); })
            .then(function (responseString) {
                var response = JSON.parse(responseString);
                return response.access_token.toString();
            }).then(null, function error() {
                // Any exceptions in postAsync or JSON parsing will be handled here.
                WinJS.log && WinJS.log("Failed to load Azure OAuth Token", "sample", "error");
                return "";
            });
    }
})();
