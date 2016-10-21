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

#include "pch.h"
#include "Scenario7_B2B.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Services::Store;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Headers;

// Note: 'tenant', 'clientId', "clientSecret" and "appIdUri" will need to be updated based on your particular
// Azure Active Directory configuration. See the README for more information.
// https://msdn.microsoft.com/en-us/library/azure/dn645542.aspx
String^ tenantId = "00000000-0000-0000-0000-000000000000";
String^ clientId = "00000000-0000-0000-0000-000000000000";
String^ clientSecret = "00000000000000000000000000000000000000000000";
String^ appIdUri = "https://contoso.onmicrosoft.com";

Scenario7_B2B::Scenario7_B2B()
{
    InitializeComponent();

    storeContext = StoreContext::GetDefault();
}


void Scenario7_B2B::GetCustomerCollectionsId(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    create_task(GetTokenFromAzureOAuthAsync())
        .then([this](String^ aadToken)
    {
        if (aadToken != nullptr)
        {
            create_task(storeContext->GetCustomerCollectionsIdAsync(aadToken, "kim@example.com"))
                .then([this](String^ id)
            {
                Output->Text = id;
                if (id == nullptr)
                {
                    rootPage->NotifyUser("GetCustomerCollectionsIdAsync failed.", NotifyType::ErrorMessage);
                }
            });
        }
    });
}

void Scenario7_B2B::GetCustomerPurchaseId(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    create_task(GetTokenFromAzureOAuthAsync())
        .then([this](String^ aadToken)
    {
        if (aadToken != nullptr)
        {
            create_task(storeContext->GetCustomerPurchaseIdAsync(aadToken, "kim@example.com"))
                .then([this](String^ id)
            {
                Output->Text = id;
                if (id == nullptr)
                {
                    rootPage->NotifyUser("GetCustomerCollectionsIdAsync failed.", NotifyType::ErrorMessage);
                }
            });
        }
    });
}

task<String^> Scenario7_B2B::GetTokenFromAzureOAuthAsync()
{
    IHttpContent^ content = ref new HttpFormUrlEncodedContent(ref new Map<String^, String^>({
        std::pair<String^, String^>("grant_type", "client_credentials"),
        std::pair<String^, String^>("client_id", clientId),
        std::pair<String^, String^>("client_secret", clientSecret),
        std::pair<String^, String^>("resource", appIdUri),
    }));

    content->Headers->ContentType = ref new HttpMediaTypeHeaderValue("application/x-www-form-urlencoded");

    HttpClient^ client = ref new HttpClient();
    Uri^ uri = ref new Uri("https://login.microsoftonline.com/" + tenantId + "/oauth2/token");
    // Networking errors in PostAsync are reported as exceptions.
    return create_task(client->PostAsync(uri, content))
        .then([this](HttpResponseMessage^ result)
    {
        return create_task(result->Content->ReadAsStringAsync());
    }).then([this](String^ responseString)
    {
        // JSON parsing errors are reported as exceptions.
        JsonValue^ response = JsonValue::Parse(responseString);
        return response->GetObject()->GetNamedString("access_token");
    }).then([this](task<String^> previousTask)
    {
        String^ result;
        try
        {
            // Any exceptions in PostAsync or JSON parsing will be rethrown here.
            result = previousTask.get();
        }
        catch (Exception^ ex)
        {
            this->rootPage->NotifyUser("There were problems getting an Azure OAuth token.", NotifyType::ErrorMessage);
        }
        return result;
    });
}
