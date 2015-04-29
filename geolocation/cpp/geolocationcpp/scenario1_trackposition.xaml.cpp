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
// Scenario1_TrackPosition.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1_TrackPosition.xaml.h"


using namespace SDKTemplate;
using namespace SDKTemplate::GeolocationCPP;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;

Scenario1::Scenario1() : rootPage(MainPage::Current)
{
	InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
	StartTrackingButton->IsEnabled = true;
	StopTrackingButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario1::OnNavigatedFrom(NavigationEventArgs^ e)
{
	// If the navigation is external to the app do not clean up.
	// This can occur on Phone when suspending the app.
	if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
	{
		return;
	}

	if (StopTrackingButton->IsEnabled)
	{
		geolocator->PositionChanged::remove(positionToken);
		geolocator->StatusChanged::remove(statusToken);
	}
}

void Scenario1::StartTracking(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	task<GeolocationAccessStatus> geolocationAccessRequestTask(Windows::Devices::Geolocation::Geolocator::RequestAccessAsync());
	geolocationAccessRequestTask.then([this](task<GeolocationAccessStatus> accessStatusTask)
	{
		// Get will throw an exception if the task was canceled or failed with an error
		auto accessStatus = accessStatusTask.get();

		switch (accessStatus)
		{
		case GeolocationAccessStatus::Allowed:
			// You should set MovementThreshold for distance-based tracking
			// or ReportInterval for periodic-based tracking before adding event
			// handlers. If none is set, a ReportInterval of 1 second is used
			// as a default and a position will be returned every 1 second.
			//
			// Value of 2000 milliseconds (2 seconds)
			// isn't a requirement, it is just an example.
			if (geolocator == nullptr)
			{
				geolocator = ref new Geolocator();
				geolocator->ReportInterval = 2000;
			}

			// Subscribe to PositionChanged event to get updated tracking positions
			positionToken = geolocator->PositionChanged::add(ref new TypedEventHandler<Geolocator^, PositionChangedEventArgs^>(this, &Scenario1::OnPositionChanged));

			// Subscribe to StatusChanged event to get updates of location status change
			statusToken = geolocator->StatusChanged::add(ref new TypedEventHandler<Geolocator^, StatusChangedEventArgs^>(this, &Scenario1::OnStatusChanged));

			LocationDisabledMessage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			StartTrackingButton->IsEnabled = false;
			StopTrackingButton->IsEnabled = true;
			break;

		case GeolocationAccessStatus::Denied:
			rootPage->NotifyUser("Access to location is denied.", NotifyType::ErrorMessage);
			LocationDisabledMessage->Visibility = Windows::UI::Xaml::Visibility::Visible;
			break;

		case GeolocationAccessStatus::Unspecified:
			rootPage->NotifyUser("Unspecified error!", NotifyType::ErrorMessage);
		}
	});
}

void Scenario1::StopTracking(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	geolocator->PositionChanged::remove(positionToken);
	geolocator->StatusChanged::remove(statusToken);

	StartTrackingButton->IsEnabled = true;
	StopTrackingButton->IsEnabled = false;
}

void Scenario1::OnPositionChanged(Geolocator^ sender, PositionChangedEventArgs^ e)
{
	// We need to dispatch to the UI thread to display the output
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler(
		[this, e]()
	{
		rootPage->NotifyUser("Location updated", NotifyType::StatusMessage);

		auto coordinate = e->Position->Coordinate;
		ScenarioOutput_Latitude->Text = coordinate->Point->Position.Latitude.ToString();
		ScenarioOutput_Longitude->Text = coordinate->Point->Position.Longitude.ToString();
		ScenarioOutput_Accuracy->Text = coordinate->Accuracy.ToString();
	},
		CallbackContext::Any
		)
		);
}

void Scenario1::OnStatusChanged(Geolocator^ sender, StatusChangedEventArgs^ e)
{
	// We need to dispatch to the UI thread to display the output
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler(
		[this, e]()
	{
		switch (e->Status)
		{
		case Windows::Devices::Geolocation::PositionStatus::Ready:
			// Location platform is providing valid data.
			ScenarioOutput_Status->Text = "Ready";
			rootPage->NotifyUser("Location platform is ready.", NotifyType::StatusMessage);
			break;

		case Windows::Devices::Geolocation::PositionStatus::Initializing:
			// Location platform is attempting to acquire a fix.
			ScenarioOutput_Status->Text = "Initializing";
			rootPage->NotifyUser("Location platform is attempting to obtain a position.", NotifyType::StatusMessage);
			break;

		case Windows::Devices::Geolocation::PositionStatus::NoData:
			// Location platform could not obtain location data.
			ScenarioOutput_Status->Text = "No data";
			rootPage->NotifyUser("Not able to determine a location.", NotifyType::ErrorMessage);
			break;

		case Windows::Devices::Geolocation::PositionStatus::Disabled:
			// The permission to access location data is denied by the user or other policies.
			ScenarioOutput_Status->Text = "Disabled";
			rootPage->NotifyUser("Access to location is denied.", NotifyType::ErrorMessage);

			// Clear cached location data if any
			ScenarioOutput_Latitude->Text = "No data";
			ScenarioOutput_Longitude->Text = "No data";
			ScenarioOutput_Accuracy->Text = "No data";
			break;

		case Windows::Devices::Geolocation::PositionStatus::NotInitialized:
			// The location platform is not initialized. This indicates that the application
			// has not made a request for location data.
			ScenarioOutput_Status->Text = "Not initialized";
			rootPage->NotifyUser("No request for location is made yet.", NotifyType::StatusMessage);
			break;

		case Windows::Devices::Geolocation::PositionStatus::NotAvailable:
			// The location platform is not available on this version of the OS.
			ScenarioOutput_Status->Text = "Not available";
			rootPage->NotifyUser("Location is not available on this version of the OS.", NotifyType::ErrorMessage);
			break;

		default:
			ScenarioOutput_Status->Text = "Unknown";
			rootPage->NotifyUser("", NotifyType::StatusMessage);
			break;
		}
	},
		CallbackContext::Any
		)
		);
}
