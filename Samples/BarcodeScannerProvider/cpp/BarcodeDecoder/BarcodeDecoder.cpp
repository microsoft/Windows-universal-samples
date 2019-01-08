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
#include "BarcodeDecoder.h"
#include "Provider.h"

using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Devices::PointOfService::Provider;

namespace BarcodeDecoder
{
    Provider^ provider = nullptr;

    void BarcodeDecodeTask::Run(IBackgroundTaskInstance^ taskInstance)
    {
        provider = ref new Provider(taskInstance);
    }
}
