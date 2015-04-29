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
// Scenario4_ForegroundGeofence.xaml.h
// Declaration of the Scenario4 class
//

#pragma once

#include "pch.h"
#include "geofenceitem.h"
#include "Scenario4_ForegroundGeofence.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
	namespace GeolocationCPP
	{
		/// <summary>
		/// An empty page that can be used on its own or navigated to within a Frame.
		/// </summary>
		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class Scenario4 sealed
		{
		public:
			Scenario4();

		protected:
			virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
			virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

		private:
			void RefreshControlsFromGeofenceItem(GeofenceItem^ item);
			bool TextChangedHandlerDouble(bool nullAllowed, Platform::String^ name, Windows::UI::Xaml::Controls::TextBox^ e);
			bool TextChangedHandlerInt(bool nullAllowed, Platform::String^ name, Windows::UI::Xaml::Controls::TextBox^ e);
			bool SettingsAvailable();
			void AddGeofenceToRegisteredGeofenceListBox(Windows::Devices::Geolocation::Geofencing::Geofence^ geoFence);
			void GetGeopositionAsync();
			Windows::Devices::Geolocation::Geofencing::Geofence^ GenerateGeofence();
			void FillRegisteredGeofenceListBoxWithExistingGeofences();
			void FillEventListBoxWithExistingEvents();
			void SaveExistingEvents();
			void AddEventDescription(Platform::String^ eventDescription);
			Platform::String^ GetTimeStampedMessage(Platform::String^ EventCalled);
			void SetStartTimeToNowFunction();
			long long GetDurationFromString(Platform::String^ str);
			long long GetUniversalTimeFromString(Platform::String^ str);
			void GetTimeComponentAsString(Platform::String^& str, int timecomponent, bool appenddelimiter);
			Platform::String^ GetDurationString(long long duration);
			void Remove(Windows::Devices::Geolocation::Geofencing::Geofence^ geofence);

			void OnGeofenceStateChanged(Windows::Devices::Geolocation::Geofencing::GeofenceMonitor^ sender, Platform::Object^ args);
			void OnGeofenceStatusChanged(Windows::Devices::Geolocation::Geofencing::GeofenceMonitor^ sender, Platform::Object^ args);
			void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
#if (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)
			void OnAccessChanged(Windows::Devices::Enumeration::DeviceAccessInformation^ sender, Windows::Devices::Enumeration::DeviceAccessChangedEventArgs^ args);
#endif
			void OnIdTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
			void OnLatitudeTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
			void OnLongitudeTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
			void OnRadiusTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
			void OnRegisteredGeofenceListBoxSelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
			void OnRemoveGeofenceItem(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void OnSetStartTimeToNow(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void OnSetPositionToHere(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void OnCreateGeofence(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

			MainPage^ rootPage;
			bool nameSet;
			bool latitudeSet;
			bool longitudeSet;
			bool radiusSet;
			bool permissionsChecked;
			bool inGetPositionAsync;
			Windows::Devices::Geolocation::Geolocator^ geolocator;
			Windows::Foundation::Collections::IVector<Windows::Devices::Geolocation::Geofencing::Geofence^>^ geofences;
#if (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)
			Windows::Devices::Enumeration::DeviceAccessInformation^ accessInfo;
#endif
			Windows::Foundation::EventRegistrationToken geofenceStateChangeToken;
			Windows::Foundation::EventRegistrationToken geofenceStatusChangeToken;
			concurrency::cancellation_token_source geopositionTaskTokenSource;
			Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ formatterShortDateLongTime;
			Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ formatterLongTime;
			Windows::Globalization::Calendar^ calendar;
			Platform::Collections::Vector<GeofenceItem^>^ geofenceCollection;
			Platform::Collections::Vector<Platform::String^>^ eventCollection;
			Platform::Agile<Windows::UI::Core::CoreWindow> coreWindow;
		};
	}
}