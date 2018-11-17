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
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Calls;
using Windows.ApplicationModel.CommunicationBlocking;
using Windows.Storage;

namespace BlockAndFilterTasks
{
    public sealed class ActivatedTask : IBackgroundTask
    {
        public async void Run(IBackgroundTaskInstance taskInstance)
        {
            var deferral = taskInstance.GetDeferral();

            // The user selected this app to be the communication blocking app.
            // Set the list of blocked numbers.
            if (CommunicationBlockingAppManager.IsCurrentAppActiveBlockingApp)
            {
                // If there are no saved blocked numbers, then default to an empty list.
                var blockedNumbers = ApplicationData.Current.LocalSettings.Values["blockedNumbers"] as string[];
                await PhoneCallBlocking.SetCallBlockingListAsync(blockedNumbers ?? new string[] { });
            }
            deferral.Complete();
        }
    }
}
