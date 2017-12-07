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
    public ref class VisitBackgroundTask sealed : public Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        VisitBackgroundTask();

        virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);

    private:
        void GetVisitReports(Windows::Devices::Geolocation::GeovisitTriggerDetails^ triggerDetails);
        void DoToast(int numEventsOfInterest, Platform::String^ eventName);
        void FillReportCollectionWithExistingReports();
        void SaveExistingEvents();
        void AddVisitReport(Platform::String^ eventDescription);
        ~VisitBackgroundTask();

        Platform::Collections::Vector<Platform::String^>^ _visitBackgroundEvents;
    };
}
