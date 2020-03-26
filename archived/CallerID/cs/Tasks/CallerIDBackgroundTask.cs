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

using System;
using System.Diagnostics;
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Calls.Background;
using Windows.ApplicationModel.Calls.Provider;

namespace CallerIDBackgroundTask
{
    public sealed class SampleCallerIDBackgroundTask : IBackgroundTask
    {
        //
        // The Run method is the entry point of a background task.
        //
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            // Associate a cancellation handler with the background task.
            // Even though this task isn't performing much work, it can still be cancelled.
            taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

            // Do the background task activity.
            BackgroundTaskDeferral _deferral = taskInstance.GetDeferral();

            PhoneCallOriginDataRequestTriggerDetails callDetails = (PhoneCallOriginDataRequestTriggerDetails)taskInstance.TriggerDetails;
            PhoneCallOrigin phoneCallOrigin = LocalLookupForCallerData(callDetails.PhoneNumber);
            PhoneCallOriginManager.SetCallOrigin(callDetails.RequestId, phoneCallOrigin);

            _deferral.Complete();
        }

        private PhoneCallOrigin LocalLookupForCallerData(String phoneNumber)
        {
            // Fake origin data for each incoming call.
            PhoneCallOrigin phoneCallOrigin = new PhoneCallOrigin();
            phoneCallOrigin.DisplayName = "Contoso";
            phoneCallOrigin.Location = "Redmond, Washington, USA";
            phoneCallOrigin.Category = "IT Services";            
            phoneCallOrigin.CategoryDescription = "Information technology services";

            return phoneCallOrigin;
        }

        //
        // Handles background task cancellation.
        //
        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            // Normally, we would signal the task to exit.
            // However, our sample task has no async operations and is so short that
            // we'll just let it run to completion instead of trying to cancel it.
        }
    }
}
