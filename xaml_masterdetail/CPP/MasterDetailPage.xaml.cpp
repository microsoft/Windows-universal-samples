//
// MasterDetailPage.xaml.cpp
// Implementation of the MasterDetailPage class.
//

#include "pch.h"
#include "MasterDetailPage.xaml.h"
#include "ItemViewModel.h"


using namespace MasterDetailApp;
using namespace MasterDetailApp::Data;
using namespace MasterDetailApp::ViewModels;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;

MasterDetailPage::MasterDetailPage()
{
	InitializeComponent();
}

void MasterDetailPage::OnNavigatedTo(NavigationEventArgs ^ e)
{
	Page::OnNavigatedTo(e);

	auto items = dynamic_cast<Vector<ItemViewModel^>^>(MasterListView->ItemsSource);

	if (items == nullptr)
	{
		items = ref new Vector<ItemViewModel^>();

		for(auto item : ItemsDataSource::GetAllItems())
		{
			items->Append(ItemViewModel::FromItem(item));
		}

		MasterListView->ItemsSource = items;
	}

	if (e->Parameter != nullptr)
	{
		// Parameter is item ID
		auto id = safe_cast<int>(e->Parameter);

		for (auto item : items)
		{
			if (item->ItemId == id)
			{
				m_lastSelectedItem = item;
			}
		}
	}

	UpdateForVisualState(AdaptiveStates->CurrentState);

	// Don't play a content transition for first item load.
	// Sometimes, this content will be animated as part of the page transition.
	DisableContentTransitions();
}


void MasterDetailPage::UpdateForVisualState(VisualState ^ newState, VisualState ^ oldState)
{
	auto isNarrow = newState == NarrowState;

	if (isNarrow && oldState == DefaultState && m_lastSelectedItem != nullptr)
	{
		// Resize down to the detail item. Don't play a transition.
		Frame->Navigate(TypeName(DetailPage::typeid), m_lastSelectedItem->ItemId, ref new SuppressNavigationTransitionInfo());
	}

	EntranceNavigationTransitionInfo::SetIsTargetElement(MasterListView, isNarrow);
	if (DetailContentPresenter != nullptr)
	{
		EntranceNavigationTransitionInfo::SetIsTargetElement(DetailContentPresenter, !isNarrow);
	}
}

void MasterDetailPage::EnableContentTransitions()
{
	DetailContentPresenter->ContentTransitions->Clear();
	DetailContentPresenter->ContentTransitions->Append(ref new EntranceThemeTransition());
}

void MasterDetailPage::DisableContentTransitions()
{
	if (DetailContentPresenter != nullptr)
	{
		DetailContentPresenter->ContentTransitions->Clear();
	}
}

void MasterDetailPage::LayoutRoot_Loaded(Object^ sender, RoutedEventArgs^ e)
{
	// Assure we are displaying the correct item. This is necessary in certain adaptive cases.
	MasterListView->SelectedItem = m_lastSelectedItem;
}


void MasterDetailPage::AdaptiveStates_CurrentStateChanged(Object^ sender, VisualStateChangedEventArgs^ e)
{
	UpdateForVisualState(e->NewState, e->OldState);
}


void MasterDetailPage::MasterListView_ItemClick(Object^ sender, ItemClickEventArgs^ e)
{
	auto clickedItem = safe_cast<ItemViewModel^>(e->ClickedItem);
	m_lastSelectedItem = clickedItem;

	if (AdaptiveStates->CurrentState == NarrowState)
	{
		// Use "drill in" transition for navigating from master list to detail view
		Frame->Navigate(TypeName(DetailPage::typeid), clickedItem->ItemId, ref new DrillInNavigationTransitionInfo());
	}
	else
	{
		// Play a refresh animation when the user switches detail items.
		EnableContentTransitions();
	}
}
