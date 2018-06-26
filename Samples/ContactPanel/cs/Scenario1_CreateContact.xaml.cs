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
using System.Threading.Tasks;
using Windows.ApplicationModel.Contacts;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_CreateContact : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_CreateContact()
        {
            this.InitializeComponent();
        }

        private async void Pin_Click(object sender, RoutedEventArgs e)
        {
            Contact contact = await CreateContactAsync();

            await AnnotateContactAsync(contact);

            await PinContactAsync(contact);
        }

        private async void Cleanup_Click(object sender, RoutedEventArgs e)
        {
            ContactList list = await GetContactListAsync();

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

        private async Task<Contact> CreateContactAsync()
        {
            // Get the contact list for this app.
            ContactList list = await GetContactListAsync();

            // Check if the sample contact already exists.
            Contact contact = await list.GetContactFromRemoteIdAsync(Constants.ContactRemoteId);

            if (contact == null)
            {
                // Create the sample contact.
                contact = new Contact();
                contact.FirstName = Constants.ContactFirstName;
                contact.LastName = Constants.ContactLastName;
                contact.RemoteId = Constants.ContactRemoteId;
                contact.Emails.Add(new ContactEmail { Address = Constants.ContactEmail });
                contact.Phones.Add(new ContactPhone { Number = Constants.ContactPhone });
                contact.SourceDisplayPicture = RandomAccessStreamReference.CreateFromUri(new Uri(Constants.ContactImageSource));

                await list.SaveContactAsync(contact);
            }

            return contact;
        }

        private async Task AnnotateContactAsync(Contact contact)
        {
            // Annotate this contact with a remote ID, which you can then retrieve when the Contact Panel is activated.
            ContactAnnotation contactAnnotation = new ContactAnnotation
            {
                ContactId = contact.Id,
                RemoteId = Constants.ContactRemoteId,
                SupportedOperations = ContactAnnotationOperations.ContactProfile | ContactAnnotationOperations.Message
            };

            // Annotate that this contact can load this app's Contact Panel.
            var infos = await Windows.System.AppDiagnosticInfo.RequestInfoForAppAsync();
            contactAnnotation.ProviderProperties.Add("ContactPanelAppID", infos[0].AppInfo.AppUserModelId);

            var contactAnnotationStore = await ContactManager.RequestAnnotationStoreAsync(ContactAnnotationStoreAccessType.AppAnnotationsReadWrite);
            var contactAnnotationLists = await contactAnnotationStore.FindAnnotationListsAsync();

            ContactAnnotationList contactAnnotationList = contactAnnotationLists.Count > 0 ? contactAnnotationLists[0] : null;

            if (contactAnnotationList == null)
            {
                contactAnnotationList = await contactAnnotationStore.CreateAnnotationListAsync();
            }


            await contactAnnotationList.TrySaveAnnotationAsync(contactAnnotation);
        }

        private async Task PinContactAsync(Contact contact)
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

            // Pin the contact to the taskbar.
            if (!await pinnedContactManager.RequestPinContactAsync(contact, PinnedContactSurface.Taskbar))
            {
                // Contact was not pinned.
                return;
            }
        }

        private async Task<ContactList> GetContactListAsync()
        {
            ContactStore store = await ContactManager.RequestStoreAsync(ContactStoreAccessType.AppContactsReadWrite);

            // Find the contact list by name. (GetContactListAsync looks up a contact list by its ID.)
            IReadOnlyList<ContactList> lists = await store.FindContactListsAsync();
            ContactList sampleList = null;
            foreach (ContactList list in lists)
            {
                if (list.DisplayName == Constants.ContactListName)
                {
                    sampleList = list;
                    break;
                }
            }

            if (sampleList == null)
            {
                sampleList = await store.CreateContactListAsync(Constants.ContactListName);
            }

            return sampleList;
        }
    }
}
