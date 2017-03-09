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
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "GET Text With Cache Control", "SDKTemplate.Scenario1" },
    { "GET Stream", "SDKTemplate.Scenario2" },
    { "GET List", "SDKTemplate.Scenario3" },
    { "POST Text", "SDKTemplate.Scenario4"},
    { "POST Stream","SDKTemplate.Scenario5"},
    { "POST Multipart", "SDKTemplate.Scenario6"},
    { "POST Stream With Progress", "SDKTemplate.Scenario7" },
    { "POST Custom Content", "SDKTemplate.Scenario8"},
    { "Get Cookies", "SDKTemplate.Scenario9"},
    { "Set Cookies", "SDKTemplate.Scenario10"},
    { "Delete Cookies", "SDKTemplate.Scenario11"},
    { "Disable Cookies","SDKTemplate.Scenario12"},
    { "Retry Filter","SDKTemplate.Scenario13"},
    { "Metered Connection Filter", "SDKTemplate.Scenario14"},
    { "Server Certificate Validation", "SDKTemplate.Scenario15" }
};

bool MainPage::TryGetUri(Platform::String^ uriString, Windows::Foundation::Uri^* uri)
{
    // Create a Uri instance and catch exceptions related to invalid input. This method returns 'true'
    // if the Uri instance was successfully created and 'false' otherwise.  
    try
    {
        Windows::Foundation::Uri^ localUri = ref new Windows::Foundation::Uri(Helpers::Trim(uriString));

        if (localUri->Host == nullptr)
        {
            return false;
        }

        if (localUri->SchemeName != "http" && localUri->SchemeName != "https")
        {
            return false;
        }

        *uri = localUri;
        return true;
    }
    catch (Platform::NullReferenceException^ exception)
    {
        NotifyUser("Error: URI must not be null or empty.", NotifyType::ErrorMessage);
    }
    catch (Platform::InvalidArgumentException^ exception)
    {
        NotifyUser("Error: Invalid URI", NotifyType::ErrorMessage);
    }
    return false;
}