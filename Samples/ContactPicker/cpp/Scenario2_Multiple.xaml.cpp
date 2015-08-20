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
#include "Scenario2_Multiple.xaml.h"

using namespace Concurrency;
using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::Contacts;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2_Multiple::Scenario2_Multiple()
{
    InitializeComponent();
}

void Scenario2_Multiple::ClearOutputs()
{
    // Reset the output fields.
    OutputText->Text = "";
    rootPage->NotifyUser("", NotifyType::StatusMessage);
}

void Scenario2_Multiple::ReportContactResults(IVector<Contact^>^ contacts)
{
    if (contacts != nullptr && contacts->Size > 0)
    {
        for (Contact^ contact : contacts)
        {
            OutputText->Text += MainPage::GetContactResult(contact) + "\n";
        }
    }
    else
    {
        rootPage->NotifyUser("No contacts were selected.", NotifyType::ErrorMessage);
    }
}

void Scenario2_Multiple::PickContactsEmail()
{
    ClearOutputs();

    // Ask the user to pick contact email addresses.
    auto contactPicker = ref new ContactPicker();
    contactPicker->DesiredFieldsWithContactFieldType->Append(ContactFieldType::Email);

    create_task(contactPicker->PickContactsAsync()).then([this](IVector<Contact^>^ contacts)
    {
        ReportContactResults(contacts);
    });
}

void Scenario2_Multiple::PickContactsPhone()
{
    ClearOutputs();

    // Ask the user to pick contact phone numbers.
    auto contactPicker = ref new ContactPicker();
    contactPicker->DesiredFieldsWithContactFieldType->Append(ContactFieldType::PhoneNumber);

    create_task(contactPicker->PickContactsAsync()).then([this](IVector<Contact^>^ contacts)
    {
        ReportContactResults(contacts);
    });
}
