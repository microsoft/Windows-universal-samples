#pragma once

#include "VisitBackgroundTask.g.h"

namespace winrt::BackgroundTask::implementation
{
    struct VisitBackgroundTask : VisitBackgroundTaskT<VisitBackgroundTask>
    {
        VisitBackgroundTask() = default;

        void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance const& taskInstance);

    private:
        Windows::Foundation::Collections::IPropertySet m_values{ Windows::Storage::ApplicationData::Current().LocalSettings().Values() };

        // Value determined by how many max length event descriptors (91 chars)
        // stored as a JSON string can fit in 8K (max allowed for local settings)
        static constexpr int maxEventDescriptors = 42;

        void ProcessVisitReports(Windows::Devices::Geolocation::GeovisitTriggerDetails const& triggerDetails);
        hstring GenerateVisitDescription(Windows::Devices::Geolocation::Geovisit const& visit);
    };
}

namespace winrt::BackgroundTask::factory_implementation
{
    struct VisitBackgroundTask : VisitBackgroundTaskT<VisitBackgroundTask, implementation::VisitBackgroundTask>
    {
    };
}
