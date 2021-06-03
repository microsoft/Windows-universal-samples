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
#include "Scenario6_GetLastVisit.xaml.h"

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

Scenario6::Scenario6() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario6::OnNavigatedTo(NavigationEventArgs^ e)
{
    GetLastVisitButton->IsEnabled = true;
}

void  Scenario6::GetLastVisitButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    GetLastVisitButton->IsEnabled = false;
    LocationDisabledMessage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    // Request permission to access location.
    create_task(Geolocator::RequestAccessAsync()).then([this](GeolocationAccessStatus accessStatus)
    {
        switch (accessStatus)
        {
        case GeolocationAccessStatus::Allowed:
            rootPage->NotifyUser("Waiting for update...", NotifyType::StatusMessage);

            // Get the last visit report, if any.
            return create_task(GeovisitMonitor::GetLastReportAsync()).then([this](Geovisit^ visit)
            {
                rootPage->NotifyUser("Location updated.", NotifyType::StatusMessage);
                return visit;
            });

        case GeolocationAccessStatus::Denied:
            rootPage->NotifyUser("Access to location is denied.", NotifyType::ErrorMessage);
            LocationDisabledMessage->Visibility = Windows::UI::Xaml::Visibility::Visible;
            break;

        case GeolocationAccessStatus::Unspecified:
        default:
            rootPage->NotifyUser("Unspecified error!", NotifyType::ErrorMessage);
            break;
        }
        return task_from_result<Geovisit^>(nullptr);
    }).then([this](Geovisit^ visit)
    {
        UpdateLastVisit(visit);
        GetLastVisitButton->IsEnabled = true;
    });
}

/// <summary>
/// Updates the user interface with the Geovisit data provided
/// </summary>
/// <param name="visit">Geovisit to display its details</param>
void Scenario6::UpdateLastVisit(Geovisit^ visit)
{
    if (visit == nullptr)
    {
        ScenarioOutput_Latitude->Text = "No data";
        ScenarioOutput_Longitude->Text = "No data";
        ScenarioOutput_Accuracy->Text = "No data";
        ScenarioOutput_Timestamp->Text = "No data";
        ScenarioOutput_VisitStateChange->Text = "No data";
    }
    else
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
}