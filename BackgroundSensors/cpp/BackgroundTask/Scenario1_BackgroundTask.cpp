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
#include "Scenario1_BackgroundTask.h"

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Devices::Background;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Storage;

using namespace BackgroundTask;

Scenario1_BackgroundTask::Scenario1_BackgroundTask() :
    _sampleCount(0)
{
}

Scenario1_BackgroundTask::~Scenario1_BackgroundTask()
{
}

/// <summary> 
/// Background task entry point.
/// </summary> 
/// <param name="taskInstance"></param>
void Scenario1_BackgroundTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    _accelerometer = Accelerometer::GetDefault();

    if (nullptr != _accelerometer)
    {
        // Select a report interval that is both suitable for the purposes
        // of the app and supported by the sensor.
        uint32 minReportIntervalMsecs = _accelerometer->MinimumReportInterval;
        _accelerometer->ReportInterval = minReportIntervalMsecs > 16 ? minReportIntervalMsecs : 16;

        // Subscribe to accelerometer ReadingChanged events.
        _readingToken = _accelerometer->ReadingChanged::add(
            ref new TypedEventHandler<Accelerometer^, AccelerometerReadingChangedEventArgs^>(
                this, &Scenario1_BackgroundTask::ReadingChanged));

        // Take a deferral that is released when the task is completed.
        _deferral = Platform::Agile<BackgroundTaskDeferral^>(taskInstance->GetDeferral());

        // Get notified when the task is canceled.
        taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(
            this, &Scenario1_BackgroundTask::OnCanceled);

        // Store a setting so that the app knows that the task is running.
        ApplicationData::Current->LocalSettings->Values->Insert("IsBackgroundTaskActive",
            dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(true)));
    }
}

/// <summary> 
/// Called when the background task is canceled by the app or by the system.
/// </summary> 
/// <param name="sender"></param>
/// <param name="reason"></param>
void Scenario1_BackgroundTask::OnCanceled(IBackgroundTaskInstance^ /*sender*/,
    BackgroundTaskCancellationReason reason)
{
    ApplicationData::Current->LocalSettings->Values->Insert("TaskCancelationReason",
        reason.ToString());

    ApplicationData::Current->LocalSettings->Values->Insert("SampleCount",
        dynamic_cast<PropertyValue^>(PropertyValue::CreateUInt64(_sampleCount)));

    ApplicationData::Current->LocalSettings->Values->Insert("IsBackgroundTaskActive",
        dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(false)));

    if (nullptr != _accelerometer)
    {
        _accelerometer->ReadingChanged::remove(_readingToken);
    }

    // Complete the background task (this raises the OnCompleted event on the corresponding
    // BackgroundTaskRegistration).
    _deferral->Complete();
}

/// <summary>
/// This is the event handler for acceleroemter ReadingChanged events.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_BackgroundTask::ReadingChanged(Accelerometer^ sender,
    AccelerometerReadingChangedEventArgs^ e)
{
    _sampleCount++;

    // Save the sample count if the foreground app is visible.
    bool appVisible = (bool)ApplicationData::Current->LocalSettings->Values->Lookup("IsAppVisible");
    if (appVisible)
    {
        ApplicationData::Current->LocalSettings->Values->Insert("SampleCount",
            dynamic_cast<PropertyValue^>(PropertyValue::CreateUInt64(_sampleCount)));
    }
}
