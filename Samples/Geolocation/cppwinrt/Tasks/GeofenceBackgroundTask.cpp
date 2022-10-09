#include "pch.h"
#include "Helpers.h"
#include "GeofenceBackgroundTask.h"
#include "GeofenceBackgroundTask.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Devices::Geolocation::Geofencing;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::UI::Notifications;

namespace winrt::BackgroundTask::implementation
{
    void GeofenceBackgroundTask::Run(IBackgroundTaskInstance const& taskInstance)
    {
        BackgroundTaskDeferral deferral = taskInstance.GetDeferral();

        try
        {
            ProcessGeofenceStateChangedReports();
        }
        catch (hresult_access_denied const&)
        {
            // Report the error as "Status".
            m_values.Insert(L"Status", box_value(L"Location permissions are disabled. Enable access through Settings."));
            m_values.TryRemove(L"BackgroundGeofenceEventCollection");
        }
        deferral.Complete();
    }

    void GeofenceBackgroundTask::ProcessGeofenceStateChangedReports()
    {
        // Load the values saved from last time.
        JsonArray geofenceBackgroundEvents = LoadSavedJson(L"BackgroundGeofenceEventCollection");

        // Process the state changes that have occurred since the last time we ran.
        hstring description;
        GeofenceMonitor monitor = GeofenceMonitor::Current();
        IVectorView<GeofenceStateChangeReport> reports = monitor.ReadReports();
        for (GeofenceStateChangeReport report : reports)
        {
            description = GenerateReportDescription(report);

            // Add to the collection, trimming old events if necessary.
            if (geofenceBackgroundEvents.Size() == maxEventDescriptors)
            {
                geofenceBackgroundEvents.RemoveAtEnd();
            }

            geofenceBackgroundEvents.InsertAt(0, JsonValue::CreateStringValue(description));
        }

        uint32_t numReports = reports.Size();
        if (numReports > 0)
        {
            // Save the modified results for next time.
            m_values.Insert(L"BackgroundGeofenceEventCollection", box_value(geofenceBackgroundEvents.Stringify()));

            if (numReports > 1)
            {
                description = L"There are " + to_hstring(numReports) + L" new geofence events";
            }
            DisplayToast(description);
        }
    }

    hstring GeofenceBackgroundTask::GenerateReportDescription(GeofenceStateChangeReport const& report)
    {
        GeofenceState state = report.NewState();

        DateTime timestamp = report.Geoposition().Coordinate().Timestamp();
        hstring result = report.Geofence().Id() + L" " + FormatDateTime(timestamp, L"shortdate shorttime");

        if (state == GeofenceState::Removed)
        {
            GeofenceRemovalReason reason = report.RemovalReason();
            if (reason == GeofenceRemovalReason::Expired)
            {
                result = result + L" (Removed/Expired)";
            }
            else if (reason == GeofenceRemovalReason::Used)
            {
                result = result + L" (Removed/Used)";
            }
        }
        else if (state == GeofenceState::Entered)
        {
            result = result + L" (Entered)";
        }
        else if (state == GeofenceState::Exited)
        {
            result = result + L" (Exited)";
        }

        return result;
    }
}
