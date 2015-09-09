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

// LocationBackgroundTask.cpp
#include "pch.h"
#include "LocationBackgroundTask.h"

using namespace BackgroundTask;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Data::Json;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Devices::Geolocation::Geofencing;
using namespace Windows::Foundation;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Storage;
using namespace Windows::UI::Notifications;

const int oneHundredNanosecondsPerSecond = 10000000;  // conversion from 100 nano-second resolution to seconds
const unsigned int maxEventDescriptors = 42;    // Value determined by how many max length event descriptors (91 chars) 
                                                // stored as a JSON string can fit in 8K (max allowed for local settings)

LocationBackgroundTask::LocationBackgroundTask()
{
}

LocationBackgroundTask::~LocationBackgroundTask()
{
}

void LocationBackgroundTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    // Get the deferral object from the task instance
    Platform::Agile<BackgroundTaskDeferral> deferral(taskInstance->GetDeferral());

    // Associate a cancellation handler with the background task
    taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(this, &LocationBackgroundTask::OnCanceled);

    Geolocator^ geolocator = ref new Geolocator();

    task<Geoposition^> geopositionTask(geolocator->GetGeopositionAsync(), geopositionTaskTokenSource.get_token());
    geopositionTask.then([this, deferral, geolocator](task<Geoposition^> getPosTask)
    {
        DateTimeFormatter^ dateFormatter = ref new DateTimeFormatter("longtime");
        auto settings = ApplicationData::Current->LocalSettings;

        try
        {
            // Get will throw an exception if the task was canceled or failed with an error
            Geoposition^ pos = getPosTask.get();

            // Write to LocalSettings to indicate that this background task ran
            settings->Values->Insert("Status", "Time: " + dateFormatter->Format(pos->Coordinate->Timestamp));
            settings->Values->Insert("Latitude", pos->Coordinate->Point->Position.Latitude.ToString());
            settings->Values->Insert("Longitude", pos->Coordinate->Point->Position.Longitude.ToString());
            settings->Values->Insert("Accuracy", pos->Coordinate->Accuracy.ToString());
        }
        catch (Platform::AccessDeniedException^)
        {
            // Write to LocalSettings to indicate that this background task ran
            settings->Values->Insert("Status", "Disabled");
            settings->Values->Insert("Latitude", "No data");
            settings->Values->Insert("Longitude", "No data");
            settings->Values->Insert("Accuracy", "No data");
        }
        catch (task_canceled&)
        {
        }
        catch (Exception^ ex)
        {
            settings->Values->Insert("Latitude", "No data");
            settings->Values->Insert("Longitude", "No data");
            settings->Values->Insert("Accuracy", "No data");

#if (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)
            // If there are no location sensors GetGeopositionAsync()
            // will timeout -- that is acceptable.

            if (ex->HResult == HRESULT_FROM_WIN32(WAIT_TIMEOUT))
            {
                settings->Values->Insert("Status", "Operation accessing location sensors timed out. Possibly there are no location sensors.");
            }
            else
#endif
            {
                settings->Values->Insert("Status", ex->ToString());
            }
        }

        // Indicate that the background task has completed
        deferral->Complete();
    });
}

// Handles background task cancellation
void LocationBackgroundTask::OnCanceled(IBackgroundTaskInstance^ taskInstance, BackgroundTaskCancellationReason reason)
{
    // Cancel the async operation
    geopositionTaskTokenSource.cancel();
}

GeofenceBackgroundTask::GeofenceBackgroundTask()
{
    geofenceBackgroundEvents = ref new Platform::Collections::Vector<Platform::String^>();
}

GeofenceBackgroundTask::~GeofenceBackgroundTask()
{
}

void GeofenceBackgroundTask::Invoke(Windows::Foundation::IAsyncAction^ asyncInfo, Windows::Foundation::AsyncStatus asyncStatus)
{
}

void GeofenceBackgroundTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    // Get the deferral object from the task instance
    Platform::Agile<BackgroundTaskDeferral> deferral(taskInstance->GetDeferral());

    // Associate a cancellation handler with the background task
    taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(this, &GeofenceBackgroundTask::OnCanceled);

    Geolocator^ geolocator = ref new Geolocator();

    task<Geoposition^> geopositionTask(geolocator->GetGeopositionAsync(), geopositionTaskTokenSource.get_token());
    geopositionTask.then([this, deferral, geolocator](task<Geoposition^> getPosTask)
    {
        auto settings = ApplicationData::Current->LocalSettings;

        try
        {
            // Get will throw an exception if the task was canceled or failed with an error
            Geoposition^ pos = getPosTask.get();

            GetGeofenceStateChangedReports(pos);
        }
        catch (Platform::AccessDeniedException^)
        {
            // Write to LocalSettings to indicate that this background task ran
            settings->Values->Insert("Status", "Disabled");
        }
        catch (task_canceled&)
        {
        }
        catch (Exception^ ex)
        {
#if (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)
            // If there are no location sensors GetGeopositionAsync()
            // will timeout -- that is acceptable.

            if (ex->HResult == HRESULT_FROM_WIN32(WAIT_TIMEOUT))
            {
                settings->Values->Insert("Status", "Operation accessing location sensors timed out. Possibly there are no location sensors.");
            }
            else
#endif
            {
                settings->Values->Insert("Status", ex->ToString());
            }
        }

        // Indicate that the background task has completed
        deferral->Complete();
    });
}

// Handles background task cancellation
void GeofenceBackgroundTask::OnCanceled(IBackgroundTaskInstance^ taskInstance, BackgroundTaskCancellationReason reason)
{
    // Cancel the async operation
    geopositionTaskTokenSource.cancel();
}

// Stores geofence events into local settings
void GeofenceBackgroundTask::GetGeofenceStateChangedReports(Geoposition^ pos)
{
    FillEventCollectionWithExistingEvents();

    GeofenceMonitor^ monitor = GeofenceMonitor::Current;

    Geoposition^ posLastKnown = monitor->LastKnownGeoposition;
    DateTime eventDateTime = posLastKnown->Coordinate->Timestamp;

    auto calendar = ref new Windows::Globalization::Calendar();
    auto us = ref new Platform::Collections::Vector<String^>();
    us->Append("en-US");
    auto formatterLongTime = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("{hour.integer}:{minute.integer(2)}:{second.integer(2)}", us, "US", Windows::Globalization::CalendarIdentifiers::Gregorian, Windows::Globalization::ClockIdentifiers::TwentyFourHour);

    bool eventOfInterest = true;

    // NOTE TO DEVELOPER:
    // These events can be filtered out if the
    // geofence event time is stale.

    calendar->SetToNow();
    DateTime nowDateTime = calendar->GetDateTime();

    DateTime diffDateTime;
    diffDateTime.UniversalTime = nowDateTime.UniversalTime - eventDateTime.UniversalTime;

    long long deltaInSeconds = diffDateTime.UniversalTime / oneHundredNanosecondsPerSecond;

    // NOTE TO DEVELOPER:
    // If the time difference between the geofence event and now is too large
    // the eventOfInterest should be set to false.

    if (true == eventOfInterest)
    {
        // NOTE TO DEVELOPER:
        // This event can be filtered out if the
        // geofence event location is too far away.
        if ((posLastKnown->Coordinate->Point->Position.Latitude != pos->Coordinate->Point->Position.Latitude) ||
            (posLastKnown->Coordinate->Point->Position.Longitude != pos->Coordinate->Point->Position.Longitude))
        {
            // NOTE TO DEVELOPER:
            // Use an algorithm like the Haversine formula or Vincenty's formulae to determine
            // the distance between the current location (pos->Coordinate)
            // and the location of the geofence event (latitudeEvent/longitudeEvent).
            // If too far apart set eventOfInterest to false to
            // filter the event out.
        }

        if (true == eventOfInterest)
        {
            // Retreive a vector of geofence state changed reports
            auto reports = monitor->ReadReports();

            unsigned int count = reports->Size;

            String^ geofenceItemEvent = nullptr;

            int numEventsOfInterest = count;

            for (unsigned int loop = 0; loop < count; loop++)
            {
                auto report = reports->GetAt(loop);

                GeofenceState state = report->NewState;

                geofenceItemEvent = report->Geofence->Id + " " + formatterLongTime->Format(eventDateTime);

                if (state == GeofenceState::Removed)
                {
                    GeofenceRemovalReason reason = report->RemovalReason;

                    if (reason == GeofenceRemovalReason::Expired)
                    {
                        geofenceItemEvent += " (Removed/Expired)";
                    }
                    else if (reason == GeofenceRemovalReason::Used)
                    {
                        geofenceItemEvent += " (Removed/Used)";
                    }
                }
                else if (state == GeofenceState::Entered)
                {
                    geofenceItemEvent += " (Entered)";
                }
                else if (state == GeofenceState::Exited)
                {
                    geofenceItemEvent += " (Exited)";
                }

                AddEventDescription(geofenceItemEvent);
            }

            if (true == eventOfInterest && 0 != numEventsOfInterest)
            {
                SaveExistingEvents();

                // NOTE: Other notification mechanisms can be used here, such as Badge and/or Tile updates.
                DoToast(numEventsOfInterest, geofenceItemEvent);
            }
        }
    }
}

// Helper method to pop a toast
void GeofenceBackgroundTask::DoToast(int numEventsOfInterest, Platform::String^ eventName)
{
    // pop a toast for each geofence event
    ToastNotifier^ ToastNotifier = ToastNotificationManager::CreateToastNotifier();

    // Create a two line toast and add audio reminder

    // Here the xml that will be passed to the 
    // ToastNotification for the toast is retrieved
    XmlDocument^ toastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastText02);

    // Set both lines of text
    XmlNodeList^ nodeList = toastXml->GetElementsByTagName("text");
    nodeList->Item(0)->AppendChild(toastXml->CreateTextNode("Geolocation Sample"));

    if (1 == numEventsOfInterest)
    {
        nodeList->Item(1)->AppendChild(toastXml->CreateTextNode(eventName));
    }
    else
    {
        auto secondLine = "There are " + numEventsOfInterest + " new geofence events";
        nodeList->Item(1)->AppendChild(toastXml->CreateTextNode(secondLine));
    }

    // now create a xml node for the audio source
    IXmlNode^ toastNode = toastXml->SelectSingleNode("/toast");
    XmlElement^ audio = toastXml->CreateElement("audio");
    audio->SetAttribute("src", "ms-winsoundevent:Notification.SMS");

    ToastNotification^ toast = ref new ToastNotification(toastXml);
    ToastNotifier->Show(toast);
}

void GeofenceBackgroundTask::FillEventCollectionWithExistingEvents()
{
    auto settings = ApplicationData::Current->LocalSettings->Values;
    if (settings->HasKey("BackgroundGeofenceEventCollection"))
    {
        String^ geofenceEvent = safe_cast<String^>(settings->Lookup("BackgroundGeofenceEventCollection"));

        if (0 != geofenceEvent->Length())
        {
            auto events = JsonValue::Parse(geofenceEvent)->GetArray();

            // NOTE: the events are accessed in reverse order
            // because the events were added to JSON from
            // newer to older.  AddEventDescription() adds
            // each new entry to the beginning of the collection.
            for (int pos = events->Size - 1; pos >= 0; pos--)
            {
                auto element = events->GetAt(pos);
                AddEventDescription(element->GetString());
            }
        }
    }
}

void GeofenceBackgroundTask::SaveExistingEvents()
{
    auto jsonArray = ref new JsonArray();

    for (auto eventDescriptor : geofenceBackgroundEvents)
    {
        jsonArray->Append(JsonValue::CreateStringValue(eventDescriptor->ToString()));
    }

    String^ jsonString = jsonArray->Stringify();

    auto settings = ApplicationData::Current->LocalSettings->Values;
    settings->Insert("BackgroundGeofenceEventCollection", jsonString);
}

void GeofenceBackgroundTask::AddEventDescription(Platform::String^ eventDescription)
{
    if (geofenceBackgroundEvents->Size == maxEventDescriptors)
    {
        geofenceBackgroundEvents->RemoveAtEnd();
    }

    geofenceBackgroundEvents->InsertAt(0, eventDescription);
}
