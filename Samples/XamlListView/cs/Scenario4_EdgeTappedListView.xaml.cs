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

using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario4_EdgeTappedListView : Page
    {
        private ObservableCollection<Contact> Contacts;

        public Scenario4_EdgeTappedListView()
        {
            InitializeComponent();

            Contacts = Contact.GetContacts(50);
            MyListView.ItemsSource = Contacts;

            // We set the state of the commands on the appbar
            SetCommandsVisibility(MyListView);
        }
        protected override void OnNavigatedTo(Windows.UI.Xaml.Navigation.NavigationEventArgs e)
        {
            // This is how devs can handle the back button
            SystemNavigationManager.GetForCurrentView().BackRequested += OnBackRequested;
        }
        protected override void OnNavigatedFrom(Windows.UI.Xaml.Navigation.NavigationEventArgs e)
        {
            SystemNavigationManager.GetForCurrentView().BackRequested -= OnBackRequested;
        }
        private void OnEdgeTapped(ListView sender, ListViewEdgeTappedEventArgs e)
        {
            // When user releases the pointer after pressing on the left edge of the item,
            // the ListView will switch to Multiple Selection
            MyListView.SelectionMode = ListViewSelectionMode.Multiple;
            // Also, we want the Left Edge Tap funcionality will be no longer enable. 
            MyListView.IsItemLeftEdgeTapEnabled = false;
            // It's desirable that the Appbar shows the actions available for multiselect
            SetCommandsVisibility(MyListView);
        }
        private void UpdateSelectionUI()
        {
            // When there are no selected items, the list returns to None selection mode.
            if (MyListView.SelectedItems.Count == 0)
            {
                MyListView.SelectionMode = ListViewSelectionMode.None;
                MyListView.IsItemLeftEdgeTapEnabled = true;
                SetCommandsVisibility(MyListView);
            }
        }
        private void OnSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdateSelectionUI();
        }
        private void OnBackRequested(object sender, BackRequestedEventArgs e)
        {
            // We want to exit from the multiselect mode when pressing back button
            if (!e.Handled && MyListView.SelectionMode == ListViewSelectionMode.Multiple)
            {
                MyListView.SelectedItems.Clear();
                UpdateSelectionUI();
                e.Handled = true;
            }
        }
        private void SetCommandsVisibility(ListView listView)
        {
            if (listView.SelectionMode == ListViewSelectionMode.Multiple || listView.SelectedItems.Count > 1)
            {
                SelectAppBarBtn.Visibility = Visibility.Collapsed;
                CancelSelectionAppBarBtn.Visibility = Visibility.Visible;
                AddItemAppBarBtn.Visibility = Visibility.Collapsed;
                RemoveItemAppBarBtn.Visibility = Visibility.Visible;
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility = AppViewBackButtonVisibility.Visible;
            }
            else
            {
                SelectAppBarBtn.Visibility = Visibility.Visible;
                CancelSelectionAppBarBtn.Visibility = Visibility.Collapsed;
                AddItemAppBarBtn.Visibility = Visibility.Visible;
                RemoveItemAppBarBtn.Visibility = Visibility.Collapsed;
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility = AppViewBackButtonVisibility.Collapsed;
            }
        }
        private void SelectItems(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            MyListView.SelectionMode = ListViewSelectionMode.Multiple;
            MyListView.IsItemLeftEdgeTapEnabled = false;
            SetCommandsVisibility(MyListView);
        }
        private void AddItem(object sender, RoutedEventArgs e)
        {
            Contacts.Add(Contact.GetNewContact());
        }
        private void RemoveItem(object sender, RoutedEventArgs e)
        {
            if (MyListView.SelectedIndex != -1)
            {
                // When an item is removed from the underlying collection, the Listview is updated, 
                // hence the this.SelectedItems is updated as well. 
                // It's needed to copy the selected items collection to iterate over other collection that 
                // is not updated.
                List<Contact> selectedItems = new List<Contact>();
                foreach (Contact item in MyListView.SelectedItems)
                {
                    selectedItems.Add(item);
                }
                foreach (Contact item in selectedItems)
                {
                    Contacts.Remove(item);
                }
            }
        }
        private void CancelSelection(object sender, RoutedEventArgs e)
        {
            // If the list is multiple selection mode but there is no items selected, 
            // then the list should return to the initial selection mode.
            if (MyListView.SelectedItems.Count == 0)
            {
                MyListView.SelectionMode = ListViewSelectionMode.None;
                MyListView.IsItemLeftEdgeTapEnabled = true;
                SetCommandsVisibility(MyListView);
            }
            else
            {
                MyListView.SelectedItems.Clear();
            }
        }
    }
}
