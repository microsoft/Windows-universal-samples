//
// DetailPage.xaml.h
// Declaration of the DetailPage class
//

#pragma once

#include "DetailPage.g.h"

namespace MasterDetailApp
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class DetailPage sealed
	{
	public:
		DetailPage();

		static property Windows::UI::Xaml::DependencyProperty^ ItemProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return s_itemProperty; }
		}

		property ::MasterDetailApp::ViewModels::ItemViewModel^ Item
		{
			::MasterDetailApp::ViewModels::ItemViewModel^ get()
			{
				return safe_cast<::MasterDetailApp::ViewModels::ItemViewModel^>(GetValue(s_itemProperty));
			}
			void set(::MasterDetailApp::ViewModels::ItemViewModel^ value)
			{
				SetValue(s_itemProperty, value);
			}
		}

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	internal:
		static void RegisterDependencyProperties();

	private:
		static Windows::UI::Xaml::DependencyProperty^ s_itemProperty;
		
		void OnBackRequested();
		void NavigateBackForWideState(bool useTransition);
		bool ShouldGoToWideState();

		Windows::Foundation::EventRegistrationToken m_sizeChangedEventRegistrationToken;
		Windows::Foundation::EventRegistrationToken m_backRequestedEventRegistrationToken;

		void BackButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void PageRoot_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void PageRoot_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Window_SizeChanged(Platform::Object ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^e);
		void DetailPage_BackRequested(Platform::Object ^sender, Windows::UI::Core::BackRequestedEventArgs ^args);
	};
}
