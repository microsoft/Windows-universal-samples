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
using namespace Windows::ApplicationModel::Contacts;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Show mini contact card", "SDKTemplate.Scenario1_Mini" },
    { "Show mini contact card with delayed information", "SDKTemplate.Scenario2_DelayMini" },
    { "Show full contact card", "SDKTemplate.Scenario3_Full" },

};

Contact^ MainPage::CreateContactFromUserInput(TextBox^ EmailAddress, TextBox^ PhoneNumber)
{
    if (EmailAddress->Text->Length() == 0 && PhoneNumber->Text->Length() == 0)
    {
        NotifyUser("You must enter an email address and/or phone number.", NotifyType::ErrorMessage);
        return nullptr;
    }

    Contact^ contact = ref new Contact();

    // Maximum length for email address is 321, enforced by XAML markup.
    if (EmailAddress->Text->Length() > 0)
    {
        ContactEmail^ email = ref new ContactEmail();
        email->Address = EmailAddress->Text;
        contact->Emails->Append(email);
    }

    // Maximum length for phone number is 50, enforced by XAML markup.
    if (PhoneNumber->Text->Length() > 0)
    {
        ContactPhone^ phone = ref new ContactPhone();
        phone->Number = PhoneNumber->Text;
        contact->Phones->Append(phone);
    }

    return contact;
}

Rect MainPage::GetElementRect(FrameworkElement^ element)
{
    Windows::UI::Xaml::Media::GeneralTransform^ buttonTransform = element->TransformToVisual(nullptr);
    Point point = buttonTransform->TransformPoint(Point());
    return Rect(point, Size(static_cast<float>(element->ActualWidth), static_cast<float>(element->ActualHeight)));
}
