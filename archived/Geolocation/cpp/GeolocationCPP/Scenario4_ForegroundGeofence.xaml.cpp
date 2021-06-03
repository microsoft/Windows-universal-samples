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
// Scenario4_ForegroundGeofence.xaml.cpp
// Implementation of the Scenario4 class
//

#include "pch.h"
#include "Scenario4_ForegroundGeofence.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::GeolocationCPP;

using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Devices::Geolocation::Geofencing;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;

const unsigned int oneHundredNanosecondsPerSecond = 10000000;
const unsigned int defaultDwellTimeSeconds = 10;

Scenario4::Scenario4()
    : rootPage(MainPage::Current),
    nameSet(false),
    latitudeSet(false),
    longitudeSet(false),
    radiusSet(false),
    permissionsChecked(false),
    inGetPositionAsync(false)
{
    InitializeComponent();

    try
    {
        auto us = ref new Platform::Collections::Vector<String^>();
        us->Append("en-US");
        formatterShortDateLongTime = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("{month.integer}/{day.integer}/{year.full} {hour.integer}:{minute.integer(2)}:{second.integer(2)}", us, "US", Windows::Globalization::CalendarIdentifiers::Gregorian, Windows::Globalization::ClockIdentifiers::TwentyFourHour);
        formatterLongTime = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("{hour.integer}:{minute.integer(2)}:{second.integer(2)}", us, "US", Windows::Globalization::CalendarIdentifiers::Gregorian, Windows::Globalization::ClockIdentifiers::TwentyFourHour);
        calendar = ref new Windows::Globalization::Calendar();

        geofenceCollection = ref new Platform::Collections::Vector<GeofenceItem^>();
        eventCollection = ref new Platform::Collections::Vector<Platform::String^>();

        geolocator = ref new Geolocator();

        geofences = GeofenceMonitor::Current->Geofences;

        // using data binding to the root page collection of GeofenceItems
        RegisteredGeofenceListBox->DataContext = geofenceCollection;

        // using data binding to the root page collection of GeofenceItems associated with events
        GeofenceEventsListBox->DataContext = eventCollection;

        FillRegisteredGeofenceListBoxWithExistingGeofences();
        FillEventListBoxWithExistingEvents();

        coreWindow = CoreWindow::GetForCurrentThread(); // this needs to be set before InitializeComponent sets up event registration for app visibility
        coreWindow->VisibilityChanged += ref new TypedEventHandler<CoreWindow ^, VisibilityChangedEventArgs ^>(this, &Scenario4::OnVisibilityChanged);

        // register for state change events
        geofenceStateChangeToken = GeofenceMonitor::Current->GeofenceStateChanged += ref new TypedEventHandler<GeofenceMonitor^, Platform::Object^>(this, &Scenario4::OnGeofenceStateChanged);
        geofenceStatusChangeToken = GeofenceMonitor::Current->StatusChanged += ref new TypedEventHandler<GeofenceMonitor^, Platform::Object^>(this, &Scenario4::OnGeofenceStatusChanged);
    }
    catch (Platform::Exception^ ex)
    {
        // GeofenceMonitor failed in adding a geofence
        // exceptions could be from out of memory, lat/long out of range,
        // too long a name, not a unique name, specifying a start
        // time + duration that is still in the past

        // If ex->HResult is RPC_E_DISCONNECTED (0x80010108):
        // The Location Framework service event state is out of synchronization
        // with the Windows::Devices::Geolocation::Geofencing::GeofenceMonitor.
        // To recover remove all event handlers on the GeofenceMonitor or restart the application.
        // Once all event handlers are removed you may add back any event handlers and retry the operation.

        rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached. The Parameter
/// property is typically used to configure the page.</param>
void Scenario4::OnNavigatedTo(NavigationEventArgs^ e)
{
}

/// <summary>
/// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
/// </summary>
/// <param name="e">
/// Event data that can be examined by overriding code. The event data is representative
/// of the navigation that will unload the current Page unless canceled. The
/// navigation can potentially be canceled by setting e.Cancel to true.
/// </param>
void Scenario4::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    if (true == inGetPositionAsync)
    {
        geopositionTaskTokenSource.cancel();
    }

    GeofenceMonitor::Current->GeofenceStateChanged::remove(geofenceStateChangeToken);
    GeofenceMonitor::Current->StatusChanged::remove(geofenceStatusChangeToken);

    // save off the contents of the event collection
    SaveExistingEvents();
}

void Scenario4::OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args)
{
    // NOTE: After the app is no longer visible on the screen and before the app is suspended
    // you might want your app to use toast notification for any geofence activity.
    // By registering for VisibiltyChanged the app is notified when the app is no longer visible on the foreground.

    if (args->Visible)
    {
        // register for foreground events
        geofenceStateChangeToken = GeofenceMonitor::Current->GeofenceStateChanged += ref new TypedEventHandler<GeofenceMonitor^, Platform::Object^>(this, &Scenario4::OnGeofenceStateChanged);
        geofenceStatusChangeToken = GeofenceMonitor::Current->StatusChanged += ref new TypedEventHandler<GeofenceMonitor^, Platform::Object^>(this, &Scenario4::OnGeofenceStatusChanged);
    }
    else
    {
        // unregister foreground events (let background capture events)
        GeofenceMonitor::Current->GeofenceStateChanged::remove(geofenceStateChangeToken);
        GeofenceMonitor::Current->StatusChanged::remove(geofenceStatusChangeToken);
    }
}

void Scenario4::OnGeofenceStatusChanged(GeofenceMonitor^ sender, Platform::Object^ args)
{
    auto status = sender->Status;

    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
        [this, status]()
    {
        Platform::String^ eventDescription = GetTimeStampedMessage("Geofence Status");

        if (GeofenceMonitorStatus::Ready == status)
        {
            eventDescription += " (Ready)";
        }
        else if (GeofenceMonitorStatus::Initializing == status)
        {
            eventDescription += " (Initializing)";
        }
        else if (GeofenceMonitorStatus::NoData == status)
        {
            eventDescription += " (NoData)";
        }
        else if (GeofenceMonitorStatus::Disabled == status)
        {
            eventDescription += " (Disabled)";
        }
        else if (GeofenceMonitorStatus::NotInitialized == status)
        {
            eventDescription += " (NotInitialized)";
        }
        else if (GeofenceMonitorStatus::NotAvailable == status)
        {
            eventDescription += " (NotAvailable)";
        }
        else
        {
            eventDescription += " (Unknown)";
        }

        AddEventDescription(eventDescription);
    },
        CallbackContext::Any
        )
        );
}

void Scenario4::OnGeofenceStateChanged(GeofenceMonitor^ sender, Platform::Object^ args)
{
    auto reports = sender->ReadReports();

    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
        [this, reports]()
    {
        for (auto report : reports)
        {
            GeofenceState state = report->NewState;

            auto geofence = report->Geofence;

            Platform::String^ eventDescription = GetTimeStampedMessage(geofence->Id);

            if (state == GeofenceState::Removed)
            {
                GeofenceRemovalReason reason = report->RemovalReason;

                if (reason == GeofenceRemovalReason::Expired)
                {
                    eventDescription += " (Removed/Expired)";
                }
                else if (reason == GeofenceRemovalReason::Used)
                {
                    eventDescription += " (Removed/Used)";
                }
                else
                {
                    eventDescription += " (Removed/Unknown)";
                }

                AddEventDescription(eventDescription);

                // remove the geofence from the client side geofences collection
                Remove(geofence);

                // empty the registered geofence listbox and repopulate
                geofenceCollection->Clear();

                FillRegisteredGeofenceListBoxWithExistingGeofences();
            }
            else if (state == GeofenceState::Entered || state == GeofenceState::Exited)
            {
                // NOTE: You might want to write your app to take particular
                // action based on whether the app has internet connectivity.

                if (state == GeofenceState::Entered)
                {
                    eventDescription += " (Entered)";
                }
                else if (state == GeofenceState::Exited)
                {
                    eventDescription += " (Exited)";
                }

                AddEventDescription(eventDescription);
            }
        }
    },
        CallbackContext::Any
        )
        );
}

/// <summary>
/// This method removes the geofence from the client side geofences collection
/// </summary>
/// <param name="geofence"></param>
void Scenario4::Remove(Windows::Devices::Geolocation::Geofencing::Geofence^ geofence)
{
    try
    {
        unsigned int index = 0;

        if (geofences->IndexOf(geofence, &index))
        {
            geofences->RemoveAt(index);
        }
        else
        {
            auto strMsg = "Could not find Geofence " + geofence->Id + " in the geofences collection";

            rootPage->NotifyUser(strMsg, NotifyType::StatusMessage);
        }
    }
    catch (Platform::Exception^ ex)
    {
        rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
    }
}

/// <summary>
/// This is the click handler for the 'Remove Geofence Item' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4::OnRemoveGeofenceItem(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (nullptr != RegisteredGeofenceListBox->SelectedItem)
    {
        // get selected item
        GeofenceItem^ itemToRemove = safe_cast<GeofenceItem^>(RegisteredGeofenceListBox->SelectedItem);

        auto geofence = itemToRemove->Geofence;

        // remove the geofence from the client side geofences collection
        Remove(geofence);

        // empty the registered geofence listbox and repopulate
        geofenceCollection->Clear();

        FillRegisteredGeofenceListBoxWithExistingGeofences();
    }
}

Windows::Devices::Geolocation::Geofencing::Geofence^ Scenario4::GenerateGeofence()
{
    Geofence^ geofence = nullptr;

    try
    {
        String^ fenceKey = Id->Text;

        BasicGeoposition position;
        position.Latitude = FromStringTo<double>(Latitude->Text);
        position.Longitude = FromStringTo<double>(Longitude->Text);
        position.Altitude = 0.0;
        double radius = FromStringTo<double>(Radius->Text);

        // the geofence is a circular region
        Geocircle^ geocircle = ref new Geocircle(position, radius);

        bool singleUse = false;

        if (true == SingleUse->IsChecked->Value)
        {
            singleUse = true;
        }

        // want to listen for enter geofence, exit geofence and remove geofence events
        // you can select a subset of these event states
        MonitoredGeofenceStates mask = static_cast<MonitoredGeofenceStates>(0);

        mask = mask | MonitoredGeofenceStates::Entered;
        mask = mask | MonitoredGeofenceStates::Exited;
        mask = mask | MonitoredGeofenceStates::Removed;

        // setting up how long you need to be in geofence for enter event to fire
        TimeSpan dwellTime;

        bool useDwellTime = false;

        // use dwell if the textbox has text
        if (nullptr != DwellTime->Text)
        {
            dwellTime.Duration = GetDurationFromString(DwellTime->Text);

            if (0 != dwellTime.Duration)
            {
                useDwellTime = true;
            }
        }

        if (!useDwellTime)
        {
            dwellTime.Duration = defaultDwellTimeSeconds * oneHundredNanosecondsPerSecond;

            useDwellTime = true;
        }

        // setting up how long the geofence should be active
        TimeSpan duration;

        bool useDuration = false;

        // use duration if the textbox has text
        if (nullptr != Duration->Text)
        {
            duration.Duration = GetDurationFromString(Duration->Text);

            if (0 != duration.Duration)
            {
                useDuration = true;
            }
        }

        // setting up the start time of the geofence
        DateTime startTime;

        bool useStartTime = false;

        if (nullptr != StartTime->Text)
        {
            startTime.UniversalTime = GetUniversalTimeFromString(StartTime->Text);

            if (0 != startTime.UniversalTime)
            {
                useStartTime = true;
            }

            if (!useStartTime)
            {
                rootPage->NotifyUser("Could not determine Start Time", NotifyType::ErrorMessage);
            }
        }

        if (false == useStartTime && false == useDuration)
        {
            if (true == useDwellTime)
            {
                geofence = ref new Geofence(fenceKey, geocircle, mask, singleUse, dwellTime);
            }
        }
        else
        {
            if (!useStartTime)
            {
                calendar->SetToNow();
                startTime = calendar->GetDateTime();
            }

            if (!useDuration)
            {
                duration.Duration = 0;
            }

            geofence = ref new Geofence(fenceKey, geocircle, mask, singleUse, dwellTime, startTime, duration);
        }
    }
    catch (Platform::Exception^ ex)
    {
        // GeofenceMonitor failed in adding a geofence
        // exceptions could be from out of memory, lat/long out of range,
        // too long a name, not a unique name, specifying an activation
        // time + duration that is still in the past
        rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
    }

    return geofence;
}
