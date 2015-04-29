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
// GeofenceItem.h
// Declaration of the GeofenceItem class
//

#pragma once

namespace SDKTemplate
{
	namespace GeolocationCPP
	{
		// Since data binding uses Platform::Collections::Vector<Object^>^
		// the GeofenceItem must be implemented as a WinRT object.
		// This means that native data types cannot be used in
		// the interface.
		// Note that this is different than the 
		// implementation in C# where native types
		// are allowed because the collection uses
		// System.Collections.ObjectModel.ObservableCollection<GeofenceItem>
		// but System isn't available in C++ Windows Store Apps.
		// Also note the metadata that allows this class
		// to be used in data binding.
		// Also note that since System isn't available
		// GeofenceItem does not implement IEquatable to allow
		// removal of objects in the collection
		[Windows::UI::Xaml::Data::Bindable]
		public ref class GeofenceItem sealed
		{
		private:
			Windows::Devices::Geolocation::Geofencing::Geofence^ geofence;

		public:
			GeofenceItem(Windows::Devices::Geolocation::Geofencing::Geofence^ geofence)
			{
				this->geofence = geofence;
			}

			property Windows::Devices::Geolocation::Geofencing::Geofence^ Geofence
			{
				Windows::Devices::Geolocation::Geofencing::Geofence^ get();
			}

			property Platform::String^ Id
			{
				Platform::String^ get();
			}

			property double Latitude
			{
				double get();
			}

			property double Longitude
			{
				double get();
			}

			property double Radius
			{
				double get();
			}

			property long long DwellTime
			{
				long long get();
			}

			property bool SingleUse
			{
				bool get();
			}

			property unsigned int MonitoredStates
			{
				unsigned int get();
			}

			property long long Duration
			{
				long long get();
			}

			property long long StartTime
			{
				long long get();
			}
		};
	}
}