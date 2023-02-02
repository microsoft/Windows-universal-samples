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
using System.ComponentModel;
using System.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_MasterDetail : Page
    {
        private ObservableCollection<Contact> Contacts;

        public Scenario3_MasterDetail()
        {
            InitializeComponent();

            // Generate the list of contacts.
            // Remember to enable the NavigationCacheMode of this Page to avoid
            // loading the data every time user navigates back and forward.    
            Contacts = Contact.GetContacts(140);
            if (Contacts.Count > 0)
            {
                MasterListView.ItemsSource = Contacts;
            }
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Details view can request an action on return.
            // S<id> = select this contact
            // D<id> = delete this contact
            var param = e.Parameter as string;
            if (param != null)
            {
                string id = param.Substring(1);
                Contact contact = Contacts.FirstOrDefault(c => c.Id == id);
                if (contact != null)
                {
                    MasterListView.SelectedItem = contact;
                    if (param[0] == 'D')
                    {
                        DeleteItem(null, null);
                    }
                }
            }
        }
        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            if (MasterListView.SelectedItem == null && Contacts.Count > 0)
            {
                MasterListView.SelectedIndex = 0;
            }
            // If the app starts in narrow mode - showing only the Master listView - 
            // it is necessary to set the commands and the selection mode.
            if (PageSizeStatesGroup.CurrentState == NarrowState)
            {
                VisualStateManager.GoToState(this, MasterState.Name, true);
            }
            else if (PageSizeStatesGroup.CurrentState == WideState)
            {
                // In this case, the app starts is wide mode, Master/Details view.
                // Preserve selection across the state change.
                var item = MasterListView.SelectedItem;
                VisualStateManager.GoToState(this, MasterDetailsState.Name, true);
                MasterListView.SelectedItem = item;
            }
            else
            {
                throw new InvalidOperationException();
            }
        }
        private void OnCurrentStateChanged(object sender, VisualStateChangedEventArgs e)
        {
            bool isNarrow = e.NewState == NarrowState;
            if (isNarrow)
            {
                Frame.Navigate(typeof(DetailsPage), MasterListView.SelectedItem, new SuppressNavigationTransitionInfo());
            }
            else
            {
                VisualStateManager.GoToState(this, MasterDetailsState.Name, true);
                MasterListView.SelectionMode = ListViewSelectionMode.Extended;
            }

            EntranceNavigationTransitionInfo.SetIsTargetElement(MasterListView, isNarrow);
            if (DetailContentPresenter != null)
            {
                EntranceNavigationTransitionInfo.SetIsTargetElement(DetailContentPresenter, !isNarrow);
            }
        }
        private void OnSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (PageSizeStatesGroup.CurrentState == WideState)
            {
                if (MasterListView.SelectedItems.Count == 1)
                {
                    EnableContentTransitions();
                }
                // Entering in Extended selection
                else if (MasterListView.SelectedItems.Count > 1
                     && MasterDetailsStatesGroup.CurrentState == MasterDetailsState)
                {
                    VisualStateManager.GoToState(this, ExtendedSelectionState.Name, true);
                }
            }
            // Exiting Extended selection
            if (MasterDetailsStatesGroup.CurrentState == ExtendedSelectionState &&
                MasterListView.SelectedItems.Count == 1)
            {
                VisualStateManager.GoToState(this, MasterDetailsState.Name, true);
            }
        }
        // ItemClick event only happens when user is a Master state. In this state, 
        // selection mode is none and click event navigates to the details view.
        private void OnItemClick(object sender, ItemClickEventArgs e)
        {
            // The clicked item it is the new selected contact            
            var selectedContact = (Contact)e.ClickedItem;
            MasterListView.SelectedItem = selectedContact;
            if (PageSizeStatesGroup.CurrentState == NarrowState)
            {
                // Go to the details page and display the item 
                Frame.Navigate(typeof(DetailsPage), selectedContact, new DrillInNavigationTransitionInfo());
            }
        }
        private void EnableContentTransitions()
        {
            DetailContentPresenter.ContentTransitions.Clear();
            DetailContentPresenter.ContentTransitions.Add(new EntranceThemeTransition());
        }
        #region Commands
        private void AddItem(object sender, RoutedEventArgs e)
        {
            Contacts.Add(Contact.GetNewContact());

            // Select this item if the list had been empty.
            if (MasterListView.SelectedIndex == -1)
            {
                MasterListView.SelectedIndex = 0;
                // Details view might be collapsed if there were no items.
                // Show it now.
                DetailContentPresenter.Visibility = Visibility.Visible;
            }
        }
        private void SelectSomethingIfPossible()
        {
            if (Contacts.Count > 0)
            {
                MasterListView.SelectedIndex = 0;
            }
            else
            {
                // Collapse the Details view if there are no items.
                DetailContentPresenter.Visibility = Visibility.Collapsed;
            }
        }
        private void DeleteItem(object sender, RoutedEventArgs e)
        {
            int index = MasterListView.SelectedIndex;
            if (index != -1)
            {
                Contacts.RemoveAt(index);
            }
            MasterListView.SelectedIndex = Math.Min(index, Contacts.Count - 1);
        }

        private void DeleteItems(object sender, RoutedEventArgs e)
        {
            int index;
            while ((index = MasterListView.SelectedIndex) != -1)
            {
                Contacts.RemoveAt(index);
            }
        }

        private void SelectItems(object sender, RoutedEventArgs e)
        {
            if (MasterListView.Items.Count > 0)
            {
                VisualStateManager.GoToState(this, MultipleSelectionState.Name, true);
            }
        }
        private void CancelSelection(object sender, RoutedEventArgs e)
        {
            if (PageSizeStatesGroup.CurrentState == NarrowState)
            {
                VisualStateManager.GoToState(this, MasterState.Name, true);
            }
            else
            {
                VisualStateManager.GoToState(this, MasterDetailsState.Name, true);
            }
        }
        #endregion
    }
}
