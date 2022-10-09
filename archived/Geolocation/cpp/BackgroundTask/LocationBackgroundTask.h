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

#pragma once

#include "pch.h"

namespace BackgroundTask
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class LocationBackgroundTask sealed : public Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        LocationBackgroundTask();

        virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);

    private:
        void OnCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason);
        ~LocationBackgroundTask();

        Concurrency::cancellation_token_source geopositionTaskTokenSource;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class GeofenceBackgroundTask sealed : public Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        GeofenceBackgroundTask();

        virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
        void Invoke(Windows::Foundation::IAsyncAction^ asyncInfo, Windows::Foundation::AsyncStatus asyncStatus);

    private:
        void OnCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason);
        void GetGeofenceStateChangedReports(Windows::Devices::Geolocation::Geoposition^ pos);
        void DoToast(int numEventsOfInterest, Platform::String^ eventName);
        void FillEventCollectionWithExistingEvents();
        void SaveExistingEvents();
        void AddEventDescription(Platform::String^ eventDescription);
        ~GeofenceBackgroundTask();

        Concurrency::cancellation_token_source geopositionTaskTokenSource;
        Platform::Collections::Vector<Platform::String^>^ geofenceBackgroundEvents;
    };
}
