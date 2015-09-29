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

using namespace Platform;
using namespace Windows::ApplicationModel::Contacts;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Pick a contact", "SDKTemplate.Scenario1_Single" },
    { "Pick multiple contacts", "SDKTemplate.Scenario2_Multiple" },
};

String^ MainPage::GetContactResult(Contact^ contact)
{
    String^ result = "Display name: " + contact->DisplayName + "\n";

    for (ContactEmail^ email : contact->Emails)
    {
        result += "Email address (" + email->Kind.ToString() + "): " + email->Address + "\n";
    }

    for (ContactPhone^ phone : contact->Phones)
    {
        result += "Phone (" + phone->Kind.ToString() + "): " + phone->Number + "\n";
    }

    return result;
}