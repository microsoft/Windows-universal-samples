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
#include "Scenario3_LocationTrackingReason.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::ExtendedExecution;
using namespace Windows::Devices::Geolocation;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

LocationTrackingReason::LocationTrackingReason()
{
    InitializeComponent();
}

void LocationTrackingReason::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    // End the Extended Execution Session.
    // Only one extended execution session can be held by an application at one time.
    ClearExtendedExecution();
}

void LocationTrackingReason::ClearExtendedExecution()
{
    if (session)
    {
        session->Revoked -= sessionRevokedToken;
        delete session;
        session = nullptr;
    }

    if (periodicTimer)
    {
        periodicTimer->Cancel();
        periodicTimer = nullptr;
    }

    geolocator = nullptr;
}

void LocationTrackingReason::UpdateUI()
{
    if (!session)
    {
        Status->Text = "Not requested";
        RequestButton->IsEnabled = true;
        CloseButton->IsEnabled = false;
    }
    else
    {
        Status->Text = "Requested";
        RequestButton->IsEnabled = false;
        CloseButton->IsEnabled = true;
    }
}

task<void> LocationTrackingReason::StartLocationTrackingAsync()
{
    // Request permission to access location
    return create_task(Geolocator::RequestAccessAsync()).then([this](GeolocationAccessStatus accessStatus)
    {
        switch (accessStatus)
        {
        case GeolocationAccessStatus::Allowed:
            // See the Geolocation sample for more information on using the Geolocator class.
            geolocator = ref new Geolocator();
            geolocator->ReportInterval = 2000;
            break;

        case GeolocationAccessStatus::Denied:
            rootPage->NotifyUser("Access to location is denied.", NotifyType::ErrorMessage);
            break;

        default:
        case GeolocationAccessStatus::Unspecified:
            rootPage->NotifyUser("Couldn't access the geolocator.", NotifyType::ErrorMessage);
            break;
        }
    });
}

void LocationTrackingReason::BeginExtendedExecution()
{
    // The previous Extended Execution must be closed before a new one can be requested.
    // This code is redundant here because the sample doesn't allow a new extended
    // execution to begin until the previous one ends, but we leave it here for illustration.
    ClearExtendedExecution();

    auto newSession = ref new ExtendedExecutionSession();
    newSession->Reason = ExtendedExecutionReason::LocationTracking;
    newSession->Description = "Tracking your location";
    sessionRevokedToken = newSession->Revoked += ref new TypedEventHandler<Object^, ExtendedExecutionRevokedEventArgs^>(
        this, &LocationTrackingReason::SessionRevoked);
    create_task(newSession->RequestExtensionAsync()).then([this, newSession](ExtendedExecutionResult result)
    {
        switch (result)
        {
        case ExtendedExecutionResult::Allowed:
            rootPage->NotifyUser("Extended execution allowed.", NotifyType::StatusMessage);
            session = newSession;
            return StartLocationTrackingAsync();
            break;

        default:
        case ExtendedExecutionResult::Denied:
            rootPage->NotifyUser("Extended execution denied.", NotifyType::ErrorMessage);
            delete newSession;
            return task_from_result();
            break;
        }
    }, task_continuation_context::get_current_winrt_context()).then([this]()
    {
        if (geolocator)
        {
            periodicTimer = ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler(this, &LocationTrackingReason::OnTimer), Helpers::TimeSpanFromSeconds(10));
        }
        UpdateUI();
    }, task_continuation_context::get_current_winrt_context());
}

void LocationTrackingReason::OnTimer(ThreadPoolTimer^ timer)
{
    task<String^> messageTask;

    if (geolocator == nullptr)
    {
        messageTask = task_from_result<String^>("No geolocator");
    }
    else
    {
        messageTask = create_task(geolocator->GetGeopositionAsync()).then([this](Geoposition^ geoposition)
        {
            String^ message;
            if (geoposition == nullptr)
            {
                message = "Cannot get current location";
            }
            else
            {
                BasicGeoposition basicPosition = geoposition->Coordinate->Point->Position;
                message = "Longitude = " + basicPosition.Longitude.ToString() + ", Latitude = " + basicPosition.Latitude.ToString();
            }
            return message;
        });
    }

    messageTask.then([this](String^ message)
    {
        MainPage::DisplayToast(message);
    });
}

void LocationTrackingReason::EndExtendedExecution()
{
    ClearExtendedExecution();
    UpdateUI();
}

void LocationTrackingReason::SessionRevoked(Object^ sender, ExtendedExecutionRevokedEventArgs^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        switch (args->Reason)
        {
        case ExtendedExecutionRevokedReason::Resumed:
            rootPage->NotifyUser("Extended execution revoked due to returning to foreground.", NotifyType::StatusMessage);
            break;

        case ExtendedExecutionRevokedReason::SystemPolicy:
            rootPage->NotifyUser("Extended execution revoked due to system policy.", NotifyType::StatusMessage);
            break;
        }

        EndExtendedExecution();
    }));
}
