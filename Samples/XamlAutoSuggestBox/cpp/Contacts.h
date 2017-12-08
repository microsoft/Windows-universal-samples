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

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Contact sealed
    {
    public:
        Contact(Platform::String^ firstName, Platform::String^ lastName, Platform::String^ company) :
            firstName(firstName), lastName(lastName), company(company)
        {
        }

        property Platform::String^ FirstName { Platform::String^ get() { return firstName; } }
        property Platform::String^ LastName { Platform::String^ get() { return lastName; } }
        property Platform::String^ Company { Platform::String^ get() { return company; } }

        property Platform::String^ FullName
        {
            Platform::String^ get() { return FirstName + " " + LastName; }
        }

        property Platform::String^ DisplayName
        {
            Platform::String^ get() { return FullName + " (" + Company + ")"; }
        }

    private:
        Platform::String^ firstName;
        Platform::String^ lastName;
        Platform::String^ company;
    };

    class ContactSampleDataSource
    {
    public:
        static Concurrency::task<void> ContactSampleDataSource::CreateContactSampleDataAsync();
        static std::vector<Contact^> GetMatchingContacts(Platform::String^ query);

    private:
        static std::vector<Contact^> contacts;
    };
}
