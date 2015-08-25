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
#include "Scenario1_Single.xaml.h"

using namespace Concurrency;
using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::Contacts;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario1_Single::Scenario1_Single() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario1_Single::ClearOutputs()
{
    // Reset the output fields.
    OutputText->Text = "";
    rootPage->NotifyUser("", NotifyType::StatusMessage);
}

void Scenario1_Single::ReportContactResult(Contact^ contact)
{
    if (contact != nullptr)
    {
        OutputText->Text = MainPage::GetContactResult(contact);
    }
    else
    {
        rootPage->NotifyUser("No contact was selected.", NotifyType::ErrorMessage);
    }
}

void Scenario1_Single::PickContactEmail()
{
    ClearOutputs();

    // Ask the user to pick a contact email address.
    auto contactPicker = ref new ContactPicker();
    contactPicker->DesiredFieldsWithContactFieldType->Append(ContactFieldType::Email);

    create_task(contactPicker->PickContactAsync()).then([this](Contact^ contact)
    {
        ReportContactResult(contact);
    });
}

void Scenario1_Single::PickContactPhone()
{
    ClearOutputs();

    // Ask the user to pick a contact phone number.
    auto contactPicker = ref new ContactPicker();
    contactPicker->DesiredFieldsWithContactFieldType->Append(ContactFieldType::PhoneNumber);

    create_task(contactPicker->PickContactAsync()).then([this](Contact^ contact)
    {
        ReportContactResult(contact);
    });
}