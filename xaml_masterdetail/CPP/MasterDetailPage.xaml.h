//
// MasterDetailPage.xaml.h
// Declaration of the MasterDetailPage class.
//

#pragma once

#include "MasterDetailPage.g.h"

namespace MasterDetailApp
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MasterDetailPage sealed
	{
	public:
		MasterDetailPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	private:
		::MasterDetailApp::ViewModels::ItemViewModel^ m_lastSelectedItem;

		void UpdateForVisualState(Windows::UI::Xaml::VisualState^ newState, Windows::UI::Xaml::VisualState^ oldState = nullptr);
		void EnableContentTransitions();
		void DisableContentTransitions();

		void LayoutRoot_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AdaptiveStates_CurrentStateChanged(Platform::Object^ sender, Windows::UI::Xaml::VisualStateChangedEventArgs^ e);
		void MasterListView_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
