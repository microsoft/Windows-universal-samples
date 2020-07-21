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
#include "MasterDetailPage.h"
#include "MasterDetailPage.g.cpp"
#include "ItemsDataSource.h"
#include <algorithm>

using namespace winrt;
using namespace MasterDetailApp::Data;
using namespace MasterDetailApp::ViewModels;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::MasterDetailApp::implementation
{
    MasterDetailPage::MasterDetailPage()
    {
        InitializeComponent();
        m_items = single_threaded_observable_vector<ItemViewModel>();
        for (auto&& item : ItemsDataSource::GetAllItems())
        {
            m_items.Append(ItemViewModel::FromItem(item));
        }
        MasterListView().ItemsSource(m_items);
    }

    void MasterDetailPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        m_lastSelectedItem = nullptr;
        if (e.Parameter())
        {
            // Parameter is item ID
            auto id = unbox_value<int>(e.Parameter());

            for (auto&& item : m_items)
            {
                if (item.ItemId() == id)
                {
                    m_lastSelectedItem = item;
                    break;
                }
            }
        }

        UpdateForVisualState(AdaptiveStates().CurrentState());

        // Don't play a content transition for first item load.
        // Sometimes, this content will be animated as part of the page transition.
        DisableContentTransitions();
    }

    void MasterDetailPage::AdaptiveStates_CurrentStateChanged(IInspectable const&, VisualStateChangedEventArgs const& e)
    {
        UpdateForVisualState(e.NewState(), e.OldState());
    }

    void MasterDetailPage::UpdateForVisualState(VisualState const& newState, VisualState const& oldState)
    {
        auto isNarrow = newState == NarrowState();

        if (isNarrow && oldState == DefaultState() && m_lastSelectedItem != nullptr)
        {
            // Resize down to the detail item. Don't play a transition.
            Frame().Navigate(xaml_typename<DetailPage>(), box_value(m_lastSelectedItem.ItemId()), SuppressNavigationTransitionInfo());
        }

        EntranceNavigationTransitionInfo::SetIsTargetElement(MasterListView(), isNarrow);
        if (DetailContentPresenter() != nullptr)
        {
            EntranceNavigationTransitionInfo::SetIsTargetElement(DetailContentPresenter(), !isNarrow);
        }
    }

    void MasterDetailPage::EnableContentTransitions()
    {
        DetailContentPresenter().ContentTransitions().ReplaceAll({ EntranceThemeTransition() });
    }

    void MasterDetailPage::DisableContentTransitions()
    {
        if (DetailContentPresenter() != nullptr)
        {
            DetailContentPresenter().ContentTransitions().Clear();
        }
    }

    void MasterDetailPage::LayoutRoot_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        // Assure we are displaying the correct item. This is necessary in certain adaptive cases.
        MasterListView().SelectedItem(m_lastSelectedItem);
    }

    void MasterDetailPage::MasterListView_ItemClick(IInspectable const&, ItemClickEventArgs const& e)
    {
        auto clickedItem = e.ClickedItem().as<ItemViewModel>();
        m_lastSelectedItem = clickedItem;

        if (AdaptiveStates().CurrentState() == NarrowState())
        {
            // Use "drill in" transition for navigating from master list to detail view
            Frame().Navigate(xaml_typename<DetailPage>(), box_value(clickedItem.ItemId()), DrillInNavigationTransitionInfo());
        }
        else
        {
            // Play a refresh animation when the user switches detail items.
            EnableContentTransitions();
        }
    }
}
