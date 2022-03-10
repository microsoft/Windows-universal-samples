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
#include "Scenario3_MasterDetail.h"
#include "Scenario3_MasterDetail.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media::Animation;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_MasterDetail::Scenario3_MasterDetail()
    {
        InitializeComponent();

        // Generate the list of contacts.
        // Remember to enable the NavigationCacheMode of this Page to avoid
        // loading the data every time user navigates back and forward.    
        Contacts = Contact::GetContacts(140);
        if (Contacts.Size() > 0)
        {
            MasterListView().ItemsSource(Contacts);
        }
    }
    void Scenario3_MasterDetail::OnNavigatedTo(NavigationEventArgs const& e)
    {
        // Details view can request an action on return.
        // S<id> = select this contact
        // D<id> = delete this contact
        hstring param = unbox_value_or(e.Parameter(), L"");
        if (!param.empty())
        {
            std::wstring_view id = std::wstring_view(param).substr(1);
            auto it = std::find_if(begin(Contacts), end(Contacts), [&](auto&& c) { return c.Id() == id; });
            if (it != end(Contacts))
            {
                MasterListView().SelectedItem(*it);
                if (param.front() == L'D')
                {
                    DeleteItem(nullptr, nullptr);
                }
            }
        }
    }

    void Scenario3_MasterDetail::OnLoaded(IInspectable const&, RoutedEventArgs const&)
    {
        if (MasterListView().SelectedItem()== nullptr && Contacts.Size() > 0)
        {
            MasterListView().SelectedIndex(0);
        }

        // If the app starts in narrow mode - showing only the Master listView - 
        // it is necessary to set the commands and the selection mode.
        if (PageSizeStatesGroup().CurrentState() == NarrowState())
        {
            VisualStateManager().GoToState(*this, MasterState().Name(), true);
        }
        else if (PageSizeStatesGroup().CurrentState() == WideState())
        {
            // In this case, the app starts is wide mode, Master/Details view.
            // Preseve the selection across the state change.
            IInspectable item = MasterListView().SelectedItem();
            VisualStateManager().GoToState(*this, MasterDetailsState().Name(), true);
            MasterListView().SelectedItem(item);
        }
        else
        {
            throw hresult_not_implemented();
        }
    }

    void Scenario3_MasterDetail::OnCurrentStateChanged(IInspectable const&, VisualStateChangedEventArgs const& e)
    {
        bool isNarrow = e.NewState() == NarrowState();
        if (isNarrow)
        {
            Frame().Navigate(xaml_typename<SDKTemplate::DetailsPage>(), MasterListView().SelectedItem(), SuppressNavigationTransitionInfo());
        }
        else
        {
            VisualStateManager().GoToState(*this, MasterDetailsState().Name(), true);
            MasterListView().SelectionMode(ListViewSelectionMode::Extended);
        }

        EntranceNavigationTransitionInfo().SetIsTargetElement(MasterListView(), isNarrow);
        if (DetailContentPresenter() != nullptr)
        {
            EntranceNavigationTransitionInfo().SetIsTargetElement(DetailContentPresenter(), !isNarrow);
        }
    }

    void Scenario3_MasterDetail::OnSelectionChanged(IInspectable const&, SelectionChangedEventArgs const&)
    {
        if (PageSizeStatesGroup().CurrentState() == WideState())
        {
            if (MasterListView().SelectedItems().Size() == 1)
            {
                EnableContentTransitions();
            }
            // Entering in Extended selection
            else if (MasterListView().SelectedItems().Size() > 1
                && MasterDetailsStatesGroup().CurrentState() == MasterDetailsState())
            {
                VisualStateManager().GoToState(*this, ExtendedSelectionState().Name(), true);
            }
        }
        // Exiting Extended selection
        if (MasterDetailsStatesGroup().CurrentState() == ExtendedSelectionState() &&
            MasterListView().SelectedItems().Size() == 1)
        {
            VisualStateManager().GoToState(*this, MasterDetailsState().Name(), true);
        }
    }

    // ItemClick event only happens when user is a Master state. In this state, 
    // selection mode is none and click event navigates to the details view.
    void Scenario3_MasterDetail::OnItemClick(IInspectable const&, ItemClickEventArgs const& e)
    {
        // The clicked item it is the new selected contact
        SDKTemplate::Contact selectedContact = e.ClickedItem().as<SDKTemplate::Contact>();
        MasterListView().SelectedItem(selectedContact);
        if (PageSizeStatesGroup().CurrentState() == NarrowState())
        {
            // Go to the details page and display the item 
            Frame().Navigate(xaml_typename<SDKTemplate::DetailsPage>(), selectedContact, DrillInNavigationTransitionInfo());
        }
    }

    void Scenario3_MasterDetail::EnableContentTransitions()
    {
        DetailContentPresenter().ContentTransitions().ReplaceAll({ EntranceThemeTransition() });
    }

#pragma region Commands
    void Scenario3_MasterDetail::AddItem(IInspectable const&, RoutedEventArgs const&)
    {
        SDKTemplate::Contact c = Contact::GetNewContact();
        Contacts.Append(c);

        // Select this item in case that the list is empty
        if (MasterListView().SelectedIndex() == -1)
        {
            MasterListView().SelectedIndex(0);
            // Details view might be collapsed if there were no items.
            // Show it now.
            DetailContentPresenter().Visibility(Visibility::Visible);
        }
    }

    void Scenario3_MasterDetail::SelectSomethingIfPossible()
    {
        if (Contacts.Size() > 0)
        {
            MasterListView().SelectedIndex(0);
        }
        else
        {
            // Collapse the Details view if there are no items.
            DetailContentPresenter().Visibility(Visibility::Collapsed);
        }
    }

    void Scenario3_MasterDetail::DeleteItem(IInspectable const&, RoutedEventArgs const&)
    {
        int32_t index = MasterListView().SelectedIndex();

        if (index != -1)
        {
                Contacts.RemoveAt(index);
                SelectSomethingIfPossible();
        }
    }

    void Scenario3_MasterDetail::DeleteItems(IInspectable const&, RoutedEventArgs const&)
    {
        if (MasterListView().SelectedIndex() != -1)
        {
            while (MasterListView().SelectedIndex() != -1)
            {
                Contacts.RemoveAt(MasterListView().SelectedIndex());
            }
            SelectSomethingIfPossible();
        }
    }

    void Scenario3_MasterDetail::SelectItems(IInspectable const&, RoutedEventArgs const&)
    {
        if (MasterListView().Items().Size() > 0)
        {
            VisualStateManager().GoToState(*this, MultipleSelectionState().Name(), true);
        }
    }
    void Scenario3_MasterDetail::CancelSelection(IInspectable const&, RoutedEventArgs const&)
    {
        if (PageSizeStatesGroup().CurrentState() == NarrowState())
        {
            VisualStateManager().GoToState(*this, MasterState().Name(), true);
        }
        else
        {
            VisualStateManager().GoToState(*this, MasterDetailsState().Name(), true);
        }
    }
#pragma endregion
}
