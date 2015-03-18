// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_XmlReader.g.h"
#include "MainPage.xaml.h"

namespace XmlLite
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario1 sealed
	{
	public:
		Scenario1();

	private:
		MainPage^ rootPage;
		void ReadXmlClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		HRESULT ReadXml(Windows::Storage::Streams::IRandomAccessStream^ readStream);
		HRESULT ReadAttributes(IXmlReader* reader);
	};
}

