//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario9.xaml.cpp
// Implementation of the Scenario9 class
//

#include "pch.h"
#include "Scenario9_B2B.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Headers;

// Note: The methods on CurrentApp require your application to be published in the store and you must have a
// valid license.  The best way to accomlish this is to publish your application once privately.  Then,
// download it from the store once, and deploy over the top of it with this application after you have associated
// it with your store application.
const unsigned int IAP_E_UNEXPECTED = 0x803f6107;
const String^ IAP_E_UNEXPECTED_MESSAGE = "This scenario will not run correctly if you haven't associated it with an existing published application.";

// Note: 'tenant', 'clientId', "clientSecret" and "appIdUri" will need to be updated based on your particular
// Azure Active Directory configuration. See the README for more information.
// https://msdn.microsoft.com/en-us/library/azure/dn645542.aspx
String^ tenantId = "00000000-0000-0000-0000-000000000000";
String^ clientId = "00000000-0000-0000-0000-000000000000";
String^ clientSecret = "00000000000000000000000000000000000000000000";
String^ appIdUri = "https://contoso.onmicrosoft.com";

Scenario9_B2B::Scenario9_B2B()
{
    InitializeComponent();
}

void Scenario9_B2B::GetCustomerCollectionsId()
{
    create_task(GetTokenFromAzureOAuthAsync())
        .then([this](String^ aadToken)
    {
        if (aadToken != "")
        {
            return create_task(CurrentApp::GetCustomerCollectionsIdAsync(aadToken, "kim@example.com"));
        }
        else
        {
            return task_from_result(aadToken);
        }
    }).then([this](String^ results)
    {
        this->DisplayResults(results);
    }).then([this](task<void> t)
    {
        try
        {
            t.get();
        }
        catch (Platform::Exception^ ex)
        {
            if (ex->HResult == IAP_E_UNEXPECTED)
            {
                this->rootPage->NotifyUser(
                    "GetCustomerPurchaseIdAsync failed: " + IAP_E_UNEXPECTED_MESSAGE,
                    NotifyType::ErrorMessage);
            }
            else
            {
                this->rootPage->NotifyUser(
                    "GetCustomerPurchaseIdAsync failed.",
                    NotifyType::ErrorMessage);
            }
        }
    });
}

void Scenario9_B2B::GetCustomerPurchaseId()
{
    create_task(GetTokenFromAzureOAuthAsync())
        .then([this](String^ aadToken)
    {
        if (aadToken != "")
        {
            return create_task(CurrentApp::GetCustomerPurchaseIdAsync(aadToken, "kim@example.com"));
        }
        else
        {
            return task_from_result(aadToken);
        }
    }).then([this](String^ results)
    {
        this->DisplayResults(results);
    }).then([this](task<void> t)
    {
        try
        {
            t.get();
        }
        catch (Platform::Exception^ ex)
        {
            if (ex->HResult == IAP_E_UNEXPECTED)
            {
                this->rootPage->NotifyUser(
                    "GetCustomerPurchaseIdAsync failed: " + IAP_E_UNEXPECTED_MESSAGE,
                    NotifyType::ErrorMessage);
            }
            else
            {
                this->rootPage->NotifyUser(
                    "GetCustomerPurchaseIdAsync failed.",
                    NotifyType::ErrorMessage);
            }
        }
    });
}

task<String^> Scenario9_B2B::GetTokenFromAzureOAuthAsync()
{
    auto map = ref new Platform::Collections::Map<String^, String^>();
    map->Insert("grant_type", "client_credentials");
    map->Insert("client_id", clientId);
    map->Insert("client_secret", clientSecret);
    map->Insert("resource", appIdUri);

    HttpClient^ client = ref new HttpClient();
    IHttpContent^ content = ref new HttpFormUrlEncodedContent(map);

    content->Headers->ContentType = ref new HttpMediaTypeHeaderValue("application/x-www-form-urlencoded");

    Uri^ uri = ref new Uri("https://login.microsoftonline.com/" + tenantId + "/oauth2/token");
    return create_task(client->PostAsync(uri, content))
        .then([this](HttpResponseMessage^ result)
    {
        return create_task(result->Content->ReadAsStringAsync());
    }).then([this](String^ responseString)
    {
        String^ result;
        try
        {
            JsonValue^ response = JsonValue::Parse(responseString);
            result = response->GetObject()->GetNamedString("access_token");
        }
        catch (Platform::Exception^ ex)
        {
            this->rootPage->NotifyUser("There were problems getting an Azure OAuth token.", NotifyType::ErrorMessage);
            result = "";
        }

        return result;
    });
}

void Scenario9_B2B::DisplayResults(String^ results)
{
    if (results != "")
    {
        this->Output->Text = results;
    }
}
