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

// VisitBackgroundTask.cpp
#include "pch.h"
#include "VisitBackgroundTask.h"

using namespace BackgroundTask;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Data::Json;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Storage;
using namespace Windows::UI::Notifications;

const unsigned int maxEventDescriptors = 42;    // Value determined by how many max length event descriptors (91 chars) 
                                                // stored as a JSON string can fit in 8K (max allowed for local settings)

VisitBackgroundTask::VisitBackgroundTask()
    : _visitBackgroundEvents(ref new Vector<String^>())
{
}

VisitBackgroundTask::~VisitBackgroundTask()
{
}

void VisitBackgroundTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    // Get the deferral object from the task instance
    BackgroundTaskDeferral^ deferral = taskInstance->GetDeferral();

    GeovisitTriggerDetails^ triggerDetails = safe_cast<GeovisitTriggerDetails^>(taskInstance->TriggerDetails);

    GetVisitReports(triggerDetails);

    // Indicate that the background task has completed
    deferral->Complete();
}


// Gets the new visits reports and saves them into local settings
void VisitBackgroundTask::GetVisitReports(GeovisitTriggerDetails^ triggerDetails)
{
     // First fill the _visitBackgroundEvents with previous existing reports saved in local settings.
    _visitBackgroundEvents->Clear();
    FillReportCollectionWithExistingReports();

    // Read reports from the triggerDetails.
    // If the user revoked location access, ReadReports returns 0 reports.
    IVectorView<Geovisit^>^ reports = triggerDetails->ReadReports();

    auto us = ref new Vector<String^>({ "en-US" });
    auto formatterLongTime = ref new DateTimeFormatter("{hour.integer}:{minute.integer(2)}:{second.integer(2)}", us, "US", CalendarIdentifiers::Gregorian, ClockIdentifiers::TwentyFourHour);

    unsigned int count = reports->Size;

    String^ visitItemEvent = nullptr;
  
    for (unsigned int index = 0; index < count; index++)
    {
        Geovisit^ report = reports->GetAt(index);

        visitItemEvent = formatterLongTime->Format(report->Timestamp);

        visitItemEvent += " " + report->StateChange.ToString();

        // Check if the report has a valid position.
        if (report->Position != nullptr)
        {
            Geoposition^ pos = report->Position;

            visitItemEvent += " (" +
              pos->Coordinate->Point->Position.Latitude.ToString() + "," +
              pos->Coordinate->Point->Position.Longitude.ToString() +
             ")";
        }

        // Now add each new visit report to _VisitBackgroundEvents.
        AddVisitReport(visitItemEvent);
    }

    if (0 != count)
    {
        // Save all the reports back to the local settings.
        SaveExistingEvents();

        // NOTE: Other notification mechanisms can be used here, such as Badge and/or Tile updates.
        DoToast(count, visitItemEvent);
    }
}

// Helper method to pop a toast
void VisitBackgroundTask::DoToast(int numEventsOfInterest, String^ eventName)
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
        auto secondLine = "There are " + numEventsOfInterest + " new visit events";
        nodeList->Item(1)->AppendChild(toastXml->CreateTextNode(secondLine));
    }

    // now create a xml node for the audio source
    IXmlNode^ toastNode = toastXml->SelectSingleNode("/toast");
    XmlElement^ audio = toastXml->CreateElement("audio");
    audio->SetAttribute("src", "ms-winsoundevent:Notification.SMS");

    ToastNotification^ toast = ref new ToastNotification(toastXml);
    ToastNotifier->Show(toast);
}

void VisitBackgroundTask::FillReportCollectionWithExistingReports()
{
    auto settings = ApplicationData::Current->LocalSettings->Values;
    if (settings->HasKey("BackgroundVisitEventCollection"))
    {
        String^ visitEvent = safe_cast<String^>(settings->Lookup("BackgroundVisitEventCollection"));

        if (0 != visitEvent->Length())
        {
            auto events = JsonValue::Parse(visitEvent)->GetArray();

            // NOTE: the events are accessed in reverse order
            // because the events were added to JSON from
            // newer to older.  AddVisitReport() adds
            // each new entry to the beginning of the collection
            // and throws away the last entry if the collection gets too large.
            for (int pos = events->Size - 1; pos >= 0; pos--)
            {
                auto element = events->GetAt(pos);
                AddVisitReport(element->GetString());
            }
        }
    }
}

void VisitBackgroundTask::SaveExistingEvents()
{
    auto jsonArray = ref new JsonArray();

    for (auto eventDescriptor : _visitBackgroundEvents)
    {
        jsonArray->Append(JsonValue::CreateStringValue(eventDescriptor->ToString()));
    }

    String^ jsonString = jsonArray->Stringify();

    auto settings = ApplicationData::Current->LocalSettings->Values;
    settings->Insert("BackgroundVisitEventCollection", jsonString);
}

void VisitBackgroundTask::AddVisitReport(String^ eventDescription)
{
    if (_visitBackgroundEvents->Size == maxEventDescriptors)
    {
        _visitBackgroundEvents->RemoveAtEnd();
    }

    _visitBackgroundEvents->InsertAt(0, eventDescription);
}
