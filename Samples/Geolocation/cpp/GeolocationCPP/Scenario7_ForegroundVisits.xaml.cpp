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

#include "pch.h"
#include "Scenario7_ForegroundVisits.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::GeolocationCPP;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario7::Scenario7() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario7::OnNavigatedTo(NavigationEventArgs^ e)
{
    StartMonitoringButton->IsEnabled = true;
    StopMonitoringButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario7::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (_visitMonitor)
    {
        _visitMonitor->Stop();
    }
}


/// <summary>
/// This is the click handler for the 'StartMonitoring' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario7::StartMonitoring(Object^ sender, RoutedEventArgs^ e)
{
    // Request permission to access location.
    create_task(Geolocator::RequestAccessAsync()).then([this](GeolocationAccessStatus accessStatus)
    {
        switch (accessStatus)
        {
        case GeolocationAccessStatus::Allowed:
            _visitMonitor = ref new GeovisitMonitor();

            // Add visit state changed event handler
            _visitMonitor->VisitStateChanged += ref new TypedEventHandler<GeovisitMonitor^, GeovisitStateChangedEventArgs^>(this, &Scenario7::OnVisitStateChanged);

            // Start monitoring with the desired scope.
            // For higher granularity, choose venue/building level changes.
            // For lower granularity more or less in the range of zipcode level changes, choose city.
            // Choosing Venue here as an example.
            _visitMonitor->Start(VisitMonitoringScope::Venue);

            LocationDisabledMessage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            rootPage->NotifyUser("Waiting for update...", NotifyType::StatusMessage);

            StartMonitoringButton->IsEnabled = false;
            StopMonitoringButton->IsEnabled = true;
            break;

        case GeolocationAccessStatus::Denied:
            rootPage->NotifyUser("Access to location is denied.", NotifyType::ErrorMessage);
            LocationDisabledMessage->Visibility = Windows::UI::Xaml::Visibility::Visible;
            break;

        case GeolocationAccessStatus::Unspecified:
        default:
            rootPage->NotifyUser("Unspecified error!", NotifyType::ErrorMessage);
            LocationDisabledMessage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            break;
        }
    });
}


/// <summary>
/// This is the click handler for the 'StopMonitoring' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario7::StopMonitoring(Object^ sender, RoutedEventArgs^ e)
{
    // Stop visit monitoring.
    _visitMonitor->Stop();
    _visitMonitor = nullptr;

    StartMonitoringButton->IsEnabled = true;
    StopMonitoringButton->IsEnabled = false;

    // Clear status
    rootPage->NotifyUser("", NotifyType::StatusMessage);
}

/// <summary>
/// Event handler for Visitstatechanged events. It is raised when
/// a new visit state change is available based on the monitoring scope specified.
/// </summary>
/// <param name="sender">Geovisitmonitor instance</param>
/// <param name="args">visitstatechanged args data</param>
void Scenario7::OnVisitStateChanged(GeovisitMonitor^ sender, GeovisitStateChangedEventArgs^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        rootPage->NotifyUser("Visit state changed.", NotifyType::StatusMessage);
        UpdateVisitData(args->Visit);
    }));
}

/// <summary>
/// Updates the user interface with the Geovisit data provided
/// </summary>
/// <param name="visit">Geovisit to display its details</param>
void Scenario7::UpdateVisitData(Geovisit^ visit)
{
    // A valid visit is available, extract the state change and Timestamp.
    ScenarioOutput_VisitStateChange->Text = visit->StateChange.ToString();

    DateTimeFormatter^ dateFormatter = ref new DateTimeFormatter("shortdate longtime");
    ScenarioOutput_Timestamp->Text = dateFormatter->Format(visit->Timestamp);

    // If a valid position is available, extract the position information that caused the state change to happen.
    if (visit->Position == nullptr)
    {
        ScenarioOutput_Latitude->Text = "No data";
        ScenarioOutput_Longitude->Text = "No data";
        ScenarioOutput_Accuracy->Text = "No data";
    }
    else
    {
        ScenarioOutput_Latitude->Text = visit->Position->Coordinate->Point->Position.Latitude.ToString();
        ScenarioOutput_Longitude->Text = visit->Position->Coordinate->Point->Position.Longitude.ToString();
        ScenarioOutput_Accuracy->Text = visit->Position->Coordinate->Accuracy.ToString();
    }
}