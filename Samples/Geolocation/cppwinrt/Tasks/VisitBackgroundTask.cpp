#include "pch.h"
#include "Helpers.h"
#include "VisitBackgroundTask.h"
#include "VisitBackgroundTask.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Notifications;

namespace winrt::BackgroundTask::implementation
{
    void VisitBackgroundTask::Run(IBackgroundTaskInstance const& taskInstance)
    {
        BackgroundTaskDeferral deferral = taskInstance.GetDeferral();

        GeovisitTriggerDetails triggerDetails = taskInstance.TriggerDetails().as<GeovisitTriggerDetails>();

        ProcessVisitReports(triggerDetails);

        deferral.Complete();
    }

    void VisitBackgroundTask::ProcessVisitReports(GeovisitTriggerDetails const& triggerDetails)
    {
        // Load the values saved from last time.
        JsonArray visitBackgroundEvents = LoadSavedJson(L"BackgroundVisitEventCollection");

        // Read reports from the triggerDetails.
        hstring description;
        IVectorView<Geovisit> reports = triggerDetails.ReadReports();
        for (Geovisit report : reports)
        {
            description = GenerateVisitDescription(report);

            // Add to the collection, trimming old events if necessary.
            if (visitBackgroundEvents.Size() == maxEventDescriptors)
            {
                visitBackgroundEvents.RemoveAtEnd();
            }

            visitBackgroundEvents.InsertAt(0, JsonValue::CreateStringValue(description));
        }

        uint32_t numReports = reports.Size();
        if (numReports > 0)
        {
            // Save the modified results for next time.
            m_values.Insert(L"BackgroundVisitEventCollection", box_value(visitBackgroundEvents.Stringify()));

            if (numReports > 1)
            {
                description = L"There are " + to_hstring(numReports) + L" new visit events";
            }
            DisplayToast(description);
        }
    }

    hstring VisitBackgroundTask::GenerateVisitDescription(Geovisit const& visit)
    {
        hstring result = FormatDateTime(visit.Timestamp(), L"shortdate shorttime") + L" " + to_hstring(visit.StateChange());

        // Check if the report has a valid position.
        if (visit.Position() != nullptr)
        {
            result = result + L" (" +
                to_brief_hstring(visit.Position().Coordinate().Point().Position().Latitude) + L"," +
                to_brief_hstring(visit.Position().Coordinate().Point().Position().Longitude) + L")";
        }
        return result;
    }
}
