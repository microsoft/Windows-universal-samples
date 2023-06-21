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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Contacts;
using Windows.Storage.Streams;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Setup : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_Setup()
        {
            InitializeComponent();
        }

        private async void Pin_Click(object sender, RoutedEventArgs e)
        {
            // Get the PinnedContactManager for the current user.
            PinnedContactManager pinnedContactManager = PinnedContactManager.GetDefault();

            // Check whether pinning to the taskbar is supported.
            if (!pinnedContactManager.IsPinSurfaceSupported(PinnedContactSurface.Taskbar))
            {
                // If not, then there is nothing for this program to do.
                rootPage.NotifyUser("The system does not support pinning contacts to the taskbar.", NotifyType.ErrorMessage);
                return;
            }

            // Get the contact list for this app.
            ContactList list = await GetContactListAsync();

            // Check if the sample contact already exists.
            Contact contact = await list.GetContactFromRemoteIdAsync(Constants.ContactRemoteId);

            if (contact == null)
            {
                // Create the sample contact.
                contact = new Contact();
                contact.FirstName = "John";
                contact.LastName = "Doe";
                contact.RemoteId = Constants.ContactRemoteId;
                contact.Emails.Add(new ContactEmail { Address = Constants.ContactEmail });
                contact.Phones.Add(new ContactPhone { Number = Constants.ContactPhone });
                contact.SourceDisplayPicture = RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/LandscapeImage20.jpg"));
                await list.SaveContactAsync(contact);
            }

            // Pin the contact to the taskbar.
            if (!await pinnedContactManager.RequestPinContactAsync(contact, PinnedContactSurface.Taskbar))
            {
                // Contact was not pinned.
                return;
            }

            // It may not be obvious to the user that a contact was pinned if it goes into the overflow.
            // If it looks like we may have gone into the overflow, notify the user that the pin was successful
            // and the contact can still receive notifications.
            if ((await pinnedContactManager.GetPinnedContactIdsAsync()).ContactIds.Count > 3)
            {
                rootPage.NotifyUser("The contact was pinned, but may appear in the overflow. Contacts in the overflow can still receive notifications.", NotifyType.StatusMessage);
            }
        }

        private async void Cleanup_Click(object sender, RoutedEventArgs e)
        {
            // Get the contact list for this app.
            ContactList list = await GetContactListAsync();

            // Look for the contact.
            Contact contact = await list.GetContactFromRemoteIdAsync(Constants.ContactRemoteId);

            if (contact != null)
            {
                // Request unpin
                if (!await PinnedContactManager.GetDefault().RequestUnpinContactAsync(contact, PinnedContactSurface.Taskbar))
                {
                    // User decided to keep the contact on the taskbar.
                    // Don't delete the contact after all.
                    return;
                }
            }

            // The contact is unpinned (or doesn't exist). Clean up the contact list.
            await list.DeleteAsync();
        }

        private async Task<ContactList> GetContactListAsync()
        {
            ContactStore store = await ContactManager.RequestStoreAsync(ContactStoreAccessType.AppContactsReadWrite);

            IReadOnlyList<ContactList> contactLists = await store.FindContactListsAsync();
            ContactList sampleList = contactLists.FirstOrDefault(list => list.DisplayName.Equals(Constants.ContactListName));

            if (sampleList == null)
            {
                sampleList = await store.CreateContactListAsync(Constants.ContactListName);
            }

            return sampleList;
        }
    }
}
