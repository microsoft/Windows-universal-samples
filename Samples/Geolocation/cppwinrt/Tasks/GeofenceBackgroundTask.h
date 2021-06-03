#pragma once

#include "GeofenceBackgroundTask.g.h"

namespace winrt::BackgroundTask::implementation
{
    struct GeofenceBackgroundTask : GeofenceBackgroundTaskT<GeofenceBackgroundTask>
    {
        GeofenceBackgroundTask() = default;

        void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance const& taskInstance);

    private:
        Windows::Foundation::Collections::IPropertySet m_values{ Windows::Storage::ApplicationData::Current().LocalSettings().Values() };

        // Value determined by how many max length event descriptors (91 chars)
        // stored as a JSON string can fit in 8K (max allowed for local settings)
        static constexpr int maxEventDescriptors = 42;

        void ProcessGeofenceStateChangedReports();
        hstring GenerateReportDescription(Windows::Devices::Geolocation::Geofencing::GeofenceStateChangeReport const& report);
    };
}

namespace winrt::BackgroundTask::factory_implementation
{
    struct GeofenceBackgroundTask : GeofenceBackgroundTaskT<GeofenceBackgroundTask, implementation::GeofenceBackgroundTask>
    {
    };
}
