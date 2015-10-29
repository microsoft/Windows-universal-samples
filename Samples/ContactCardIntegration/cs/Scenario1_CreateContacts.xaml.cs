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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.ApplicationModel.Contacts;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_CreateContacts : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_CreateContacts()
        {
            this.InitializeComponent();
        }

        private async Task<ContactList> _GetContactList()
        {
            ContactStore store = await ContactManager.RequestStoreAsync(ContactStoreAccessType.AppContactsReadWrite);
            if (null == store)
            {
                rootPage.NotifyUser("Unable to get a contacts store.", NotifyType.ErrorMessage);
                return null;
            }

            ContactList contactList;
            IReadOnlyList<ContactList> contactLists = await store.FindContactListsAsync();
            if (0 == contactLists.Count)
            {
                contactList = await store.CreateContactListAsync("TestContactList");
            }
            else
            {
                contactList = contactLists[0];
            }

            return contactList;
        }

        private async Task<ContactAnnotationList> _GetContactAnnotationList()
        {
            ContactAnnotationStore annotationStore = await ContactManager.RequestAnnotationStoreAsync(ContactAnnotationStoreAccessType.AppAnnotationsReadWrite);
            if (null == annotationStore)
            {
                rootPage.NotifyUser("Unable to get an annotations store.", NotifyType.ErrorMessage);
                return null;
            }

            ContactAnnotationList annotationList;
            IReadOnlyList<ContactAnnotationList> annotationLists = await annotationStore.FindAnnotationListsAsync();
            if (0 == annotationLists.Count)
            {
                annotationList = await annotationStore.CreateAnnotationListAsync();
            }
            else
            {
                annotationList = annotationLists[0];
            }

            return annotationList;
        }

        private async void CreateTestContacts()
        {
            //
            // Creating two test contacts with email address and phone number.
            //

            Contact contact1 = new Contact();
            contact1.FirstName = "TestContact1";

            ContactEmail email1 = new ContactEmail();
            email1.Address = "TestContact1@contoso.com";
            contact1.Emails.Add(email1);

            ContactPhone phone1 = new ContactPhone();
            phone1.Number = "4255550100";
            contact1.Phones.Add(phone1);

            Contact contact2 = new Contact();
            contact2.FirstName = "TestContact2";

            ContactEmail email2 = new ContactEmail();
            email2.Address = "TestContact2@contoso.com";
            email2.Kind = ContactEmailKind.Other;
            contact2.Emails.Add(email2);

            ContactPhone phone2 = new ContactPhone();
            phone2.Number = "4255550101";
            phone2.Kind = ContactPhoneKind.Mobile;
            contact2.Phones.Add(phone2);

            // Save the contacts
            ContactList contactList = await _GetContactList();
            if (null == contactList)
            {
                return;
            }

            await contactList.SaveContactAsync(contact1);
            await contactList.SaveContactAsync(contact2);

            //
            // Create annotations for those test contacts.
            // Annotation is the contact meta data that allows People App to generate deep links
            // in the contact card that takes the user back into this app.
            //

            ContactAnnotationList annotationList = await _GetContactAnnotationList();
            if (null == annotationList)
            {
                return;
            }

            ContactAnnotation annotation = new ContactAnnotation();
            annotation.ContactId = contact1.Id;

            // Remote ID: The identifier of the user relevant for this app. When this app is
            // launched into from the People App, this id will be provided as context on which user
            // the operation (e.g. ContactProfile) is for.
            annotation.RemoteId = "user12";

            // The supported operations flags indicate that this app can fulfill these operations
            // for this contact. These flags are read by apps such as the People App to create deep
            // links back into this app. This app must also be registered for the relevant
            // protocols in the Package.appxmanifest (in this case, ms-contact-profile).
            annotation.SupportedOperations = ContactAnnotationOperations.ContactProfile;

            if (!await annotationList.TrySaveAnnotationAsync(annotation))
            {
                rootPage.NotifyUser("Failed to save annotation for TestContact1 to the store.", NotifyType.ErrorMessage);
                return;
            }

            annotation = new ContactAnnotation();
            annotation.ContactId = contact2.Id;
            annotation.RemoteId = "user22";

            // You can also specify multiple supported operations for a contact in a single
            // annotation. In this case, this annotation indicates that the user can be
            // communicated via VOIP call, Video Call, or IM via this application.
            annotation.SupportedOperations = ContactAnnotationOperations.Message |
                ContactAnnotationOperations.AudioCall |
                ContactAnnotationOperations.VideoCall;

            if (!await annotationList.TrySaveAnnotationAsync(annotation))
            {
                rootPage.NotifyUser("Failed to save annotation for TestContact2 to the store.", NotifyType.ErrorMessage);
                return;
            }

            rootPage.NotifyUser("Sample data created successfully.", NotifyType.StatusMessage);
        }

        private async void DeleteTestContacts()
        {
            ContactList contactList = null;
            ContactStore store = await ContactManager.RequestStoreAsync(ContactStoreAccessType.AppContactsReadWrite);
            if (null != store)
            {
                IReadOnlyList<ContactList> contactLists = await store.FindContactListsAsync();
                if (0 < contactLists.Count)
                {
                    contactList = contactLists[0];
                }
            }

            if (null != contactList)
            {
                await contactList.DeleteAsync();
                rootPage.NotifyUser("Sample data deleted.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Could not delete sample data.", NotifyType.ErrorMessage);
            }
        }
    }
}
