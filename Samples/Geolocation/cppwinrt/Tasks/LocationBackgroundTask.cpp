#include "pch.h"
#include "Helpers.h"
#include "LocationBackgroundTask.h"
#include "LocationBackgroundTask.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;

namespace winrt::BackgroundTask::implementation
{
    fire_and_forget LocationBackgroundTask::Run(IBackgroundTaskInstance const& taskInstance)
    {
        auto lifetime = get_strong();

        BackgroundTaskDeferral deferral = taskInstance.GetDeferral();

        try
        {
            // Associate a cancellation handler with the background task.
            taskInstance.Canceled({ get_weak(), &LocationBackgroundTask::OnCanceled });

            // Create geolocator object
            Geolocator geolocator;

            // Make the request for the current position
            auto operation = geolocator.GetGeopositionAsync();
            {
                auto guard = slim_lock_guard(m_mutex);
                m_pendingOperation = operation;
            }
            Geoposition pos = co_await operation;
            {
                auto guard = slim_lock_guard(m_mutex);
                m_pendingOperation = nullptr;
            }

            WriteStatusToAppData(L"Time: " + FormatDateTime(clock::now(), L"shortdate shorttime"));
            WriteGeolocToAppData(pos);
        }
        catch (hresult_access_denied const&)
        {
            WriteStatusToAppData(L"Disabled");
            WipeGeolocDataFromAppData();
        }
        catch (hresult_error const& ex)
        {
            WriteStatusToAppData(ex.message());
            WipeGeolocDataFromAppData();
        }
        catch (std::exception const& ex)
        {
            WriteStatusToAppData(to_hstring(ex.what()));
            WipeGeolocDataFromAppData();
        }
        deferral.Complete();
    }

    void LocationBackgroundTask::WriteGeolocToAppData(Geoposition const& position)
    {
        m_values.Insert(L"Latitude", box_value(to_hstring(position.Coordinate().Point().Position().Latitude)));
        m_values.Insert(L"Longitude", box_value(to_hstring(position.Coordinate().Point().Position().Longitude)));
        m_values.Insert(L"Accuracy", box_value(to_hstring(position.Coordinate().Accuracy())));
    }

    void LocationBackgroundTask::WipeGeolocDataFromAppData()
    {
        m_values.TryRemove(L"Latitude");
        m_values.TryRemove(L"Longitude");
        m_values.TryRemove(L"Accuracy");
    }

    void LocationBackgroundTask::WriteStatusToAppData(hstring const& status)
    {
        m_values.Insert(L"Status", box_value(status));
    }

    void LocationBackgroundTask::OnCanceled(IBackgroundTaskInstance const&, BackgroundTaskCancellationReason)
    {
        IAsyncOperation<Geoposition> operation;
        {
            auto guard = slim_lock_guard(m_mutex);
            operation = m_pendingOperation;
        }
        if (operation)
        {
            operation.Cancel();
        }
    }
}
