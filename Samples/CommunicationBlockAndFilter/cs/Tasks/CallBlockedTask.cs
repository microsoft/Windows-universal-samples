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

using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Calls.Background;

namespace BlockAndFilterTasks
{
    public sealed class CallBlockedTask : IBackgroundTask
    {
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            // This sample displays a toast to notify the user that a call was blocked.

            PhoneCallBlockedTriggerDetails triggerDetails =
                (PhoneCallBlockedTriggerDetails)taskInstance.TriggerDetails;

            string info = "Blocked call: " + triggerDetails.PhoneNumber;

            Toast.Send(info);
        }
    }
}
