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
// Scenario1_TrackPosition.xaml.h
// Declaration of the Scenario1 class
//

#pragma once

#include "pch.h"
#include "Scenario1_TrackPosition.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
	namespace GeolocationCPP
	{
		/// <summary>
		/// An empty page that can be used on its own or navigated to within a Frame.
		/// </summary>
		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class Scenario1 sealed
		{
		public:
			Scenario1();

		protected:
			virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
			virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

		private:
			MainPage^ rootPage;
			Windows::Devices::Geolocation::Geolocator^ geolocator = nullptr;
			Windows::Foundation::EventRegistrationToken positionToken;
			Windows::Foundation::EventRegistrationToken statusToken;

			void OnPositionChanged(Windows::Devices::Geolocation::Geolocator^ sender, Windows::Devices::Geolocation::PositionChangedEventArgs^ e);
			void OnStatusChanged(Windows::Devices::Geolocation::Geolocator^ sender, Windows::Devices::Geolocation::StatusChangedEventArgs^ e);
			void StartTracking(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void StopTracking(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		};
	}
}