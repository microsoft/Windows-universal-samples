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

namespace BarcodeDecoder
{
    public sealed class BarcodeDecodeTask : IBackgroundTask
    {
        Provider provider = null;

        public void Run(IBackgroundTaskInstance taskInstance)
        {
            provider = new Provider(taskInstance);
        }
    }
}
