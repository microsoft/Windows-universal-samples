//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Collections.Generic;

namespace BackgroundTask
{
    internal class LocalSettingKeys
    {
        public const String SyncBackgroundTaskStatus = "SyncBackgroundTaskStatus";
        public const String SyncBackgroundTaskResult = "SyncBackgroundTaskResult";
    }

    internal class BackgroundTaskInformation
    {
        public const String TaskCanceled = "Canceled";
        public const String TaskCompleted = "Completed";
    }

    internal class Sync
    {
        public const UInt32 BytesToWriteAtATime = 512;
        public const UInt32 NumberOfTimesToWrite = 2;
    }
}
