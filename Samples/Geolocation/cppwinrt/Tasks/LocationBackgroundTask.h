#pragma once

#include "LocationBackgroundTask.g.h"

namespace winrt::BackgroundTask::implementation
{
    struct LocationBackgroundTask : LocationBackgroundTaskT<LocationBackgroundTask>
    {
        LocationBackgroundTask() = default;

        fire_and_forget Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance const& taskInstance);

    private:
        slim_mutex m_mutex;
        Windows::Foundation::IAsyncOperation<Windows::Devices::Geolocation::Geoposition> m_pendingOperation;
        Windows::Foundation::Collections::IPropertySet m_values{ Windows::Storage::ApplicationData::Current().LocalSettings().Values() };

        void WriteGeolocToAppData(Windows::Devices::Geolocation::Geoposition const& pos);
        void WipeGeolocDataFromAppData();
        void WriteStatusToAppData(hstring const& status);
        void OnCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance const&, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason);
    };
}

namespace winrt::BackgroundTask::factory_implementation
{
    struct LocationBackgroundTask : LocationBackgroundTaskT<LocationBackgroundTask, implementation::LocationBackgroundTask>
    {
    };
}
