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
#include "Contact.h"
#include "Scenario4_EdgeTappedListView.h"
#include "Scenario4_EdgeTappedListView.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_EdgeTappedListView::Scenario4_EdgeTappedListView()
    {
        InitializeComponent();

        Contacts = Contact::GetContacts(50);
        MyListView().ItemsSource(Contacts);

        // We set the state of the commands on the appbar
        SetCommandsVisibility(MyListView());
    }

    void Scenario4_EdgeTappedListView::OnNavigatedTo(NavigationEventArgs const&)
    {
        // This is how devs can handle the back button
        backRequestedToken = SystemNavigationManager::GetForCurrentView().BackRequested({ get_weak(), &Scenario4_EdgeTappedListView::OnBackRequested });
    }

    void Scenario4_EdgeTappedListView::OnNavigatedFrom(NavigationEventArgs const&)
    {
        SystemNavigationManager::GetForCurrentView().BackRequested(backRequestedToken);
    }

    void Scenario4_EdgeTappedListView::OnEdgeTapped(SDKTemplate::EdgeTappedListView const&, SDKTemplate::ListViewEdgeTappedEventArgs const&)
    {
        // When user releases the pointer after pressing on the left edge of the item,
        // the ListView will switch to Multiple Selection
        MyListView().SelectionMode(ListViewSelectionMode::Multiple);
        // Also, we want the Left Edge Tap funcionality will be no longer enable.
        MyListView().IsItemLeftEdgeTapEnabled(false);
        // It's desirable that the Appbar shows the actions available for multiselect
        SetCommandsVisibility(MyListView());
    }

    void Scenario4_EdgeTappedListView::UpdateSelectionUI()
    {
        // When there are no selected items, the list returns to None selection mode.
        if (MyListView().SelectedItems().Size() == 0)
        {
            MyListView().SelectionMode(ListViewSelectionMode::None);
            MyListView().IsItemLeftEdgeTapEnabled(true);
            SetCommandsVisibility(MyListView());
        }
    }

    void Scenario4_EdgeTappedListView::OnSelectionChanged(IInspectable const&, SelectionChangedEventArgs const&)
    {
        UpdateSelectionUI();
    }

    void Scenario4_EdgeTappedListView::OnBackRequested(IInspectable const&, BackRequestedEventArgs const& e)
    {
        // We want to exit from the multiselect mode when pressing back button
        if (!e.Handled() && MyListView().SelectionMode() == ListViewSelectionMode::Multiple)
        {
            MyListView().SelectedItems().Clear();
            UpdateSelectionUI();
            e.Handled(true);
        }
    }

    void Scenario4_EdgeTappedListView::SetCommandsVisibility(ListView const& listView)
    {
        if (listView.SelectionMode() == ListViewSelectionMode::Multiple || listView.SelectedItems().Size() > 1)
        {
            SelectAppBarBtn().Visibility(Visibility::Collapsed);
            CancelSelectionAppBarBtn().Visibility(Visibility::Visible);
            AddItemAppBarBtn().Visibility(Visibility::Collapsed);
            RemoveItemAppBarBtn().Visibility(Visibility::Visible);
            SystemNavigationManager::GetForCurrentView().AppViewBackButtonVisibility(AppViewBackButtonVisibility::Visible);
        }
        else
        {
            SelectAppBarBtn().Visibility(Visibility::Visible);
            CancelSelectionAppBarBtn().Visibility(Visibility::Collapsed);
            AddItemAppBarBtn().Visibility(Visibility::Visible);
            RemoveItemAppBarBtn().Visibility(Visibility::Collapsed);
            SystemNavigationManager::GetForCurrentView().AppViewBackButtonVisibility(AppViewBackButtonVisibility::Collapsed);
        }
    }

    void Scenario4_EdgeTappedListView::SelectItems(IInspectable const&, RoutedEventArgs const&)
    {
        MyListView().SelectionMode(ListViewSelectionMode::Multiple);
        MyListView().IsItemLeftEdgeTapEnabled(false);
        SetCommandsVisibility(MyListView());
    }

    void Scenario4_EdgeTappedListView::AddItem(IInspectable const&, RoutedEventArgs const&)
    {
        Contacts.Append(Contact::GetNewContact());
    }

    void Scenario4_EdgeTappedListView::RemoveItem(IInspectable const&, RoutedEventArgs const&)
    {
        int32_t index;
        while ((index = MyListView().SelectedIndex()) != -1)
        {
            Contacts.RemoveAt(index);
        }
    }

    void Scenario4_EdgeTappedListView::CancelSelection(IInspectable const&, RoutedEventArgs const&)
    {
        // If the list is multiple selection mode but there is no items selected, 
        // then the list should return to the initial selection mode.
        if (MyListView().SelectedItems().Size() == 0)
        {
            MyListView().SelectionMode(ListViewSelectionMode::None);
            MyListView().IsItemLeftEdgeTapEnabled(true);
            SetCommandsVisibility(MyListView());
        }
        else
        {
            MyListView().SelectedItems().Clear();
        }
    }

}
