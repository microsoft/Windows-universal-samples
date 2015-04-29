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

namespace BackgroundTask
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_BackgroundTask sealed :
        public Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        Scenario1_BackgroundTask();
        virtual ~Scenario1_BackgroundTask();

        virtual void Run(
            Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);

    private:
        void OnCanceled(
            Windows::ApplicationModel::Background::IBackgroundTaskInstance^ sender,
            Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason);

        void ReadingChanged(
            Windows::Devices::Sensors::Accelerometer^ sender,
            Windows::Devices::Sensors::AccelerometerReadingChangedEventArgs^ e);

    private:
        // The accelerometer we will be subscribing to.
        Windows::Devices::Sensors::Accelerometer^ _accelerometer;

        // Used to defer the task from completion until it is canceled.
        Platform::Agile<Windows::ApplicationModel::Background::BackgroundTaskDeferral^> _deferral;

        // Used to track the number of received accelerometer readings.
        uint64 _sampleCount;

        // Used for unregistering from the AccelerometerReadingChanged event.
        Windows::Foundation::EventRegistrationToken _readingToken;
    };
}