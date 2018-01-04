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
#include "Scenario1_Setup.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Contacts;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
//using namespace Windows::UI::Xaml::Controls;
//using namespace Windows::UI::Xaml::Data;
//using namespace Windows::UI::Xaml::Input;
//using namespace Windows::UI::Xaml::Media;
//using namespace Windows::UI::Xaml::Navigation;

Scenario1_Setup::Scenario1_Setup()
{
    InitializeComponent();
}

void Scenario1_Setup::Pin_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Get the PinnedContactManager for the current user.
    PinnedContactManager^ pinnedContactManager = PinnedContactManager::GetDefault();

    // Check whether pinning to the taskbar is supported.
    if (!pinnedContactManager->IsPinSurfaceSupported(PinnedContactSurface::Taskbar))
    {
        // If not, then there is nothing for this program to do.
        rootPage->NotifyUser("The system does not support pinning contacts to the taskbar.", NotifyType::ErrorMessage);
        return;
    }

    // Get the contact list for this app.
    GetContactListAsync().then([this](ContactList^ list)
    {
        return create_task(list->GetContactFromRemoteIdAsync(Constants::c_ContactRemoteId))
            .then([this, list](Contact^ contact)
        {
            if (contact == nullptr)
            {
                // Create the sample contact.
                contact = ref new Contact();
                contact->FirstName = L"John";
                contact->LastName = L"Doe";
                contact->RemoteId = Constants::c_ContactRemoteId;
                ContactEmail^ email = ref new ContactEmail();
                email->Address = Constants::c_ContactEmail;
                contact->Emails->Append(email);
                ContactPhone^ phone = ref new ContactPhone();
                phone->Number = Constants::c_ContactPhone;
                contact->Phones->Append(phone);
                contact->SourceDisplayPicture = RandomAccessStreamReference::CreateFromUri(ref new Uri(L"ms-appx:///Assets/LandscapeImage20.jpg"));
                return create_task(list->SaveContactAsync(contact)).then([contact]() { return contact; });
            }

            return task_from_result(contact);
        });
    }).then([this, pinnedContactManager](Contact^ contact)
    {
        // Pin the contact to the taskbar.
        return create_task(pinnedContactManager->RequestPinContactAsync(contact, PinnedContactSurface::Taskbar));
    }).then([this, pinnedContactManager](bool pinned)
    {
        if (pinned)
        {
            // It may not be obvious to the user that a contact was pinned if it goes into the overflow.
            // If it looks like we may have gone into the overflow, notify the user that the pin was successful
            // and the contact can still receive notifications.
            create_task(pinnedContactManager->GetPinnedContactIdsAsync()).then([this](PinnedContactIdsQueryResult^ results)
            {
                if (results->ContactIds->Size > 3)
                {
                    rootPage->NotifyUser(L"The contact was pinned, but may appear in the overflow. Contacts in the overflow can still receive notifications.", NotifyType::StatusMessage);
                }
            });
        }
    });
}

void Scenario1_Setup::Cleanup_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Get the contact list for this app.
    GetContactListAsync().then([](ContactList^ list)
    {
        // Look for the contact.
        create_task(list->GetContactFromRemoteIdAsync(Constants::c_ContactRemoteId))
            .then([list](Contact^ contact)
        {
            if (contact != nullptr)
            {
                // Request unpin
                return create_task(PinnedContactManager::GetDefault()->RequestUnpinContactAsync(contact, PinnedContactSurface::Taskbar));
            }

            return task_from_result(true);
        }).then([list](bool unpinned)
        {
            if (unpinned)
            {
                // The contact is unpinned (or doesn't exist). Clean up the contact list.
                list->DeleteAsync();
            }
        });
    });
}

task<ContactList^> Scenario1_Setup::GetContactListAsync()
{
    return create_task(ContactManager::RequestStoreAsync(ContactStoreAccessType::AppContactsReadWrite))
        .then([](ContactStore^ store)
    {
        return create_task(store->FindContactListsAsync())
            .then([store](IVectorView<ContactList^>^ contactLists)
        {
            for (ContactList^ list : contactLists)
            {
                if (list->DisplayName->Equals(Constants::c_ContactListName))
                {
                    return task_from_result(list);
                }
            }

            return create_task(store->CreateContactListAsync(Constants::c_ContactListName));
        });
    });
}