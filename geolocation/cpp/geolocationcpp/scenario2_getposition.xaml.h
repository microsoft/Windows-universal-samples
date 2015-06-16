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

//
// Scenario2_GetPosition.xaml.h
// Declaration of the Scenario2 class
//

#pragma once

#include "pch.h"
#include "Scenario2_GetPosition.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
	namespace GeolocationCPP
	{
		/// <summary>
		/// An empty page that can be used on its own or navigated to within a Frame.
		/// </summary>
		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class Scenario2 sealed
		{
		public:
			Scenario2();

		protected:
			virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
			virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

		private:
			MainPage^ rootPage;
			unsigned int desiredAccuracyInMetersValue = 0;
			concurrency::cancellation_token_source geopositionTaskTokenSource;

			void GetGeolocationButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void CancelGetGeolocationButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void DesiredAccuracyInMeters_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void UpdateLocationData(Windows::Devices::Geolocation::Geoposition^ position);
		};
	}
}