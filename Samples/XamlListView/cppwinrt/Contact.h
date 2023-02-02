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

#pragma once

#include "Contact.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Contact : ContactT<Contact>
    {
        hstring Id() { return id; }
        hstring Initials() { return InitialOf(firstName) + InitialOf(lastName); }
        hstring Name() { return firstName + L" " + lastName; }
        hstring LastName() { return lastName; }
        hstring FirstName() { return firstName; }
        hstring Position() { return position; }
        hstring PhoneNumber() { return phoneNumber; }
        hstring Biography() { return biography; }

        static SDKTemplate::Contact GetNewContact() { return make<Contact>(); }
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::Contact> GetContacts(int count);
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::GroupInfoList> GetContactsGrouped(int count);

    private:
        hstring id = to_hstring(Windows::Foundation::GuidHelper::CreateNewGuid());
        hstring lastName = GenerateLastName();
        hstring firstName = GenerateFirstName();
        hstring position = GeneratePosition();
        hstring phoneNumber = GeneratePhoneNumber();
        hstring biography = GenerateBiography();

        static hstring InitialOf(hstring const& word)
        {
            return hstring(static_cast<std::wstring_view>(word).substr(0, 1));
        }

        static hstring GenerateLastName();
        static hstring GenerateFirstName();
        static hstring GeneratePosition();
        static hstring GeneratePhoneNumber();
        static hstring GenerateBiography();
    };
}
