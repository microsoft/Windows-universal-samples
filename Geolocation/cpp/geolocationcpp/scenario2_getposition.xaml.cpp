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
// Scenario2_GetPosition.xaml.cpp
// Implementation of the Scenario2 class
//

#include "pch.h"
#include "Scenario2_GetPosition.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::GeolocationCPP;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;
using namespace concurrency;

Scenario2::Scenario2() : rootPage(MainPage::Current)
{
	InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2::OnNavigatedTo(NavigationEventArgs^ e)
{
	GetGeolocationButton->IsEnabled = true;
	CancelGetGeolocationButton->IsEnabled = false;
	DesiredAccuracyInMeters->IsEnabled = true;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario2::OnNavigatedFrom(NavigationEventArgs^ e)
{
	if (CancelGetGeolocationButton->IsEnabled)
	{
		geopositionTaskTokenSource.cancel();
	}
}

void Scenario2::GetGeolocationButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GetGeolocationButton->IsEnabled = false;
	CancelGetGeolocationButton->IsEnabled = true;
	LocationDisabledMessage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

	try
	{
		task<GeolocationAccessStatus> geolocationAccessRequestTask(Windows::Devices::Geolocation::Geolocator::RequestAccessAsync());
		geolocationAccessRequestTask.then([this](task<GeolocationAccessStatus> accessStatusTask)
		{
			// Get will throw an exception if the task was canceled or failed with an error
			auto accessStatus = accessStatusTask.get();

			if (accessStatus == GeolocationAccessStatus::Allowed)
			{
				rootPage->NotifyUser("Waiting for update...", NotifyType::StatusMessage);

				auto geolocator = ref new Windows::Devices::Geolocation::Geolocator();
				geolocator->DesiredAccuracyInMeters = desiredAccuracyInMetersValue;

				task<Geoposition^> geopositionTask(geolocator->GetGeopositionAsync(), geopositionTaskTokenSource.get_token());
				geopositionTask.then([this](task<Geoposition^> getPosTask)
				{
					rootPage->NotifyUser("Location updated.", NotifyType::StatusMessage);

					// Get will throw an exception if the task was canceled or failed with an error
					UpdateLocationData(getPosTask.get());

					GetGeolocationButton->IsEnabled = true;
					CancelGetGeolocationButton->IsEnabled = false;
				});
			}
			else if (accessStatus == GeolocationAccessStatus::Denied)
			{
				rootPage->NotifyUser("Access to location is denied.", NotifyType::ErrorMessage);
				LocationDisabledMessage->Visibility = Windows::UI::Xaml::Visibility::Visible;
				UpdateLocationData(nullptr);
			}
			else //GeolocationAccessStatus::Unspecified:
			{
				rootPage->NotifyUser("Unspecified error!", NotifyType::ErrorMessage);
				UpdateLocationData(nullptr);
			}
		});
	}
	catch (task_canceled&)
	{
		rootPage->NotifyUser("Canceled", NotifyType::StatusMessage);
	}
	catch (Exception^ ex)
	{
		rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
	}
}

void Scenario2::CancelGetGeolocationButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	geopositionTaskTokenSource.cancel();

	GetGeolocationButton->IsEnabled = true;
	CancelGetGeolocationButton->IsEnabled = false;
}

void Scenario2::DesiredAccuracyInMeters_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	bool goodValue = true;

	unsigned int value = FromStringTo<unsigned int>(DesiredAccuracyInMeters->Text);

	if (0 == value)
	{
		// make sure string was '0'
		if ("0" != DesiredAccuracyInMeters->Text)
		{
			rootPage->NotifyUser("Desired Accuracy must be a number", NotifyType::StatusMessage);
			goodValue = false;
		}
	}

	if (true == goodValue)
	{
		// clear out status message
		rootPage->NotifyUser("", NotifyType::StatusMessage);
	}
}

void Scenario2::UpdateLocationData(Windows::Devices::Geolocation::Geoposition^ position)
{
	if (position == nullptr)
	{
		ScenarioOutput_Latitude->Text = "No data";
		ScenarioOutput_Longitude->Text = "No data";
		ScenarioOutput_Accuracy->Text = "No data";
	}
	else
	{
		ScenarioOutput_Latitude->Text = position->Coordinate->Point->Position.Latitude.ToString();
		ScenarioOutput_Longitude->Text = position->Coordinate->Point->Position.Longitude.ToString();
		ScenarioOutput_Accuracy->Text = position->Coordinate->Accuracy.ToString();
		ScenarioOutput_Source->Text = position->Coordinate->PositionSource.ToString();
		
		if (position->Coordinate->PositionSource == PositionSource::Satellite)
		{
			// show labels and satellite data
			Label_PosPrecision->Opacity = 1;
			ScenarioOutput_PosPrecision->Opacity = 1;
			ScenarioOutput_PosPrecision->Text = position->Coordinate->SatelliteData->PositionDilutionOfPrecision->ToString();
			Label_HorzPrecision->Opacity = 1;
			ScenarioOutput_HorzPrecision->Opacity = 1;
			ScenarioOutput_HorzPrecision->Text = position->Coordinate->SatelliteData->HorizontalDilutionOfPrecision->ToString();
			Label_VertPrecision->Opacity = 1;
			ScenarioOutput_VertPrecision->Opacity = 1;
			ScenarioOutput_VertPrecision->Text = position->Coordinate->SatelliteData->VerticalDilutionOfPrecision->ToString();
		}
		else
		{
			// hide labels and satellite data
			Label_PosPrecision->Opacity = 0;
			ScenarioOutput_PosPrecision->Opacity = 0;
			Label_HorzPrecision->Opacity = 0;
			ScenarioOutput_HorzPrecision->Opacity = 0;
			Label_VertPrecision->Opacity = 0;
			ScenarioOutput_VertPrecision->Opacity = 0;
		}
	}
}

