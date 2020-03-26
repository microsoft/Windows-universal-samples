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
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.ApplicationModel.Contacts;
using Windows.ApplicationModel.Core;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
using PhoneCall.Helpers;

namespace PhoneCall.ViewModels
{
    class ContactsViewModel : ViewModelBase
    {
        private string contactsStackPanelVisibleState = "Visible";
        private string contactsStatusVisibleState = "Visible"; 
        private string contactsStatusText = "Loading Contacts...";
        private string contactsSearchVisibleState = "collapsed";
        private string contactsListVisibleState = "Collapsed";
        private string contactsListGridHeight = "Auto";
        private string contactsGroupVisibleState = "Collapsed";
        private string contactsGroupGridHeight = "Auto";
        private ObservableCollection<ContactItem> contactItems = new ObservableCollection<ContactItem>();
        private ObservableCollection<ContactItem> listOfContacts = new ObservableCollection<ContactItem>();
        private List<AlphaKeyGroup<ContactItem>> groupsOfContacts;
        private ContactStore store;
        private SolidColorBrush displayAccentBrush;

        /// <summary>
        /// A view model class for the contacts panel/module.
        /// </summary>
        public ContactsViewModel()
        {
            UpdateContactListAsync();
            CallingInfo.CallingInfoInstance.CellInfoUpdateCompleted += CallingInfoInstance_CellInfoUpdateCompletedAsync;
        }

        /// <summary>
        /// Ensures the UI is updated upon initilization of the cellular details singleton.
        /// Even if the contacts module is loaded before the singleton is done intializaing
        /// This is fired from the calling info singleton
        /// </summary>
        private async void CallingInfoInstance_CellInfoUpdateCompletedAsync()
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                () =>
                {
                    UpdateContactListAsync();
                });
        }

        /// <summary>
        /// Updates the contacts view with information from the calling info singleton.
        /// The only item we care about here is the accent color.
        /// </summary>
        public async void UpdateContactListAsync()
        {
            //Color Values to be used during update
            Color NoColor = Color.FromArgb(0, 0, 0, 0);
            Color DisplayAccentColor = (Color)Application.Current.Resources["SystemAccentColor"];

            //Set Accent highlight based on display
            Color DisplayColorObject = CallingInfo.CallingInfoInstance.CurrentDisplayColor;
            if (DisplayColorObject == NoColor)
            {
                DisplayColorObject = DisplayAccentColor;
            }
            DisplayAccentBrush = new SolidColorBrush(DisplayColorObject);
            await LoadContactsFromStoreAsync();
        }

        /// <summary>
        /// Loads up the contact store using the contactsRT APIs.
        /// Checks to make sure the store can be loaded and errors out accordingly
        /// </summary>
        private async Task LoadContactsFromStoreAsync()
        {
            //Try loading the contact atore
            try
            {
                store = await ContactManager.RequestStoreAsync(ContactStoreAccessType.AllContactsReadOnly);
            }
            catch (Exception ex)
            {
                ContactsStatusText = "We couldn't load the contact store.";
                Debug.WriteLine("Potential contact store bug: " + ex, "error");
            }

            //If we can access the store without crashing (There seems to be a bug with the store).
            //Check to make sure we actually have access.
            if (store == null)
            {
                //Launch the settings app to fix the security settings
                Debug.WriteLine("Could not open contact store, is app access disabled in privacy settings?", "error");
                return;
            }
            Debug.WriteLine("Contact store opened for reading successfully.", "informational");
            //Load the contacts into the ListView on the page
            ContactReader reader = store.GetContactReader();
            await DisplayContactsFromReaderAsync(reader, true);
            return;
        }

        /// <summary>
        /// Processes contact search.
        /// </summary>
        /// <param name="ContactFilter">Takes in the string inputed by the user</param>
        private async Task SearchForTextAsync(string ContactFilter)
        {
            if (store == null)
            {
                //Shouldn't happen, and I don't want to deal with opening the store in multiple locations
                await LoadContactsFromStoreAsync();
                return;
            }
            //A null query string is being treated as a query for "*"
            if (!string.IsNullOrWhiteSpace(ContactFilter))
            {
                ContactQueryOptions option = new ContactQueryOptions(ContactFilter, ContactQuerySearchFields.All);
                ContactReader reader = store.GetContactReader(option);
                await DisplayContactsFromReaderAsync(reader, false);
            }
            else
            {
                ContactReader reader = store.GetContactReader();
                await DisplayContactsFromReaderAsync(reader, true);
            }
            return;
        }

        /// <summary>
        /// Displays the items from the contact store in the contacts view.
        /// </summary>
        /// <param name="reader">Contact store reader</param>
        /// <param name="isGroup">
        /// Boolean to decide if to show the list in a group or a flat list
        /// Groups is shown by default and search
        /// Flast list is shown during search
        /// </param>
        private async Task DisplayContactsFromReaderAsync(ContactReader reader, bool isGroup)
        {
            contactItems.Clear();
            ContactBatch contactBatch = await reader.ReadBatchAsync();
            if (contactBatch.Contacts.Count == 0)
            {
                Debug.WriteLine("Contact store empty");
                ContactsStatusText = "We couldn't find any contacts.";
                ShowContactListStatusText();
                return;
            }

            while (contactBatch.Contacts.Count != 0)
            {
                //should batch add to avoid triggering callbacks            
                foreach (Contact c in contactBatch.Contacts)
                {
                    ContactItem contactToAdd = new ContactItem(c.Id, c.DisplayName);
                    contactToAdd.SetImageAsync(c.Thumbnail);
                    contactItems.Add(contactToAdd);
                }
                contactBatch = await reader.ReadBatchAsync();
            }

            if (isGroup)
            {
                GroupsOfContacts = alphaGroupSorting(contactItems);
                ShowContactGroup();
            }
            else
            {
                ListOfContacts = contactItems;
                ShowContactList();
            }
            return;
        }

        /// <summary>
        /// Creating an aplabetical sorted group of the contacts in the store.
        /// </summary>
        /// <param name="items">Flat list of contacts</param>
        private List<AlphaKeyGroup<ContactItem>> alphaGroupSorting(IEnumerable<ContactItem> items)
        {
            ///////////////////////////////////////////////////
            //   To create the list grouped by letter, we add
            //   1) the list of items we're going to group
            //   2) how we are going to group them (in this case, by Title
            //   3) if we want to sort them (true)
            //
            //   This creates an alphabetized list of lists in which the items 
            //   in each list all starts with the same letter. This type is 
            //   recognized by our GridView and ListView controls and can 
            //   be grouped appropriately as well as being responsive to a
            //   SemanticZoom control for the sake of a jumplist interaction
            ///////////////////////////////////////////////////

            var returnGroup = AlphaKeyGroup<ContactItem>.CreateGroups(
                items,                                      // ungrouped list of items
                (ContactItem s) => { return s.ContactName; },  // the property to sort 
                true);                                      // order the items alphabetically 

            return returnGroup;
        }

        /// <summary>
        /// Shows a flat list of contacts.
        /// Hides irrelevant elements/controls
        /// </summary>
        private void ShowContactList()
        {
            ContactsStatusVisibleState = "Collapsed";
            ContactsGroupVisibleState = "Collapsed";
            ContactsGroupGridHeight = "Auto";
            ContactsSearchVisibleState = "Visible";
            ContactsListGridHeight = "*";
            ContactsListVisibleState = "Visible";
        }

        /// <summary>
        /// Shows a grouped list of contacts.
        /// Hides irrelevant elements/controls
        /// </summary>
        private void ShowContactGroup()
        {
            ContactsStatusVisibleState = "Collapsed";
            ContactsListVisibleState = "Collapsed";
            ContactsListGridHeight = "Auto";
            ContactsSearchVisibleState = "Visible";
            ContactsGroupGridHeight = "*";
            ContactsGroupVisibleState = "Visible";
        }

        /// <summary>
        /// Shows the status text in case of errors and additional info.
        /// Hides all contact list
        /// </summary>
        private void ShowContactListStatusText()
        {
            ContactsSearchVisibleState = "Visible";
            ContactsListVisibleState = "Collapsed";
            ContactsListGridHeight = "Auto";
            ContactsGroupVisibleState = "Collapsed";
            ContactsGroupGridHeight = "Auto";
            ContactsStatusVisibleState = "Visible";
        }

        /// <summary>
        /// Hides the status and search elements
        /// Used in the group zoomed out mode
        /// </summary>
        private void HideStatusAndSearch()
        {
            ContactsStackPanelVisibleState = "Collapsed";
        }

        /// <summary>
        /// Shows the status and search elements
        /// Used in the group zoomed in mode
        /// </summary>
        private void ShowStatusAndSearch()
        {
            ContactsStackPanelVisibleState = "Visible";
        }


        /// <summary>
        /// Processes the contact search event
        /// </summary>
        //Update to this when event bindings are working
        //public void ProcessContactSearch(object sender, TextChangedEventArgs e)
        public async void ProcessContactSearchAsync(string searchQuery)
        {
            await SearchForTextAsync(searchQuery);
        }

        /// <summary>
        /// Processes the open contact event
        /// </summary>
        public async void ProcessOpenContactAsync(ContactItem contact)
        {
            if (store == null)
            {
                //Shouldn't happen, and I don't want to deal with opening the store in multiple locations
                await LoadContactsFromStoreAsync();
                return;
            }
            Contact c = await store.GetContactAsync(contact.ContactId);
            ContactManager.ShowFullContactCard(c, new FullContactCardOptions());
        }


        /// <summary>
        /// Shows/Hides different elements to maximize semantic zoom realestate
        /// </summary>
        public void MaximizeSemanticZoomOut(bool isZoomedInActive)
        {
            if (isZoomedInActive)
            {
                //Restore view
                ShowStatusAndSearch();
            }
            else
            {
                //Maximise view
                HideStatusAndSearch();
            }
        }

        #region Setters and Getters
        /// <summary>
        /// Gets and sets contacts stack panel visitble state
        /// </summary>
        public string ContactsStackPanelVisibleState
        {
            get
            {
                return contactsStackPanelVisibleState;
            }
            private set
            {
                if (contactsStackPanelVisibleState != value)
                {
                    contactsStackPanelVisibleState = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the contacts status text block visibile state.
        /// </summary>
        public string ContactsStatusVisibleState
        {
            get
            {
                return contactsStatusVisibleState;
            }
            private set
            {
                if (contactsStatusVisibleState != value)
                {
                    contactsStatusVisibleState = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the contact status text.
        /// </summary>
        public string ContactsStatusText
        {
            get
            {
                return contactsStatusText;
            }
            private set
            {
                if (contactsStatusText != value)
                {
                    contactsStatusText = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the contact search text box visibile state.
        /// </summary>
        public string ContactsSearchVisibleState
        {
            get
            {
                return contactsSearchVisibleState;
            }
            private set
            {
                if (contactsSearchVisibleState != value)
                {
                    contactsSearchVisibleState = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the contact list visible state.
        /// </summary>
        public string ContactsListVisibleState
        {
            get
            {
                return contactsListVisibleState;
            }
            private set
            {
                if (contactsListVisibleState != value)
                {
                    contactsListVisibleState = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the contacts list grid height.
        /// </summary>
        public string ContactsListGridHeight
        {
            get
            {
                return contactsListGridHeight;
            }
            private set
            {
                if (contactsListGridHeight != value)
                {
                    contactsListGridHeight = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the contacts group list visibile state.
        /// </summary>
        public string ContactsGroupVisibleState
        {
            get
            {
                return contactsGroupVisibleState;
            }
            private set
            {
                if (contactsGroupVisibleState != value)
                {
                    contactsGroupVisibleState = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the contact group list grid height.
        /// </summary>
        public string ContactsGroupGridHeight
        {
            get
            {
                return contactsGroupGridHeight;
            }
            private set
            {
                if (contactsGroupGridHeight != value)
                {
                    contactsGroupGridHeight = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the flat contacts list.
        /// </summary>
        public ObservableCollection<ContactItem> ListOfContacts
        {
            get
            {
                return listOfContacts;
            }
            private set
            {
                if (listOfContacts != value)
                {
                    listOfContacts = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the grouped contacts list.
        /// </summary>
        public List<AlphaKeyGroup<ContactItem>> GroupsOfContacts
        {
            get
            {
                return groupsOfContacts;
            }
            private set
            {
                if (groupsOfContacts != value)
                {
                    groupsOfContacts = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the display color brush of the current line.
        /// </summary>
        public SolidColorBrush DisplayAccentBrush
        {
            get
            {
                return displayAccentBrush;
            }
            private set
            {
                if (displayAccentBrush != value)
                {
                    displayAccentBrush = value;
                    RaisePropertyChanged();
                }
            }
        }
        #endregion
    }
}