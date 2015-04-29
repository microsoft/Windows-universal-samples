// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario7_ListSmartCards.g.h"
#include "MainPage.xaml.h"

namespace Smartcard
{
	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class SmartCardItem sealed
	{
	public:
		SmartCardItem();

		property Platform::String^ ReaderName
		{
			Platform::String^  get()
			{
				return readerName;
			}

			void set(Platform::String^  value)
			{
				readerName = value;
			}
		}

		property Platform::String^ CardName
		{
			Platform::String^  get()
			{
				return cardName;
			}

			void set(Platform::String^  value)
			{
				cardName = value;
			}
		}
	private:
		Platform::String^ readerName;
		Platform::String^ cardName;
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario7_ListSmartCards sealed
	{
	public:
		Scenario7_ListSmartCards();
	private:
		void ListSmartCards_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		Platform::Collections::Vector<SmartCardItem^>^ smartCardItems;
	};
}